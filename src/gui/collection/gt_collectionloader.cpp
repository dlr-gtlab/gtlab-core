/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionloader.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 09.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>

#include "gt_logging.h"
#include "gt_collectionnetworkitem.h"
#include "gt_mdilauncher.h"
#include "gt_collectioninterface.h"
#include "gt_collectionreply.h"
#include "gt_collectionhelper.h"
#include "gt_downloaddialog.h"

#include "gt_collectionloader.h"

GtCollectionLoader::GtCollectionLoader(const QString& collectionId,
                                       QObject* parent) : QObject(parent)
{
    m_collection = gtMdiLauncher->collection(collectionId);
}

QList<GtCollectionItem>
GtCollectionLoader::loadLocalCollection()
{
    QList<GtCollectionItem> retval;

    if (!m_collection)
    {
        gtError() << tr("Invalid collection!");
        return retval;
    }

    const QString colPath = collectionPath();

    if (colPath.isEmpty())
    {
        gtError() << tr("Invalid collection path!");
        return retval;
    }

    QDirIterator directories(colPath,
                             QDir::Dirs |
                             QDir::NoSymLinks |
                             QDir::NoDotAndDotDot,
                             QDirIterator::Subdirectories);

    while (directories.hasNext())
    {
        directories.next();
        GtCollectionItem item = loadLocalCollectionItem(directories.filePath());

        if (item.isValid())
        {
//            gtDebug() << "item successfully loaded!";
//            gtDebug() << "  |-> " << item.ident();
//            gtDebug() << "  |-> " << item.version();
//            gtDebug() << "  |-> " << item.propertyIds();
            retval << item;
        }

        else
        {
            gtError() << tr("Invalid collection item!") <<
                      QStringLiteral(" - ") << directories.filePath();
        }
    }

    return retval;
}

