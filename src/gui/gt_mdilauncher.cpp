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

    if (!gtApp->devMode())
    {
        undockBtn->hide();
    }

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

struct GtMdiLauncher::Impl
{
    /// Mdi area widget
    QPointer<QTabWidget> area{nullptr};

    /// Registered dock widget meta objects
    QHash<QString, QMetaObject> dockWidgets;

    /// List of all open mdi items
    QMap<const QObject*, QPointer<GtMdiItem>> openItems;

    /// MDI Editor name map (class, name)
    QMap<QString, QString> mdiNames;

    /// Collections
    QHash<QString, GtCollectionInterface*> collections;
};

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    pimpl(std::make_unique<Impl>())
{
}

GtMdiLauncher::~GtMdiLauncher() = default;
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

    const auto& openItems = pimpl->openItems.values();

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

    if (!pimpl->area)
    {
        return;
    }

    gt::for_each_key(pimpl->openItems, [&](const QObject* e)
    {
        if (generateMdiItemId(pimpl->openItems.value(e)) == mdiId)
        {
            // TODO: replace with `GtTabWidget->widgets()`

            QList<QWidget*> list;
            for (int i = 0; i < pimpl->area->count(); ++i)
            {
                if (pimpl->area->widget(i))
                {
                    list.append(pimpl->area->widget(i));
                }
            }

            for (QWidget* listItem : list)
            {
                if (listItem == e)
                {
                    pimpl->area->setCurrentWidget(listItem);
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
    for (GtMdiItem* item : pimpl->openItems)
    {
        assert(item);
        // cppcheck-suppress assertWithSideEffect
        assert(item->widget());
        item->widget()->deleteLater();
    }
    pimpl->openItems.clear();
}

void
GtMdiLauncher::onSubWindowClose(QObject* obj)
{
    auto iter = pimpl->openItems.find(obj);
    if (iter != pimpl->openItems.end())
    {
        pimpl->openItems.erase(iter);
    }
}

void
GtMdiLauncher::onCloseTabRequest(int i)
{
    if (QWidget* w = pimpl->area->widget(i))
    {
        pimpl->area->removeTab(i);
        delete w;

        // focus the new current widget to recieve input etc.
        if (QWidget* next = pimpl->area->currentWidget())
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
    if (pimpl->area)
    {
        return;
    }

    pimpl->area = area;

    connect(pimpl->area, SIGNAL(tabCloseRequested(int)), this,
            SLOT(onCloseTabRequest(int)));
}

bool
GtMdiLauncher::registerMdiWidgets(const QList<QMetaObject> &mdiItems)
{
    bool allInvokable = true;

    for (const auto& metaObj : mdiItems)
    {
        std::unique_ptr<GtObject> obj(newObject(metaObj));
        if (!obj)
        {
            if (!m_silent) qWarning() << "class " << metaObj.className() << " not invokable!";
            allInvokable = false;
        }
        else
        {
            // Register openWith menu entry name
            if (!obj->objectName().isEmpty())
            {
                pimpl->mdiNames[metaObj.className()] = obj->objectName();
            }
            else
            {
                gtWarning() << tr("The MDI class '%1' should define an "
                                  "objectName in its constructor")
                                   .arg(metaObj.className());
            }
        }

    }

    return allInvokable;
}

QString GtMdiLauncher::mdiOpenWithName(const QString &className) const
{
    auto iter = pimpl->mdiNames.find(className);
    if (iter != pimpl->mdiNames.end() && !iter.value().isEmpty()) return iter.value();

    // Fallback, no name registered.
    return className;
}

QStringList
GtMdiLauncher::dockWidgetIds()
{
    return pimpl->dockWidgets.keys();
}

QMetaObject
GtMdiLauncher::dockWidget(const QString& id)
{
    return pimpl->dockWidgets.value(id);
}

QStringList
GtMdiLauncher::collectionIds()
{
    return pimpl->collections.keys();
}

QIcon
GtMdiLauncher::collectionIcon(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return {};
    }

    GtCollectionInterface* coll = pimpl->collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return {};
    }

    GtCollectionInterface* coll = pimpl->collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionStructure();
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return nullptr;
    }

    return pimpl->collections.value(id);
}

void
GtMdiLauncher::printCurrentWindow()
{
    if (!pimpl->area)
    {
        return;
    }

    print(pimpl->area->currentWidget());
}

void
GtMdiLauncher::print(QWidget* subWindow)
{
    if (!subWindow) return;

    if (auto item = pimpl->openItems.value(subWindow, nullptr))
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

    if (pimpl->openItems.contains(subWindow) && pimpl->openItems.value(subWindow))
    {
        return pimpl->openItems.value(subWindow)->isPrintable();
    }

    return false;
}

std::vector<GtQmlAction*>
GtMdiLauncher::toolbarActions(QWidget* subWindow) const
{
    if (!subWindow)
    {
        return {};
    }

    if (pimpl->openItems.contains(subWindow) && pimpl->openItems.value(subWindow))
    {
        return pimpl->openItems.value(subWindow)->toolbarActions();
    }

    return {};
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj)
{
    if (pimpl->dockWidgets.contains(metaObj.className()))
    {
        gtWarning().medium()
                << tr("Dockwidget '%1' was already registered")
                   .arg(metaObj.className());
        return false;
    }

    pimpl->dockWidgets.insert(metaObj.className(), metaObj);

    return true;
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    for (const QMetaObject& mobj : metaData)
    {
        QString classname = mobj.className();
        if (pimpl->dockWidgets.contains(classname))
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
        pimpl->dockWidgets.insert(metaObj.className(), metaObj);
    }

    return true;
}

bool
GtMdiLauncher::registerCollection(const QString& str,
                                  GtCollectionInterface* coll)
{
    if (pimpl->collections.contains(str))
    {
        return false;
    }

    pimpl->collections.insert(str, coll);

    return true;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, const QString& customId)
{
    if (!pimpl->area) return nullptr;

    auto* tabWidget = qobject_cast<GtTabWidget*>(pimpl->area.data());
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
            const auto openItems = pimpl->openItems.values();

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

    mdiItem->initialized();
    pimpl->openItems.insert(mdiWidget, mdiItem);

    int idx = tabWidget->addTab(mdiWidget, icon, mdiItem->objectName());
    assert(idx >= 0);
    tabWidget->tabBar()->setTabButton(idx, QTabBar::RightSide,
                                      makeTabButtons(tabWidget, icon, mdiWidget, mdiItem));


    connect(mdiWidget, &QObject::destroyed,
            this, &GtMdiLauncher::onSubWindowClose);
    connect(mdiItem, &QObject::objectNameChanged,
            this, &GtMdiLauncher::changeTabTitle);
    connect(mdiItem, &QObject::destroyed,
            mdiWidget, &QObject::deleteLater);
    connect(gtApp, &GtApplication::themeChanged,
            mdiItem, &GtMdiItem::onThemeChanged);


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
    assert(pimpl->area);
    int index = -1;

    /// if this function was called by a connection
    if (sender())
    {
        if (GtMdiItem* item = qobject_cast<GtMdiItem*>(sender()))
        {
            index = pimpl->area->indexOf(item->widget());
        }
    }
    /// else use the current mdi item
    else
    {
        index = pimpl->area->currentIndex();
    }

    /// if index can be found:
    if (index > -1)
    {
        if (auto* w = pimpl->area->widget(index))
        {
            // undocked
            if (!w->parent()) w->setWindowTitle(S_WINDOW_PREFIX + newTitle);
        }
        pimpl->area->setTabText(index, newTitle);
    }
}
