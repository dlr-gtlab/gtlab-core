/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QMdiArea>
#include <QTabBar>
#include <QTabWidget>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>

#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_algorithms.h"
#include "gt_icons.h"
#include "gt_logging.h"

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

    const auto openItems = m_openItems.values();

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
GtMdiLauncher::close()
{
    // the ownership is quite a mess, deleting the widget
    for (GtMdiItem* item : m_openItems)
    {
        assert(item);
        assert(item->widget());
        item->widget()->deleteLater();
    }
    m_openItems.clear();
}

void
GtMdiLauncher::onSubWindowClose(QObject* obj)
{
    auto iter = m_openItems.find(obj);
    if (iter != m_openItems.end())
    {
        QPointer<GtMdiItem> item = iter.value();
        m_openItems.erase(iter);

        bool isDetached = !obj->parent();
        gtError() << "HERE BEFORE" << item->objectName() << obj;
        if (isDetached) item->deleteLater();
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
GtMdiLauncher::printCurrentWindow()
{
    if (!m_area)
    {
        return;
    }

    print(m_area->currentWidget());
}

void
GtMdiLauncher::print(QWidget* subWindow)
{
    if (!subWindow) return;

    if (m_openItems.contains(subWindow))
    {
        m_openItems.value(subWindow)->print();
    }
}

bool
GtMdiLauncher::isPrintable(QWidget* subWindow) const
{
    if (!subWindow)
    {
        return false;
    }

    if (m_openItems.contains(subWindow) && m_openItems.value(subWindow))
    {
        return m_openItems.value(subWindow)->isPrintable();
    }

    return false;
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj)
{
    if (m_dockWidgets.contains(metaObj.className()))
    {
        gtWarning().medium()
                << tr("Dockwidget '%1' was already registered")
                   .arg(metaObj.className());
        return false;
    }

    m_dockWidgets.insert(metaObj.className(), metaObj);

    return true;
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    for (const QMetaObject& mobj : metaData)
    {
        QString classname = mobj.className();
        if (m_dockWidgets.contains(classname))
        {
            gtWarning().medium()
                    << tr("Dockwidget '%1' was already registered")
                       .arg(classname);
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

    for (const QMetaObject& metaObj : metaData)
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
        gtWarning().medium() << tr("Unknown Mdi item '%1'!").arg(id);
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

    // add parent to this class for now
    mdiItem->setParent(this);

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
            const auto openItems = m_openItems.values();

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

    // move to widget
    mdiItem->setParent(wid);

    QIcon icon = mdiItem->icon();

    if (icon.isNull())
    {
        icon = gt::gui::icon::layers();
    }

    int idx = m_area->addTab(wid, icon, mdiItem->objectName());

    // set custom close button
    QPushButton* closeBtn = new QPushButton;
    closeBtn->setIconSize(QSize{14, 14}); // because stani wants it this way...
    closeBtn->resize(QSize(14, 14));
    closeBtn->setIcon(gt::gui::icon::close());
    closeBtn->setFlat(true);
    // for identification in gui tests
    closeBtn->setObjectName(QStringLiteral("MdiTabCloseBtn"));

    QPushButton* undockBtn = new QPushButton;
    undockBtn->setIcon(gt::gui::icon::dock());
    undockBtn->setIconSize(QSize{14, 14});
    undockBtn->resize(QSize(14, 14));
    undockBtn->setFlat(true);

    QHBoxLayout* buttonLay = new QHBoxLayout;
    buttonLay->setContentsMargins(0, 0, 0, 0);
    buttonLay->addWidget(undockBtn);
    buttonLay->addWidget(closeBtn);

    QWidget* layoutWidget = new QWidget;
    layoutWidget->setLayout(buttonLay);

    m_area->tabBar()->setTabButton(idx, QTabBar::RightSide, layoutWidget);
    connect(closeBtn, &QPushButton::clicked, wid, &QObject::deleteLater);
    connect(undockBtn, &QPushButton::clicked, wid,
            [this, wid = QPointer<QWidget>(wid)](){
        // get size
        auto current = m_area->currentWidget();
        assert(current);
        QSize size = current->size();
        // get screen pos
        auto pos = wid->mapToGlobal(wid->pos());
        // detach
        wid->setParent(nullptr);
        wid->move(pos);
        wid->resize(size);
        wid->show();
    });

    mdiItem->initialized();

    connect(wid, &QObject::destroyed,
            this, &GtMdiLauncher::onSubWindowClose);
    connect(mdiItem, &QObject::objectNameChanged,
            this, &GtMdiLauncher::changeTabTitle);
    connect(mdiItem, &QObject::destroyed,
            wid, &QObject::deleteLater);
    connect(gtApp, &GtApplication::themeChanged,
            mdiItem, &GtMdiItem::onThemeChanged);

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
