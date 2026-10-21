/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guitablayoutadapter.h"

#include <gt_tabwidget.h>
#include <gt_mdiitem.h>
#include <gt_mdiwidget.h>
#include <gt_algorithms.h>
#include <gt_icons.h>
#include <gt_application.h>

#include <QTabBar>
#include <QPushButton>
#include <QHBoxLayout>
#include <QApplication>

namespace
{

/// returns standardized window title
inline QString windowTitle(GtMdiItem& item)
{
    return QStringLiteral("GTlab - ") + item.objectName();
}

/// helper function to make tab buttons
QWidget* makeTabButtons(QPointer<GtTabWidget> tabWidget,
                        QPointer<GtMdiWidget> mdiWidget,
                        QPointer<GtMdiItem> mdiItem)
{
    // set custom close button
    auto* closeBtn = new QPushButton;
    closeBtn->setIconSize(QSize{14, 14});
    closeBtn->resize(QSize(14, 14));
    closeBtn->setIcon(gt::gui::icon::close());
    closeBtn->setFlat(true);
    // for identification in gui tests
    closeBtn->setObjectName(QStringLiteral("GuiItemTabCloseBtn"));

    auto* undockBtn = new QPushButton;
    undockBtn->setIconSize(QSize{14, 14});
    undockBtn->resize(QSize(14, 14));
    undockBtn->setIcon(gt::gui::icon::dock());
    undockBtn->setFlat(true);

    if (!gtApp || !gtApp->devMode())
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
                     mdiWidget, [tabWidget, mdiItem, mdiWidget](){
                         assert(mdiItem);
                         assert(mdiWidget);
                         // get widget size
                         QWidget* current = tabWidget->currentWidget();
                         assert(current);
                         QSize size = current->size();
                         // get screen pos
                         QPoint pos = mdiWidget->mapToGlobal(mdiWidget->pos());
                         // automatically undocks widget
                         mdiWidget->setParent(nullptr);
                         assert(!tabWidget->contains(mdiWidget));
                         mdiWidget->setAttribute(Qt::WA_DeleteOnClose, true);
                         mdiWidget->setWindowIcon(mdiWidget->windowIcon());
                         mdiWidget->setWindowTitle(windowTitle(*mdiItem));
                         mdiWidget->move(pos);
                         mdiWidget->resize(size);
                         mdiWidget->show();
                     });

    return layoutWidget;
};

/// helper function to setup redock action
void setupRedockAction(int idx,
                       QPointer<GtTabWidget> tabWidget,
                       QPointer<GtMdiWidget> mdiWidget,
                       QPointer<GtMdiItem> mdiItem)
{
    tabWidget->tabBar()->setTabButton(
        idx, QTabBar::RightSide, makeTabButtons(tabWidget,
                                                mdiWidget,
                                                mdiItem));

    QObject::connect(mdiWidget, &GtMdiWidget::redockWidget,
                     tabWidget, [tabWidget, mdiItem, mdiWidget](){
            // widget may be deleted during queuing of signal
            if (!mdiWidget) return;

            assert(!mdiWidget->parent());
            assert(tabWidget && !tabWidget->contains(mdiWidget));

            // add tab
            int idx = tabWidget->addTab(mdiWidget,
                                        mdiWidget->windowIcon(),
                                        mdiItem->objectName());

            assert(idx >= 0);
            tabWidget->tabBar()->setTabButton(
                idx, QTabBar::RightSide, makeTabButtons(tabWidget,
                                                        mdiWidget,
                                                        mdiItem));
            tabWidget->setCurrentIndex(idx);
        },
        Qt::QueuedConnection); // <- this prohibits crash in drawing procedure
}

} // namespace

GtGuiTabLayoutAdapter::GtGuiTabLayoutAdapter(GtTabWidget& tabWidget) :
    m_tab(&tabWidget)
{
    connect(m_tab, &QTabWidget::tabCloseRequested,
            this, &GtGuiTabLayoutAdapter::onTabCloseRequested);
}

bool
GtGuiTabLayoutAdapter::isNull() const noexcept
{
    return !m_tab;
}

