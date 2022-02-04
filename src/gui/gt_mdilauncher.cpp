/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDebug>

#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_abstractcollectionsettings.h"

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    m_area(Q_NULLPTR)
{

}

QString
GtMdiLauncher::generateMdiItemId(GtMdiItem* mdiItem)
{
    if (mdiItem == Q_NULLPTR)
    {
        return QString();
    }

    QString retval = mdiItem->metaObject()->className();

    if (!mdiItem->m_cid.isEmpty())
    {
        retval = retval + QStringLiteral("#") + mdiItem->m_cid;
    }
    else
    {
        QString customMdiId = mdiItem->customId(mdiItem->m_d);

        if (!customMdiId.isEmpty())
        {
            retval = retval + QStringLiteral("#") + customMdiId;
        }
        else
        {
            if (mdiItem->m_d != Q_NULLPTR)
            {
                retval = retval + QStringLiteral("#") + mdiItem->m_d->uuid();
            }
        }
    }

    return retval;
}

bool
GtMdiLauncher::mdiItemAllowed(GtMdiItem* mdiItem)
{
    if (mdiItem == Q_NULLPTR)
    {
        return false;
    }

    if (mdiItem->allowsMultipleInstances())
    {
        return true;
    }

    QString mdiId = generateMdiItemId(mdiItem);

    if (mdiId.isEmpty())
    {
        return false;
    }

    if (openItemIds().contains(mdiId))
    {
        return false;
    }

    return true;
}

QStringList
GtMdiLauncher::openItemIds()
{
    QStringList retval;

    QList<GtMdiItem*> openItems = m_openItems.values();

    foreach (GtMdiItem* openItem, openItems)
    {
        retval << generateMdiItemId(openItem);
    }

    return retval;
}

void
GtMdiLauncher::setFocus(const QString& mdiId)
{
    if (mdiId.isEmpty())
    {
        return;
    }

    if (m_area == Q_NULLPTR)
    {
        return;
    }

    QList<QMdiSubWindow*> list = m_area->subWindowList();

    for (auto e : m_openItems.keys())
    {
        if (generateMdiItemId(m_openItems.value(e)) == mdiId)
        {
            foreach (QMdiSubWindow* listItem, list)
            {
                if (listItem == e)
                {
                    listItem->setFocus();
                }
            }
        }
    }
}

void
GtMdiLauncher::onSubWindowClose(QObject* obj)
{
    if (m_openItems.contains(obj))
    {
        m_openItems.remove(obj);
    }
}

GtMdiLauncher*
GtMdiLauncher::instance()
{
    static GtMdiLauncher* retval = Q_NULLPTR;
    if (retval == Q_NULLPTR)
    {
        retval = new GtMdiLauncher(qApp);
    }
    return retval;
}

void
GtMdiLauncher::setMdiArea(QMdiArea* area)
{
    if (m_area)
    {
        return;
    }

    m_area = area;
}

QStringList
GtMdiLauncher::dockWidgetIds()
{
    return m_dockWidgets.keys();
}

QMetaObject
GtMdiLauncher::dockWidget(const QString& id)
{
    return m_dockWidgets.value(id);
}

QStringList
GtMdiLauncher::collectionIds()
{
    return m_collections.keys();
}

QIcon
GtMdiLauncher::collectionIcon(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return QIcon();
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (coll == Q_NULLPTR)
    {
        return QIcon();
    }

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return QMap<QString, QMetaType::Type>();
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (coll == Q_NULLPTR)
    {
        return QMap<QString, QMetaType::Type>();
    }

    return coll->collectionStructure();
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return Q_NULLPTR;
    }

    return m_collections.value(id);
}

