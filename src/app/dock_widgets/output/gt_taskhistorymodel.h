/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskhistorymodel.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 08.02.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_TASKHISTORYMODEL_H
#define GT_TASKHISTORYMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QDateTime>

#include "gt_processcomponent.h"

/**
 * @brief The GtTaskHistoryModel class
 */
class GtTaskHistoryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtTaskHistoryModel(QObject* parent);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
        const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int columnCount(
        const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role) const override;

    /**
     * @brief Clears history data model.
     */
    void clear();

    /**
     * @brief Sets new path to task history and repopulates model.
     * @param path New task history path.
     */
    void setPath(const QString& path);

    /**
     * @brief hasChildren
     * @param parent
     * @return
     */
    bool hasChildren(const QModelIndex& parent) const override;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent =
                          QModelIndex()) const override;

    /**
     * @brief parent
     * @param index
     * @return
     */
    QModelIndex parent(const QModelIndex& index) const override;

private:
    /// Entry structure
    struct Entry
    {
        /// Task identification string
        QString m_taskId;

        /// Task start time tag
        QDateTime m_startTime;

        /// Task end time tag
        QDateTime m_endTime;

        /// Task state
        GtProcessComponent::STATE m_state;

    };

    /// History path
    QString m_path;

    /// Entry list.
    QVector<Entry> m_entries;

    /// Date format.
    const QString m_dateFormat;

    /**
     * @brief Populates task history list.
     */
    void populate();

    /**
     * @brief Converts meta data to entry.
     * @param path Path to meta file.
     * @param entry Reference to entry.
     * @return Returns true if meta data was successfully converted to entry.
     */
    bool metaToEntry(const QString& path, Entry& entry);

    /**
     * @brief Validates meta data.
     * @param json Json object of meta data.
     * @return Returns true if meta data is valid. Otherwise false is returned.
     */
    bool metaIsValid(const QJsonObject& json);

    /**
     * @brief Converts task state to state specific string.
     * @param state Task state.
     * @return State specific string.
     */
    QString taskStateToString(GtProcessComponent::STATE state) const;

    /**
     * @brief COnverts state specific string to task state.
     * @param str State specific string.
     * @return Task state.
     */
    GtProcessComponent::STATE stringToTaskState(const QString& str) const;

signals:
    /**
     * @brief Emitted after model refresh.
     */
    void modelRefreshed();

};

#endif // GT_TASKHISTORYMODEL_H
