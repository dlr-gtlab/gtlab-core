/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#ifndef GT_OBJECTUUIDMAP_H
#define GT_OBJECTUUIDMAP_H

#include "gt_datamodel_exports.h"

#include <QString>
#include <QHash>


/**
 * @brief The GtObjectUUIDMap class
 * Contains a UUID mapping of all copied objects to the new objects.
 *
 * When copying an object, the UUID of the to-be-copied object (original)
 * is mapped onto the UUID of the newly created object, stored as key-value pair
 */
class GT_DATAMODEL_EXPORT GtObjectUUIDMap
{
public:
    /**
     * @brief Get the UUID of the copied object based on original UUID
     * @param originalUuid
     * @return UUID of the copied object
     */
    QString copiedUuid(const QString& originalUuid) const;

    /**
     * @brief Ask whether an UUID is in the list of original UUIDs
     * @param originalUuid
     * @return
     */
    bool containsOriginalUuid(const QString& originalUuid) const;

    /**
     * @brief Check whether there are entries in the list
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief Remove all entries
     */
    void clear();

    /**
     * @brief Add a key value pair for mapping from original to copied UUID
     * @param originalUuid
     * @param newUuid
     */
    void insert(const QString& originalUuid, const QString& newUuid);

    /**
     * @brief Get size of entries
     * @return
     */
    int size() const;

private:
    QHash<QString, QString> m_uuidMap;
};



#endif // GT_OBJECTUUIDMAP_H