void
GtMdiLauncher::print(QMdiSubWindow* subWindow)
{
    if (subWindow == Q_NULLPTR)
    {
        return;
    }

    if (m_openItems.contains(subWindow))
    {
        m_openItems.value(subWindow)->print();
    }
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj)
{

    if (m_dockWidgets.keys().contains(metaObj.className()))
    {
        qWarning() << tr("Dockwidget") << metaObj.className()
                   << tr("already registered");
        return false;
    }

    m_dockWidgets.insert(metaObj.className(), metaObj);

    return true;
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    foreach (const QMetaObject& mobj, metaData)
    {
        QString classname = mobj.className();
        if (m_dockWidgets.contains(classname))
        {
            qWarning() << "dock widget class name already exists!";
            return true;
        }
    }
    return false;
}

bool
GtMdiLauncher::registerDockWidgets(const QList<QMetaObject>& metaData)
{
    if (dockWidgetsExist(metaData))
    {
        return false;
    }

    foreach (const QMetaObject& metaObj, metaData)
    {
        m_dockWidgets.insert(metaObj.className(), metaObj);
    }

    return true;
}

bool
GtMdiLauncher::registerCollection(const QString& str,
                                  GtCollectionInterface* coll)
{
    if (m_collections.contains(str))
    {
        return false;
    }

    m_collections.insert(str, coll);

    return true;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, QString customId)
{
    if (m_area == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (!knownClass(id))
    {
        return Q_NULLPTR;
    }

    GtObject* obj = newObject(id);

    if (!obj)
    {
        return Q_NULLPTR;
    }

    GtMdiItem* mdiItem = qobject_cast<GtMdiItem*>(obj);

    if (!mdiItem)
    {
        delete obj;
        return Q_NULLPTR;
    }

    if (data != Q_NULLPTR)
    {
        mdiItem->m_d = data;
    }

    if (!customId.isEmpty())
    {
        mdiItem->m_cid = customId;
    }

    qDebug()  << "mdiItem id = " << generateMdiItemId(mdiItem);

    if (!mdiItemAllowed(mdiItem))
    {
        if ((data != nullptr) && (mdiItem->objectName() == "Result Viewer"))
        {
            QList<GtMdiItem*> openItems = m_openItems.values();

            foreach (GtMdiItem* openItem, openItems)
            {
                if (openItem->objectName() == mdiItem->objectName())
                {
                    emit openItem->tryReopening(openItem->m_d.data());
					break;
                }
            }
        }

        setFocus(generateMdiItemId(mdiItem));

        delete obj;
        return Q_NULLPTR;
    }

    QWidget* wid = mdiItem->widget();

    QMdiSubWindow* subWin = m_area->addSubWindow(wid);
    subWin->setWindowTitle(mdiItem->objectName());

    QIcon icon = mdiItem->icon();

    if (!icon.isNull())
    {
        subWin->setWindowIcon(mdiItem->icon());
    }
    else
    {
        subWin->setWindowIcon(gtApp->icon(QStringLiteral("frameIcon.png")));
    }

    mdiItem->initialized();

    connect(subWin,
            SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)),
            mdiItem,
            SLOT(windowStateChanged(Qt::WindowStates, Qt::WindowStates)));
    connect(subWin, SIGNAL(aboutToActivate()), mdiItem,
            SLOT(windowAboutToActive()));
    connect(subWin, SIGNAL(destroyed(QObject*)), mdiItem,
           SLOT(deleteLater()));
    connect(subWin, SIGNAL(destroyed(QObject*)),
            SLOT(onSubWindowClose(QObject*)));
    connect(mdiItem, SIGNAL(objectNameChanged(QString)),
            subWin, SLOT(setWindowTitle(QString)));
    connect(mdiItem, SIGNAL(destroyed(QObject*)), subWin,
            SLOT(deleteLater()));
    connect(gtApp, SIGNAL(themeChanged(bool)), mdiItem,
            SLOT(onThemeChanged()));

    m_openItems.insert(subWin, mdiItem);

    mdiItem->setSubWin(subWin);

    subWin->show();
    mdiItem->showEvent();

    if (data != Q_NULLPTR)
    {
        mdiItem->setData(data);
    }

    return mdiItem;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, const QString& customId)
{
    return open(id, nullptr, customId);
}