GtCollectionReply*
GtCollectionLoader::loadNetworkCollection(
    const QList<GtAccessData>& accessDataList)
{
    GtCollectionReply* reply = new GtCollectionReply(this);

    GtCollectionHelper* helper = new GtCollectionHelper(accessDataList,
            reply);

    QThread* thread = new QThread;

    connect(thread, SIGNAL(started()), helper, SLOT(exec()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(helper, SIGNAL(finished()), SLOT(onHelperFinished()));
    connect(helper, SIGNAL(finished()), thread, SLOT(quit()));
    //    connect(helper, SIGNAL(finished()), reply, SLOT(onHelperFinished()));

    thread->start();

    return reply;
}

void
GtCollectionLoader::downloadCollectionItems(
    const QList<GtCollectionNetworkItem>& items)
{
    if (items.isEmpty())
    {
        return;
    }

    const QString colPath = collectionPath();

    if (colPath.isEmpty())
    {
        gtError() << tr("Invalid collection path!");
        return;
    }

    GtDownloadDialog dialog;

    foreach (GtCollectionNetworkItem item, items)
    {
        QString target = colPath + QDir::separator() + item.uuid();
        QUrl url = item.url();
        QString urlStr = url.toDisplayString();
        QStringList files = item.files();
        files << QStringLiteral("index.json");

        foreach (const QString& filename, files)
        {
            if (urlStr.endsWith(QStringLiteral("/")))
            {
                url.setUrl(QString(urlStr + filename));
            }
            else
            {
                url.setUrl(QString(urlStr + QStringLiteral("/") + filename));
            }

            dialog.addFileDownload(url, target, filename);
        }
    }

    dialog.exec();
}

QString
GtCollectionLoader::collectionPath()
{
    if (!m_collection)
    {
        gtError() << tr("Invalid collection!");
        return QString();
    }

    QDir dir(qApp->applicationDirPath());

    if (!dir.cdUp())
    {
        gtError() << tr("Could not reach collection path!");
        return QString();
    }

    dir.setPath(dir.absolutePath() + QDir::separator() +
                QStringLiteral("Collections"));

    if (!dir.exists())
    {
        if (!dir.mkpath(dir.absolutePath()))
        {
            gtError() << tr("Could not create collection root path!");
            return QString();
        }
    }

    dir.setPath(dir.absolutePath() + QDir::separator() +
                m_collection->collectionId());

    if (!dir.exists())
    {
        if (!dir.mkpath(dir.absolutePath()))
        {
            gtError() << tr("Could not create collection path!");
            return QString();
        }
    }

    return dir.absolutePath();
}

GtCollectionItem
GtCollectionLoader::loadLocalCollectionItem(const QString& path)
{
    if (path.isEmpty())
    {
        return GtCollectionItem();
    }

//    gtDebug() << "loading " << path << "...";

    QString filepath = path + QDir::separator() + QStringLiteral("index.json");

    QFile file(filepath);

    if (!file.exists())
    {
        gtError() << tr("Could not find json file!") << QStringLiteral(" - ") <<
                  path;
        return GtCollectionItem();
    }

    if (!file.open(QFile::ReadOnly))
    {
        gtError() << tr("Could not open json file!") << QStringLiteral(" - ") <<
                  path;
        return GtCollectionItem();
    }

    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(file.readAll(), &error));

    file.close();

    if (loadDoc.isEmpty())
    {
        gtError() << tr("Invalid json file!") << QStringLiteral(" - ") <<
                  path;
        return GtCollectionItem();
    }

    const QJsonObject jsonObj = loadDoc.object();

    if (!itemIsValid(jsonObj))
    {
        gtError() << tr("Invalid json structure!") << QStringLiteral(" - ") <<
                  path;
        return GtCollectionItem();
    }

    GtCollectionItem retval = readItemInformation(jsonObj);

    retval.m_localPath = path;

    return retval;
}

GtCollectionItem
GtCollectionLoader::readItemInformation(const QJsonObject& json)
{
    if (!m_collection)
    {
        gtError() << tr("Invalid collection!");
        return GtCollectionItem();
    }

    const QString ident = json[QStringLiteral("ident")].toString();

    const QString desc = json[QStringLiteral("description")].toString();

    const QString uuid = json[QStringLiteral("uuid")].toString();

    const double version = json[QStringLiteral("version")].toDouble(-1.);

    QStringList fileNames;

    QJsonArray filesArray = json[QStringLiteral("files")].toArray();

    for (int i = 0; i < filesArray.size(); i++)
    {
        fileNames << filesArray.at(i).toString();
    }

    if (ident.isEmpty() || uuid.isEmpty() || version < 0. ||
            fileNames.isEmpty())
    {
        gtError() << tr("Invalid json file!");
        return GtCollectionItem();
    }

    GtCollectionItem retval;

    retval.m_ident = ident;
    retval.m_description = desc;
    retval.m_uuid = uuid;
    retval.m_version = version;
    retval.m_files = fileNames;

    QMap<QString, QMetaType::Type> colStruct =
        m_collection->collectionStructure();

    for (auto const& e : colStruct.keys())
    {
        if (json.contains(e))
        {
            QJsonValue val = json.value(e);
            QVariant var = val.toVariant();

            retval.m_properties.insert(e, var);
        }
    }

    return retval;
}

GtCollectionNetworkItem
GtCollectionLoader::readNetworkItemInformation(const QUrl& url,
        const QJsonObject& json)
{
    if (!m_collection)
    {
        gtError() << tr("Invalid collection!");
        return GtCollectionNetworkItem();
    }

    const QString ident = json[QStringLiteral("ident")].toString();

    const QString desc = json[QStringLiteral("description")].toString();

    const QString uuid = json[QStringLiteral("uuid")].toString();

    const double version = json[QStringLiteral("version")].toDouble(-1.);

    QStringList fileNames;

    QJsonArray filesArray = json[QStringLiteral("files")].toArray();

    for (int i = 0; i < filesArray.size(); i++)
    {
        fileNames << filesArray.at(i).toString();
    }

    if (ident.isEmpty() || uuid.isEmpty() || version < 0. ||
            fileNames.isEmpty())
    {
        gtError() << tr("Invalid json file!");
        return GtCollectionNetworkItem();
    }

    GtCollectionNetworkItem retval;

    retval.m_ident = ident;
    retval.m_description = desc;
    retval.m_uuid = uuid;
    retval.m_version = version;
    retval.m_files = fileNames;
    retval.m_url = url;

    QMap<QString, QMetaType::Type> colStruct =
        m_collection->collectionStructure();

    for (auto const& e : colStruct.keys())
    {
        if (json.contains(e))
        {
            QJsonValue val = json.value(e);
            QVariant var = val.toVariant();

            retval.m_properties.insert(e, var);
        }
    }

    return retval;
}

bool
GtCollectionLoader::itemIsValid(const QJsonObject& json)
{
    if (!m_collection)
    {
        gtError() << tr("Invalid collection!");
        return false;
    }

    if (!json.contains(QStringLiteral("ident")))
    {
        gtError() << tr("Identification string not found!");
        return false;
    }

    if (!json.contains(QStringLiteral("description")))
    {
        gtError() << tr("Identification string not found!");
        return false;
    }

    if (!json.contains(QStringLiteral("uuid")))
    {
        gtError() << tr("Identification string not found!");
        return false;
    }

    if (!json.contains(QStringLiteral("version")))
    {
        gtError() << tr("Identification string not found!");
        return false;
    }

    QMap<QString, QMetaType::Type> colStruct =
        m_collection->collectionStructure();

    for (auto const& e : colStruct.keys())
    {
        if (!json.contains(e))
        {
            gtError() << e << QStringLiteral(" ") << tr("not found!");
            return false;
        }

        QJsonValue val = json.value(e);
        QVariant var = val.toVariant();

        if (var.type() != colStruct.value(e))
        {
            gtError() << tr("Invalid property type!");
            return false;
        }
    }

    return true;
}

void
GtCollectionLoader::sortItems(const QList<GtCollectionNetworkItem>& items,
                              QList<GtCollectionNetworkItem>& installedItems,
                              QList<GtCollectionNetworkItem>& availableItems,
                              QList<GtCollectionNetworkItem>& updateAvailableItems)
{
    QList<GtCollectionItem> localCollection = loadLocalCollection();

    foreach (GtCollectionNetworkItem item, items)
    {
        bool itemInstalled = false;

        foreach (GtCollectionItem localItem, localCollection)
        {
            if (item.uuid() == localItem.uuid())
            {
                itemInstalled = true;

                item.m_installedVersion = localItem.version();
                item.m_localPath = localItem.localPath();

                if (item.version() > localItem.version())
                {
                    updateAvailableItems << item;
                }
                else
                {
                    installedItems << item;
                }

                break;
            }
        }

        if (!itemInstalled)
        {
            availableItems << item;
        }
    }
}

void
GtCollectionLoader::onHelperFinished()
{
    gtDebug() << "helper finished!";
    GtCollectionHelper* helper = qobject_cast<GtCollectionHelper*>(sender());

    if (!helper)
    {
        // TODO error
        return;
    }

    if (!helper->reply())
    {
        gtError() << tr("Invalid connection reply!");
        return;
    }

    QList<GtCollectionHelperData> colData = helper->collectionItems();

    QList<GtCollectionNetworkItem> items;

    foreach (GtCollectionHelperData obj, colData)
    {
        if (itemIsValid(obj.data()))
        {
            GtCollectionNetworkItem item =
                readNetworkItemInformation(obj.url(), obj.data());

            if (item.isValid())
            {
                items << item;
            }
            else
            {
                gtError() << tr("Invalid collection item!");
            }
        }
        else
        {
            gtError() << tr("Invalid collection item!");
        }
    }

    QList<GtCollectionNetworkItem> installedItems;
    QList<GtCollectionNetworkItem> availableItems;
    QList<GtCollectionNetworkItem> updateAvailableItems;

    sortItems(items, installedItems, availableItems, updateAvailableItems);

    gtDebug() << "  |-> " << items.size() << "items loaded!";

    helper->reply()->m_installedItems = installedItems;
    helper->reply()->m_availableItems = availableItems;
    helper->reply()->m_updateAvailableItems = updateAvailableItems;

    helper->reply()->replyReady();

    helper->deleteLater();
}
