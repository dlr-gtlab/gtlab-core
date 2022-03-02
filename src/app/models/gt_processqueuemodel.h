/* GTlab - Gas Turbine laboratory
 * Source File: gt_processqueuemodel.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 11.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTPROCESSQUEUEMODEL_H
#define GTPROCESSQUEUEMODEL_H

#include <QAbstractItemModel>

class GtTask;
class GtProcessExecutor;

/**
 * @brief The GtProcessQueueModel class
 */
class GtProcessQueueModel  : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessQueueModel
     * @param proExec
     * @param view
     * @param parent
     */
    GtProcessQueueModel(GtProcessExecutor* proExec,
                        QObject* parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(
            const QModelIndex& index, int role) const override;

    /**
     * @brief index
     * @param row
     * @param column
     * @param parent
     * @return
     */
    QModelIndex index(int row, int column,
            const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief parent
     * @param child
     * @return
     */
    QModelIndex parent(const QModelIndex &child) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    /**
     * @brief setProcessExecutor
     * @param procExec
     */
    void setProcessExecutor(GtProcessExecutor* procExec);

private:
    GtProcessExecutor* m_proExec;

    QList<GtTask*> m_tasks;

    void updateTaskList();

private slots:
    void onQueueChanged();

    void onStateChanged();

signals:
    void queueChanged();

};

#endif // GTPROCESSQUEUEMODEL_H
