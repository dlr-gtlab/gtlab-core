/* GTlab - Gas Turbine laboratory
 * Source File: gt_processfiltermodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSFILTERMODEL_H
#define GTPROCESSFILTERMODEL_H

#include "gt_treefiltermodel.h"

/**
 * @brief The GtProcessFilterModel class
 */
class GtProcessFilterModel : public GtTreeFilterModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GtProcessFilterModel(QObject* parent = 0);

protected:
    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const;

};

#endif // GTPROCESSFILTERMODEL_H
