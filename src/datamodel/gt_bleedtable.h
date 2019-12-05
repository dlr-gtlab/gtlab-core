/* GTlab - Gas Turbine laboratory
 * Source File: gt_bleedtable.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 26.09.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601-2191
 */

#ifndef GTBLEEDTABLE_H
#define GTBLEEDTABLE_H

#include "gt_datamodel_exports.h"

#include "gt_table.h"

/**
 * @brief The GtBleedTable class - specific type of GtTable
 *  no more features but necessary to select specific tables for bleed
 * information for maps with bleed information
 */
class GT_DATAMODEL_EXPORT GtBleedTable : public GtTable
{
    Q_OBJECT
public:
    /**
     * @brief GtBleedTable - constructor
     */
    Q_INVOKABLE GtBleedTable();
};

#endif // GTBLEEDTABLE_H
