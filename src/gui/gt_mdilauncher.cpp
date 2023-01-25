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
#include <QTabWidget>
#include <QMdiSubWindow>
#include <QDebug>

#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_algorithms.h"
#include "gt_icons.h"

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    m_area(nullptr)
{

}

QString
GtMdiLauncher::generateMdiItemId(GtMdiItem* mdiItem)
{
    if (!mdiItem)
    {
        return {};
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
            if (mdiItem->m_d)
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
    if (!mdiItem)
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

    if (!m_area)
    {
        return;
    }

    QList<QWidget*> list;
    for (int i = 0; i < m_area->count(); ++i)
    {
        if (m_area->widget(i))
        {
            list.append(m_area->widget(i));
        }
    }

    gt::for_each_key(m_openItems, [&](const QObject* e)
    {
        if (generateMdiItemId(m_openItems.value(e)) == mdiId)
        {
            foreach (QWidget* listItem, list)
            {
                if (listItem == e)
                {
                    m_area->setCurrentWidget(listItem);
                    listItem->setFocus();
                }
            }
        }
    });

}

void
GtMdiLauncher::onSubWindowClose(QObject* obj)
{
    if (m_openItems.contains(obj))
    {
        m_openItems.remove(obj);
    }
}

void
GtMdiLauncher::onCloseTabRequest(int i)
{
    if (QWidget* w = m_area->widget(i))
    {
        m_area->removeTab(i);
        delete w;

        // focus the new current widget to recieve input etc.
        if (QWidget* next = m_area->currentWidget())
        {
            next->setFocus();
        }
    }
}

GtMdiLauncher*
GtMdiLauncher::instance()
{
    static GtMdiLauncher* retval = nullptr;
    if (!retval)
    {
        retval = new GtMdiLauncher(qApp);
    }
    return retval;
}

void
GtMdiLauncher::setMdiArea(QTabWidget* area)
{
    if (m_area)
    {
        return;
    }

    m_area = area;

    connect(m_area, SIGNAL(tabCloseRequested(int)), this,
            SLOT(onCloseTabRequest(int)));
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
        return {};
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return {};
    }

    GtCollectionInterface* coll = m_collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionStructure();
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    if (!m_collections.contains(id))
    {
        return nullptr;
    }

    return m_collections.value(id);
}

void
GtMdiLauncher::print(QWidget* subWindow)
{
    if (!subWindow)
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

    if (m_dockWidgets.contains(metaObj.className()))
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
GtMdiLauncher::open(const QString& id, GtObject* data, const QString& customId)
{
    if (!m_area)
    {
        return nullptr;
    }

    if (!knownClass(id))
    {
        return nullptr;
    }

    GtObject* obj = newObject(id);

    if (!obj)
    {
        return nullptr;
    }

    GtMdiItem* mdiItem = qobject_cast<GtMdiItem*>(obj);

    if (!mdiItem)
    {
        delete obj;
        return nullptr;
    }

    if (data)
    {
        mdiItem->m_d = data;
    }

    if (!customId.isEmpty())
    {
        mdiItem->m_cid = customId;
    }

    if (!mdiItemAllowed(mdiItem))
    {
        if (data && mdiItem->objectName() == "Result Viewer")
        {
            QList<GtMdiItem*> openItems = m_openItems.values();

            foreach (GtMdiItem* openItem, openItems)
            {
                if (openItem->objectName() == mdiItem->objectName())
                { // cppcheck-suppress useStlAlgorithm
                    emit openItem->tryReopening(openItem->m_d.data());
                    break;
                }
            }
        }

        setFocus(generateMdiItemId(mdiItem));

        delete obj;
        return nullptr;
    }

    QWidget* wid = mdiItem->widget();

    QIcon icon = mdiItem->icon();

    if (!icon.isNull())
    {       
        m_area->addTab(wid, mdiItem->icon(), mdiItem->objectName());
    }
    else
    {
        m_area->addTab(wid, gt::gui::icon::frame(), mdiItem->objectName());
    }

    mdiItem->initialized();

    connect(wid, SIGNAL(destroyed(QObject*)), mdiItem,
           SLOT(deleteLater()));
    connect(wid, SIGNAL(destroyed(QObject*)),
            SLOT(onSubWindowClose(QObject*)));
    connect(mdiItem, &GtMdiItem::objectNameChanged,
            this, &GtMdiLauncher::changeTabTitle);

    connect(mdiItem, SIGNAL(destroyed(QObject*)), wid,
            SLOT(deleteLater()));
    connect(gtApp, SIGNAL(themeChanged(bool)), mdiItem,
            SLOT(onThemeChanged()));

    m_openItems.insert(wid, mdiItem);

    mdiItem->setSubWin(wid);

    m_area->setCurrentWidget(wid);
    wid->show();
    mdiItem->showEvent();

    if (data)
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

void
GtMdiLauncher::changeTabTitle(const QString& newTitle)
{
    assert(m_area);
    int index = -1;

    /// if this function was called by a connection
    if (sender())
    {
        if (GtMdiItem* item = qobject_cast<GtMdiItem*>(sender()))
        {
            index = m_area->indexOf(item->widget());
        }
    }
    /// else use the current mdi item
    else
    {
        index = m_area->currentIndex();
    }

    /// if index can be found:
    if (index > -1)
    {
        m_area->setTabText(index, newTitle);
    }
}
