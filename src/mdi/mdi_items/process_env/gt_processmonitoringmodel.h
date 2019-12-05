/* GTlab - Gas Turbine laboratory
 * Source File: gt_processmonitoringmodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 14.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMONITORINGMODEL_H
#define GTPROCESSMONITORINGMODEL_H

#include <QAbstractItemModel>

#include "gt_processmonitoringitem.h"

class GtTask;

/**
 * @brief The GtProcessMonitoringModel class
 */
class GtProcessMonitoringModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles
    {
        PropertyRole = Qt::UserRole + 1
    };

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    GtProcessMonitoringModel(QObject* parent = Q_NULLPTR);

    /**
     * @brief Destructor.
     */
    virtual ~GtProcessMonitoringModel();

    /**
     * @brief Setups monitoring data and resets entire model.
     * @param Task for monitoring.
     */
    void setMonitoringData(GtTask* task);

    /**
     * @brief Returns list of selected monitoring items.
     * @return  List of selected monitoring items.
     */
    QList<GtProcessMonitoringItem*> selectedItems();

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    virtual int columnCount(
            const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index,
                          int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(const QModelIndex& index,
                         const QVariant& value,
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent =QModelIndex()) const Q_DECL_OVERRIDE;

    /**
     * @brief parent
     * @param index
     * @return
     */
    QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    /**
     * @brief flags
     * @param index
     * @return
     */
    virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;

public slots:
    void clearModel();

private:
    /// Root item
    GtProcessMonitoringItem* m_root;

    /**
     * @brief Returns item for given model index.
     * @param Model index.
     * @return Item based on given index.
     */
    GtProcessMonitoringItem* itemFromIndex(const QModelIndex& index) const;

    /**
     * @brief Returns index corresponding to given process monitoring item.
     * @param Process monitoring item.
     * @return Model index based on given item.
     */
    QModelIndex indexFromItem(GtProcessMonitoringItem* item) const;

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
                        GtProcessMonitoringItem* item);

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

signals:
    /**
     * @brief Emitted after selection has been changed.
     */
    void selectionChanged();

};

#endif // GTPROCESSMONITORINGMODEL_H
