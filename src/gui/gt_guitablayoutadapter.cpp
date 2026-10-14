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

GtGuiTabLayoutAdapter::GtGuiTabLayoutAdapter(GtTabWidget& tabWidget)
{

}

void
GtGuiTabLayoutAdapter::focus(const QString& itemId)
{

}

void
GtGuiTabLayoutAdapter::addItem(GtMdiItem* item)
{

}

bool
GtGuiTabLayoutAdapter::hasItem(const QString& itemId) const
{
    return false;
}

GtMdiItem*
GtGuiTabLayoutAdapter::currentItem()
{
    return nullptr;
}
