/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_objectuuidmap.h
 *
 *  Created on: 21.09.2026
 *  Author: Matthias Schuff (SG-VTM)
 */

#ifndef GT_OBJECTUUIDMAP_H
#define GT_OBJECTUUIDMAP_H

#include "gt_datamodel_exports.h"

#include <QString>
#include <QHash>

class GT_DATAMODEL_EXPORT GtObjectUUIDMap
{
public:
    QString newUuid(const QString& originalUuid) const;

    bool containsOriginalUuid(const QString& originalUuid) const;

    bool isEmpty() const;

    void clear();

    void insert(const QString& originalUuid, const QString& newUuid);

    int size() const;

private:
    QHash<QString, QString> m_uuidMap;
};



#endif // GT_OBJECTUUIDMAP_H
