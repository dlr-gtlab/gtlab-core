/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 01.09.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPLORERMODEL_H
#define GTEXPLORERMODEL_H

#include "gt_core_exports.h"

#include "gt_treefiltermodel.h"

class GtObject;

/**
 * @brief The GtExplorerModel class
 */
class GT_CORE_EXPORT GtExplorerModel : public GtTreeFilterModel
{
    Q_OBJECT

public:
    /**
     * @brief GtExplorerModel
     * @param parent
     */
    explicit GtExplorerModel(QObject* parent = nullptr);

    /**
     * @brief ~GtExplorerModel
     */
    virtual ~GtExplorerModel();

    /**
     * @brief objectFromIndex
     * @param index
     * @return
     */
    GtObject* objectFromIndex(const QModelIndex& index);

    /**
     * @brief getPersistentIndexList
     * @return
     */
    QModelIndexList getPersistentIndexList();
    
public slots:
    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

protected:
    /**
     * @brief supportedDragActions
     * @return
     */
    Qt::DropActions supportedDragActions() const;

    /**
     * @brief filterAcceptsRow
     * @param source_row
     * @param source_parent
     * @return
     */
    virtual bool filterAcceptsRow(int source_row,
                                  const QModelIndex& source_parent) const;

};

#endif // GTEXPLORERMODEL_H
