/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_objectuuidmap.cpp
 *
 *  Created on: 21.09.2026
 *  Author: Matthias Schuff (SG-VTM)
 */

#include "gt_objectuuidmap.h"


QString GtObjectUUIDMap::newUuid(const QString &originalUuid) const
{
    return m_uuidMap.value(originalUuid);
}

bool GtObjectUUIDMap::containsOriginalUuid(const QString &originalUuid) const
{
    return m_uuidMap.contains(originalUuid);
}

bool GtObjectUUIDMap::isEmpty() const
{
    return m_uuidMap.isEmpty();
}

int GtObjectUUIDMap::size() const
{
    return m_uuidMap.size();
}

void GtObjectUUIDMap::clear()
{
    m_uuidMap.clear();
}

void GtObjectUUIDMap::insert(const QString &originalUuid, const QString &newUuid)
{
    m_uuidMap.insert(originalUuid, newUuid);
}

