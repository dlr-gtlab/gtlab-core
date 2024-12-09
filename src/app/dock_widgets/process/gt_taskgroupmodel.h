/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 22.11.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKGROUPMODEL_H
#define GTTASKGROUPMODEL_H

#include <QAbstractListModel>

#include "gt_taskgroup.h"

/**
 * @brief The GtTaskGroupModel class represents a simple list model based on
 * information from the task groups. the individual scopes are separated and
 * the groups are assigned. The model can be used e.g. within a combobox.
 */
class GtTaskGroupModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    GtTaskGroupModel(QObject* parent = nullptr);

    /**
     * @brief Initializes model. Old structure will be reseted.
     * @param userGroups list of task groups in user scope
     * @param customGroups list of task groups in custom scope
     */
    void init(const QStringList& userGroups, const QStringList& customGroups);

    /**
     * @brief Returns scope of given row number.
     * @param row row number of task group
     * @return scope
     */
    GtTaskGroup::SCOPE rowScope(int row) const;

    /**
     * @brief rowCount Returns row count of model
     * @param parent
     * @return row count
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Model data function.
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief model flags functions
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief Searches for a Task Group with the given name in the specified
     * scope and returns its corresponding QModelIndex.
     * @param name The name of the Task Group to search for.
     * @param scope The scope in which the Task Group should be searched.
     * @return The QModelIndex of the Task Group with the specified name in the
     * given scope. If no matching Task Group is found, an invalid QModelIndex
     * is returned.
     */
    QModelIndex indexByGroupName(const QString& name,
                                 GtTaskGroup::SCOPE scope) const;

private:
    /// List of task group identification strings in user scope.
    QStringList m_userGroups;

    /// List of task group identification strings in custom scope.
    QStringList m_customGroups;

    /**
     * @brief Returns text for given row number.
     * @param row
     * @return row text
     */
    QString rowText(int row) const;

    /**
     * @brief Returns whether row for given row number is selectable. Usually
     * scope title rows are not selectable.
     * @param row
     * @return
     */
    bool rowIsSelectable(int row) const;

};

#endif // GTTASKGROUPMODEL_H
