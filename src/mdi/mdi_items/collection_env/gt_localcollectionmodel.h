/* GTlab - Gas Turbine laboratory
 * Source File: gt_localcollectionmodel.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_LOCALCOLLECTIONMODEL_H
#define GT_LOCALCOLLECTIONMODEL_H

#include <QAbstractListModel>

#include "gt_collectionitem.h"

/**
 * @brief The GtLocalCollectionModel class
 */
class GtLocalCollectionModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief GtLocalCollectionModel
     * @param parent
     */
    GtLocalCollectionModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;

    /**
     * @brief setItems
     * @param items
     */
    void setItems(const QList<GtCollectionItem>& items);

    /**
     * @brief itemFromIndex
     * @param index
     * @return
     */
    GtCollectionItem itemFromIndex(const QModelIndex& index);

    /**
     * @brief uninstallItem
     * @param index
     * @return
     */
    bool uninstallItem(const QModelIndex& index);

    /**
     * @brief setShowInfoColumns
     * @param val
     */
    void setShowInfoColumns(bool val);

private:
    /// Collection item list
    QList<GtCollectionItem> m_items;

    /// Shows info columns
    bool m_showInfoColumns;

};

#endif // GT_LOCALCOLLECTIONMODEL_H