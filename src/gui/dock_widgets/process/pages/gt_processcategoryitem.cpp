/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcategoryitem.cpp
 *
 *  Created on: 12.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processcategoryitem.h"

GtProcessCategoryItem::GtProcessCategoryItem(const QString& id,
                                             QObject* parent) :
    GtAbstractProcessItem(parent),
    m_collapsed(false)
{
    setObjectName(id);
}

bool
GtProcessCategoryItem::addItem(GtAbstractProcessItem* newItem)
{
    if (!newItem)
    {
        return false;
    }

    if (findDirectChildren<GtAbstractProcessItem*>().contains(newItem))
    {
        return false;
    }

    appendChild(newItem);

    return true;
}

QList<GtAbstractProcessItem*>
GtProcessCategoryItem::items()
{
    return findDirectChildren<GtAbstractProcessItem*>();
}

bool
GtProcessCategoryItem::collapsed() const
{
    return m_collapsed;
}

void
GtProcessCategoryItem::setCollapsed(bool collapsed)
{
    m_collapsed = collapsed;
}
