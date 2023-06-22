/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processfiltermodel.h
 *
 *  Created on: 19.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSFILTERMODEL_H
#define GTPROCESSFILTERMODEL_H

#include "gt_gui_exports.h"
#include "gt_treefiltermodel.h"

/**
 * @brief The GtProcessFilterModel class
 */
class GT_GUI_EXPORT GtProcessFilterModel : public GtTreeFilterModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit GtProcessFilterModel(QObject* parent = nullptr);

protected:
    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const override;

};

#endif // GTPROCESSFILTERMODEL_H