void
GtGuiTabLayoutAdapter::focus(const QString& itemId)
{
    if (isNull() || itemId.isEmpty()) return;

    auto entry = std::find_if(m_openItems.keyValueBegin(),
                              m_openItems.keyValueEnd(),
                              [&itemId](auto entry){
                                  return entry.second &&
                                         entry.second->currentId() == itemId;
                              });
    if (entry == m_openItems.keyValueEnd()) return;

    int index = m_tab->indexOf(entry->first);
    if (index < 0) return;

    QWidget* widget = m_tab->widget(index);
    assert(widget);

    m_tab->setCurrentIndex(index);
    widget->setFocus();
}

bool
GtGuiTabLayoutAdapter::addItem(GtMdiItem& item)
{
    if (isNull()) return false;

    auto widget = item.widget();
    if (!widget) return false;

    bool isDuplicate = itemFromWidget(*widget);
    if (isDuplicate) return false;

    widget->setParent(m_tab);
    widget->setWindowIcon(item.icon());
    widget->setWindowTitle(windowTitle(item));

    int idx = m_tab->addTab(widget, item.icon(), item.objectName());
    assert(idx >= 0);

    if (GtMdiWidget* w = qobject_cast<GtMdiWidget*>(widget))
    {
        setupRedockAction(idx, m_tab, w, &item);
    }

    m_openItems.insert(widget, &item);

    connect(widget, &QObject::destroyed,
            this, [this, widget](){
                auto iter = m_openItems.find(widget);
                if (iter != m_openItems.end())
                {
                    delete *iter;
                    m_openItems.erase(iter);
                }
        }, Qt::DirectConnection);
    connect(&item, &QObject::objectNameChanged,
            this, [this, &item]() { onObjectNameChanged(&item); });
    connect(&item, &QObject::destroyed,
            widget, &QObject::deleteLater);
    connect(gtApp, &GtApplication::themeChanged,
            &item, &GtMdiItem::onThemeChanged);

    m_tab->setCurrentIndex(idx);

    widget->show();
    item.showEvent();

    return true;
}

GtMdiItem*
GtGuiTabLayoutAdapter::item(const QString& itemId)
{
    if (isNull() || itemId.isEmpty()) return nullptr;

    auto entry = std::find_if(m_openItems.keyValueBegin(),
                              m_openItems.keyValueEnd(),
                              [&itemId](auto entry){
                                  return entry.second &&
                                         entry.second->currentId() == itemId;
                              });
    if (entry == m_openItems.keyValueEnd()) return nullptr;

    return entry->second;
}

GtMdiItem*
GtGuiTabLayoutAdapter::itemFromWidget(QWidget& widget)
{
    auto entry = std::find_if(m_openItems.keyValueBegin(),
                              m_openItems.keyValueEnd(),
                              [&widget](auto entry){
                                  return entry.first == &widget;
                              });
    return (entry != m_openItems.keyValueEnd()) ? entry->second : nullptr;
}

GtMdiItem*
GtGuiTabLayoutAdapter::currentItem()
{
    if (isNull() || !m_tab->focusWidget()) return nullptr;
    return itemFromWidget(*m_tab->focusWidget());
}

void
GtGuiTabLayoutAdapter::closeAll()
{
    std::for_each(m_openItems.keyValueBegin(),
                  m_openItems.keyValueEnd(),
                  [](auto entry){
        // schedule widget deletion
        if (entry.first) entry.first->deleteLater();
        // delete item
        delete entry.second;
    });
    m_openItems.clear();
}

void
GtGuiTabLayoutAdapter::onTabCloseRequested(int idx)
{
    if (isNull()) return;

    if (QWidget* w = m_tab->widget(idx))
    {
        m_tab->removeTab(idx);
        delete w;

        // focus the new current widget to recieve input etc.
        if (QWidget* next = m_tab->currentWidget())
        {
            next->setFocus();
        }
    }
}

void
GtGuiTabLayoutAdapter::onObjectNameChanged(GtMdiItem* item)
{
    assert(!isNull());
    assert(item);

    item->widget()->setWindowTitle(windowTitle(*item));

    int index = m_tab->indexOf(item->widget());
    if (index < 0) return;
    m_tab->setTabText(index, item->objectName());
}
