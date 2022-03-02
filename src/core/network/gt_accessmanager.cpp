/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessmanager.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_networkinterface.h"
#include "gt_accessgroup.h"
#include "gt_logging.h"

#include "gt_accessmanager.h"

#include <QNetworkInterface>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QNetworkAccessManager>
#include <QDataStream>

#include <algorithm>

GtAccessManager::GtAccessManager(QObject* parent) :
    QObject(parent), m_qnam(nullptr)
{
}

bool
GtAccessManager::loadAccessData(GtAccessGroup* accessGroup)
{
    if (!accessGroup)
    {
        return false;
    }

    QString path = roamingPath() + QDir::separator() +
                   QStringLiteral("access");

    QDir dir(path);

    if (!dir.exists())
    {
        return true;
    }

    if (accessGroup->objectName().isEmpty())
    {
        gtError() << "Invalid group id!";
        return false;
    }

    QString id = accessGroup->objectName().replace(QStringLiteral(" "),
                                                   QStringLiteral("_"));
    id = id.toLower();
    QString filename = id + QStringLiteral(".gtacc");

    QFile file(dir.absoluteFilePath(filename));

    if (!file.exists())
    {
        return true;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        gtError() << "Could not open access data file! ("
                  << filename << ")";
        return false;
    }

    QDataStream stream(&file);

    QString in;

    stream >> in;

    QStringList list = deserializeStringList(in);

    // number of header lines
    const int nhl = list.size() % 4;

    if (list.size() >= 1 && nhl == 1)
    {
        qDebug() << "   |-> reading data...";

        list.removeFirst();

        while (!list.isEmpty())
        {
            if (list.size() >= 4)
            {
                accessGroup->addAccessData(list.value(0),
                                           list.value(1).toInt(),
                                           list.value(2),
                                           list.value(3));

                list.removeFirst();
                list.removeFirst();
                list.removeFirst();
                list.removeFirst();
            }
            else
            {
                list.clear();
            }
        }
    }
    else
    {
        gtError() << "Invalid access data file! ("
                  << filename << ") -  deserialization error!";
        return false;
    }

    file.close();

    return true;
}

QString
GtAccessManager::roamingPath()
{
#ifdef _WIN32
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
    return QStandardPaths::writableLocation(
               QStandardPaths::GenericConfigLocation);
}

GtAccessManager*
GtAccessManager::instance()
{
    static GtAccessManager* retval = nullptr;
    if (!retval)
    {
        retval = new GtAccessManager(qApp);
    }
    return retval;
}

GtAccessGroup*
GtAccessManager::addAccessGroup(const QString& id,
                                const QMetaObject& connection)
{
    if (id.isEmpty())
    {
        return nullptr;
    }

    if (groupExists(id))
    {
        return nullptr;
    }

    GtAccessGroup* retval = new GtAccessGroup(id , connection, this);
    m_data << retval;

    loadAccessData(retval);

    return retval;
}

int
GtAccessManager::numberOfDataGroups()
{
    return m_data.size();
}

bool
GtAccessManager::isEmpty()
{
    return m_data.isEmpty();
}

bool
GtAccessManager::groupExists(const QString& id)
{
    return std::any_of(std::begin(m_data), std::end(m_data), [&id](const GtAccessGroup* group) {
        return group->objectName() == id;
    });
}

QStringList
GtAccessManager::accessGroupIds()
{
    QStringList retval;

    foreach (GtAccessGroup* group, m_data)
    {
        retval << group->objectName();
    }

    return retval;
}

const QList<GtAccessGroup*>&
GtAccessManager::accessGroups()
{
    return m_data;
}

bool
GtAccessManager::saveAccessData()
{
    QString path = roamingPath() + QDir::separator() +
                   QStringLiteral("access");

    QDir dir(path);

    if (!dir.exists())
    {
        if (!dir.mkpath(path))
        {
            gtError() << "Could not create access data directory!";
            return false;
        }
    }

    QList<GtAccessGroup*> groups = accessGroups();

    foreach (GtAccessGroup* group, groups)
    {
        if (group->objectName().isEmpty())
        {
            gtError() << "Invalid group id!";
            continue;
        }

        QString id = group->objectName().replace(QStringLiteral(" "),
                                                 QStringLiteral("_"));
        id = id.toLower();
        QString filename = id + QStringLiteral(".gtacc");

        QFile file(dir.absoluteFilePath(filename));

        if (file.exists())
        {
            if (!file.remove())
            {
                gtError() << "Could not remove old access data file! ("
                          << filename << ")";
                continue;
            }
        }

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            gtError() << "Could not open access data file! ("
                      << filename << ")";
            continue;
        }

        QStringList output;
        output << group->objectName();

        foreach (GtAccessData data, group->accessData())
        {
            output << data.host();
            output << QString::number(data.port());
            output << data.user();
            output << data.password();
        }

        QString outStr = serializeStringList(output);

        QDataStream out(&file);

        out << outStr;

        file.close();
    }

    return true;
}

QString
GtAccessManager::macAddress()
{
    foreach (QNetworkInterface netInterface, QNetworkInterface::allInterfaces())
    {
        // Return only the first non-loopback MAC Address
        if (!(netInterface.flags() & QNetworkInterface::IsLoopBack))
        {
            return netInterface.hardwareAddress();
        }
    }

    return QString();
}

QString
GtAccessManager::serializeStringList(const QStringList& list)
{
    QByteArray byteArray;
    QDataStream out(&byteArray, QIODevice::WriteOnly);
    out << list;

    return QString::fromLatin1(byteArray.toBase64());
}

QStringList
GtAccessManager::deserializeStringList(const QString& str)
{
    QStringList sampleData;
    QByteArray byteArray = QByteArray::fromBase64(str.toUtf8());
    QDataStream in(&byteArray, QIODevice::ReadOnly);
    in >> sampleData;

    return sampleData;
}

GtAccessGroup*
GtAccessManager::accessGroup(const QString& id)
{
    foreach (GtAccessGroup* group, m_data)
    {
        if (group->objectName() == id)
        {
            return group;
        }
    }

    return nullptr;
}

QNetworkAccessManager*
GtAccessManager::qnam()
{
    if (!m_qnam)
    {
        m_qnam = new QNetworkAccessManager(this);
    }

    return m_qnam;
}
