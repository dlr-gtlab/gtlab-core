/* GTlab - Gas Turbine laboratory
 * Source File: gt_tablegroup.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 13.04.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#ifndef GTTABLEGROUP_H
#define GTTABLEGROUP_H

#include "gt_datamodel_exports.h"
#include "gt_objectgroup.h"

#include "gt_table.h"

/**
 * @brief The GtTableGroup class to order several tables as a group
 */
class GT_DATAMODEL_EXPORT GtTableGroup : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtTableGroup - constructor
     * @param parent
     */
    Q_INVOKABLE explicit GtTableGroup(GtObject* parent = nullptr);

    /**
     * @brief tables - returns a list of all GtTable children
     * @return
     */
    QList<GtTable*> tables();

    /**
     * @brief table - returns a GtTable child with the specififc name
     * @param name
     * @return
     */
    GtTable* table(const QString& name);

    /**
     * @brief tableNames - returns the names of all children GtTables
     * @return
     */
    QStringList tableNames();

};

#endif // GTTABLEGROUP_H
