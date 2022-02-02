/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionmodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCONNECTIONMODEL_H
#define GTPROCESSCONNECTIONMODEL_H

#include <QAbstractItemModel>

#include "gt_processconnectionitem.h"

class GtTask;

/**
 * @brief The GtProcessConnectionModel class
 */
class GtProcessConnectionModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles
    {
        PropertyRole = Qt::UserRole + 1
    };

    enum Modes
    {
        WRITE_ONLY = 0,
        READ_WRITE
    };

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtProcessConnectionModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief Destructor.
     */
    virtual ~GtProcessConnectionModel();

    /**
     * @brief Setups data and resets entire model.
     * @param New root task.
     */
    void setRoot(GtTask* task);

    /**
     * @brief Sets model mode and resets entire model.
     * @param New model mode
     */
    void setMode(GtProcessConnectionModel::Modes mode);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const override;

    /**
     * @brief flags
     * @param index
     * @return
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent =QModelIndex()) const override;

    /**
     * @brief parent
     * @param index
     * @return
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * @brief Returns item for given model index.
     * @param Model index.
     * @return Item based on given index.
     */
    GtProcessConnectionItem* itemFromIndex(const QModelIndex& index) const;

    /**
     * @brief Returns index corresponding to given process connection item.
     * @param Process connection item.
     * @return Model index based on given item.
     */
    QModelIndex indexFromItem(GtProcessConnectionItem* item) const;

    /**
     * @brief Returns process connection item corresponding to given
     * identification strings.
     * @param UUID of component.
     * @param Property identification string.
     * @return Process connection item.
     */
    GtProcessConnectionItem* itemById(const QString& uuid,
                                      const QString& propId);

private:
    /// Root item
    GtProcessConnectionItem* m_root;

    /// Model mode
    GtProcessConnectionModel::Modes m_mode;

    /**
     * @brief Generates a data tree based on task structure.
     * @param Given task.
     */
    void createDataTree(GtTask* task);

    /**
     * @brief Generates a data tree based on process component structure
     * recursively.
     * @param Process component.
     * @param Process monitoring item.
     */
    void createDataTree(GtProcessComponent* component,
                        GtProcessConnectionItem* item);

    /**
     * @brief dataChangedToRoot
     * @param index
     */
    void dataChangedToRoot(const QModelIndex& index);

private slots:
    /**
     * @brief onObjectDataChanged
     * @param obj
     */
    void onObjectDataChanged(GtObject* obj);

};

#endif // GTPROCESSCONNECTIONMODEL_H
