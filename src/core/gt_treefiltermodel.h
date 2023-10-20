/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 22.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTREEFILTERMODEL_H
#define GTTREEFILTERMODEL_H

#include "gt_core_exports.h"

#include <QSortFilterProxyModel>

/**
 * @brief The GtTreeFilterModel class
 */
class GT_CORE_EXPORT GtTreeFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit GtTreeFilterModel(QObject* parent = nullptr);

    void setRootIndex(const QModelIndex& index);

public slots:
    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

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

#endif // GTTREEFILTERMODEL_H
