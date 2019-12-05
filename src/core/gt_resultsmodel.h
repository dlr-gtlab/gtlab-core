/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 09.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTRESULTSMODEL_H
#define GTRESULTSMODEL_H

#include <QPointer>

#include "gt_core_exports.h"
#include "gt_treefiltermodel.h"

class GtExplorerModel;

/**
 * @brief The GtResultsModel class
 */
class GT_CORE_EXPORT GtResultsModel : public GtTreeFilterModel
{
    Q_OBJECT

public:
    /**
     * @brief GtResultsModel
     */
    GtResultsModel(GtExplorerModel* parent = 0);

    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const;

    /**
     * @brief getPersistentIndexList
     * @return
     */
    QModelIndexList getPersistentIndexList();

private:
    /// Explorer model
    QPointer<GtExplorerModel> m_explorerModel;

};

#endif // GTRESULTSMODEL_H
