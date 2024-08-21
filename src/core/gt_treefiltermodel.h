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
#include <gt_object.h>

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

    /**
     * @brief Same as filterAcceptsRow, but also checks the predicate pred(obj)
     *        weather to accept this object or not in addition
     *
     * @param source_row
     * @param source_parent
     * @param pred Function(GtObject* obj). Returns true,
     *             if obj should be accepted
     * @return
     */
    template <typename Func>
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent,
                          Func&& pred) const
    {
        QModelIndex source_index =
            sourceModel()->index(source_row,
                                 this->filterKeyColumn(),
                                 source_parent);

        if (!source_index.isValid()) return false;

        GtObject* obj = static_cast<GtObject*>(source_index.internalPointer());
        assert(obj);

        QString key = sourceModel()->data(source_index,
                                          filterRole()).toString();

        // check if this object is accepted
        if (pred(obj) && key.contains(filterRegExp()))
        {
            return true;
        }

        // otherwise test childs
        for (int i = 0; i < sourceModel()->rowCount(source_index); ++i)
        {
            if (filterAcceptsRow(i, source_index, pred)) return true ;
        }

        return false;
    }

};

#endif // GTTREEFILTERMODEL_H
