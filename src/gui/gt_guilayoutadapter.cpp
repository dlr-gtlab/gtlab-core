/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guilayoutadapter.h"
#include "gt_mdiitem.h"

GtGuiLayoutAdapter::GtGuiLayoutAdapter() = default;

bool
GtGuiLayoutAdapter::hasItem(const QString& itemId)
{
    return item(itemId);
}

void
GtGuiLayoutAdapter::setContainingWidget(GtMdiItem& item,
                                        QWidget* containingWidget)
{
    item.setContainingWidget(containingWidget);
}

void
GtGuiLayoutAdapter::triggerInitialization(GtMdiItem& item)
{
    item.initialized();
}

void
GtGuiLayoutAdapter::triggerShowEvent(GtMdiItem& item)
{
    item.showEvent();
}
