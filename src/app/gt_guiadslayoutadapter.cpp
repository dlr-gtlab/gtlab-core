/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guiadslayoutadapter.h"

#include <gt_object.h>
#include <gt_mdiwidget.h>
#include <gt_mdiitem.h>
#include <gt_palette.h>
#include <gt_application.h>

GtGuiAdsLayoutAdapter::GtGuiAdsLayoutAdapter(ads::CDockAreaWidget& centralWidget) :
    m_manager(centralWidget.dockManager())
{
    assert(m_manager);
}

bool
GtGuiAdsLayoutAdapter::isNull() const noexcept
{
    return !m_manager;
}

void
GtGuiAdsLayoutAdapter::focus(const QString& itemId)
{
    if (isNull() || itemId.isEmpty()) return;

    auto entry = std::find_if(m_openItems.keyValueBegin(),
                              m_openItems.keyValueEnd(),
                              [&itemId](auto entry){
                                  return entry.second &&
                                         entry.second->currentId() == itemId;
                              });
    if (entry == m_openItems.keyValueEnd() || !entry->first) return;

    auto dockWidget = gt::findParent<ads::CDockWidget*>(*entry->first);
    if (!dockWidget) return;

    dockWidget->toggleView();
    dockWidget->raise();
    dockWidget->focusWidget();
}

bool
GtGuiAdsLayoutAdapter::addItem(GtMdiItem& item)
{
    if (isNull()) return false;

    assert(m_manager->centralWidget() &&
           m_manager->centralWidget()->dockAreaWidget());

    auto widget = item.widget();
    if (!widget) return false;

    bool isDuplicate = itemFromWidget(*widget);
    if (isDuplicate) return false;

    auto* dock = new ads::CDockWidget(m_manager, item.objectName());
    dock->setIcon(item.icon());
    dock->setWidget(widget);
    dock->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);

    auto* area = m_manager->addDockWidgetTabToArea(dock, m_manager->centralWidget()->dockAreaWidget());
    m_openItems.insert(widget, &item);

    assert(area);

    // style buttons more appropriately
    for (auto* child : area->findChildren<QPushButton*>(QStringLiteral("tabCloseButton")))
    {
        child->setFlat(true);
        child->resize(child->height(), child->height());
    }

    // remove frames
    dock->setFrameShape(QFrame::NoFrame);
    for (auto* child : dock->findChildren<QFrame*>(QStringLiteral("dockWidgetScrollArea"), Qt::FindDirectChildrenOnly))
    {
        child->setFrameShape(QFrame::NoFrame);
    }

    connect(widget, &QObject::destroyed,
            this, [this, widget, d = QPointer<ads::CDockWidget>(dock)](){
            if (d) d->deleteLater();

            auto iter = m_openItems.find(widget);
            if (iter != m_openItems.end())
            {
                delete *iter;
                m_openItems.erase(iter);
            }
        }, Qt::DirectConnection);
    connect(&item, &QObject::objectNameChanged,
            dock, [dock, &item]() {
                dock->setObjectName(item.objectName());
            });
    connect(&item, &QObject::destroyed,
            dock, &QObject::deleteLater);
    connect(gtApp, &GtApplication::themeChanged,
            &item, &GtMdiItem::onThemeChanged);

    m_manager->setDockWidgetFocused(dock);

    widget->show();
    item.showEvent();

    return true;
}

GtMdiItem*
GtGuiAdsLayoutAdapter::item(const QString& itemId)
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
GtGuiAdsLayoutAdapter::itemFromWidget(QWidget& widget)
{
    auto entry = std::find_if(m_openItems.keyValueBegin(),
                              m_openItems.keyValueEnd(),
                              [&widget](auto entry){
                                  return entry.first == &widget;
                              });
    return (entry != m_openItems.keyValueEnd()) ? entry->second : nullptr;
}

GtMdiItem*
GtGuiAdsLayoutAdapter::currentItem()
{
    auto dockWidget = m_manager->focusedDockWidget();
    auto* widget  = dockWidget->findChild<GtMdiWidget*>();
    if (!widget) return nullptr;

    return itemFromWidget(*widget);
}

void
GtGuiAdsLayoutAdapter::closeAll()
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
