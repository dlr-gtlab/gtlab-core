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
    explicit GtLocalCollectionModel(QObject* parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent) const override;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex &parent) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override;

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

    /**
     * @brief propIds
     * @return all properties of the collection items used in the model.
     */
    const QStringList& propIds() const;

public slots:
    /**
     * @brief sortByColumn
     * @param column
     * @param order
     */
    void sortByColumn(int column, Qt::SortOrder order);

private:
    /// Collection item list
    QList<GtCollectionItem> m_items;

    /// Shows info columns
    bool m_showInfoColumns;

    /// list of the identifiers of all propteris of the collection items.
    /// E.g. the map type in case of the map collection
    QStringList m_propIds;

    /**
     * @brief sort
     * Overwrite the basic model function how to sort
     * @param column - column the sort is based on
     * @param order - order (up/down) to use for the sort
     */
    void sort(int column, Qt::SortOrder order) override;
};



#endif // GT_LOCALCOLLECTIONMODEL_H
