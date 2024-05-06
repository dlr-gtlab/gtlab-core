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

#include "gt_mdiwidget.h"
#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_algorithms.h"
#include "gt_icons.h"
#include "gt_logging.h"
#include "gt_tabwidget.h"

static const QString S_WINDOW_PREFIX = QStringLiteral("GTlab - ");

namespace
{

/// helper function to make tab buttons
QWidget* makeTabButtons(QPointer<GtTabWidget> tabWidget,
                        QIcon const& tabIcon,
                        QPointer<GtMdiWidget> mdiWidget,
                        QPointer<GtMdiItem> mdiItem)
{
    // set custom close button
    auto* closeBtn = new QPushButton;
    closeBtn->setIconSize(QSize{14, 14}); // because stani wants it this way...
    closeBtn->resize(QSize(14, 14));
    closeBtn->setIcon(gt::gui::icon::close());
    closeBtn->setFlat(true);
    // for identification in gui tests
    closeBtn->setObjectName(QStringLiteral("MdiTabCloseBtn"));

    auto* undockBtn = new QPushButton;
    undockBtn->setIconSize(QSize{14, 14});
    undockBtn->resize(QSize(14, 14));
    undockBtn->setIcon(gt::gui::icon::dock());
    undockBtn->setFlat(true);

    auto* buttonLay = new QHBoxLayout;
    buttonLay->setContentsMargins(0, 0, 0, 0);
    buttonLay->addWidget(undockBtn);
    buttonLay->addWidget(closeBtn);

    auto* layoutWidget = new QWidget;
    layoutWidget->setLayout(buttonLay);

    QObject::connect(closeBtn, &QPushButton::clicked,
                     mdiWidget, &QObject::deleteLater);

    QObject::connect(undockBtn, &QPushButton::clicked,
                     mdiWidget, [tabWidget, tabIcon, mdiItem, mdiWidget](){
        assert(mdiItem);
        assert(mdiWidget);
        // get widget size
        QWidget* current = tabWidget->currentWidget();
        assert(current);
        QSize size = current->size();
        // get screen pos
        QPoint pos = mdiWidget->mapToGlobal(mdiWidget->pos());
        // detach
        mdiWidget->setParent(nullptr); // automatically undocks widget
        assert(!tabWidget->contains(mdiWidget));
        mdiWidget->setAttribute(Qt::WA_DeleteOnClose, true);
        mdiWidget->setWindowIcon(tabIcon);
        mdiWidget->setWindowTitle(S_WINDOW_PREFIX + mdiItem->objectName());
        mdiWidget->move(pos);
        mdiWidget->resize(size);
        mdiWidget->show();
    });

    return layoutWidget;
};

/// helper function to setup redock action
void setupRedockAction(QPointer<GtTabWidget> tabWidget,
                       QIcon const& tabIcon,
                       QPointer<GtMdiWidget> mdiWidget,
                       QPointer<GtMdiItem> mdiItem)
{
    QObject::connect(mdiWidget, &GtMdiWidget::redockWidget,
                     tabWidget, [tabWidget, tabIcon, mdiItem, mdiWidget](){
        // widget may be deleted during queuing of signal
        if (!mdiWidget) return;

        assert(!mdiWidget->parent());
        assert(tabWidget && !tabWidget->contains(mdiWidget));

        // add tab
        auto const& objectName = mdiWidget->windowTitle().mid(S_WINDOW_PREFIX.size());
        int idx = tabWidget->addTab(mdiWidget, tabIcon, objectName);
        assert(idx >= 0);
        tabWidget->tabBar()->setTabButton(idx, QTabBar::RightSide,
                                          makeTabButtons(tabWidget, tabIcon, mdiWidget, mdiItem));
        tabWidget->setCurrentWidget(mdiWidget);
    }, Qt::QueuedConnection); // <- this prohibits crash in drawing procedure
}

} // namespace

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

    const auto& openItems = m_openItems.values();

    for (GtMdiItem* openItem : openItems)
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

    gt::for_each_key(m_openItems, [&](const QObject* e)
    {
        if (generateMdiItemId(m_openItems.value(e)) == mdiId)
        {
            // TODO: replace with `GtTabWidget->widgets()`

            QList<QWidget*> list;
            for (int i = 0; i < m_area->count(); ++i)
            {
                if (m_area->widget(i))
                {
                    list.append(m_area->widget(i));
                }
            }

            for (QWidget* listItem : list)
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
        // cppcheck-suppress assertWithSideEffect
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
        m_openItems.erase(iter);
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

    if (auto item = m_openItems.value(subWindow, nullptr))
    {
        item->print();
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
    if (!m_area) return nullptr;

    auto* tabWidget = qobject_cast<GtTabWidget*>(m_area.data());
    assert(tabWidget);

    if (!knownClass(id))
    {
        gtWarning().medium() << tr("Unknown Mdi item '%1'!").arg(id);
        return nullptr;
    }

    GtObject* obj = newObject(id);

    auto* mdiItem = qobject_cast<GtMdiItem*>(obj);

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

    QPointer<GtMdiWidget> mdiWidget{qobject_cast<GtMdiWidget*>(mdiItem->widget())};
    assert(mdiWidget);

    // move to widget
    mdiItem->setParent(mdiWidget);

    QIcon icon = mdiItem->icon();

    if (icon.isNull())
    {
        icon = gt::gui::icon::layers();
    }

    setupRedockAction(tabWidget, icon, mdiWidget, mdiItem);

    int idx = tabWidget->addTab(mdiWidget, icon, mdiItem->objectName());
    assert(idx >= 0);
    tabWidget->tabBar()->setTabButton(idx, QTabBar::RightSide,
                                      makeTabButtons(tabWidget, icon, mdiWidget, mdiItem));

    mdiItem->initialized();

    connect(mdiWidget, &QObject::destroyed,
            this, &GtMdiLauncher::onSubWindowClose);
    connect(mdiItem, &QObject::objectNameChanged,
            this, &GtMdiLauncher::changeTabTitle);
    connect(mdiItem, &QObject::destroyed,
            mdiWidget, &QObject::deleteLater);
    connect(gtApp, &GtApplication::themeChanged,
            mdiItem, &GtMdiItem::onThemeChanged);

    m_openItems.insert(mdiWidget, mdiItem);

    mdiItem->setSubWin(mdiWidget);

    tabWidget->setCurrentWidget(mdiWidget);
    mdiWidget->show();
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
        if (auto* w = m_area->widget(index))
        {
            // undocked
            if (!w->parent()) w->setWindowTitle(S_WINDOW_PREFIX + newTitle);
        }
        m_area->setTabText(index, newTitle);
    }
}
