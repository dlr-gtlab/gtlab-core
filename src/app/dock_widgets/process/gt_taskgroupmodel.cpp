/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 22.11.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processdata.h"

#include "gt_taskgroupmodel.h"

GtTaskGroupModel::GtTaskGroupModel(QObject* parent) :
    QAbstractListModel(parent)
{

}

void GtTaskGroupModel::init(const QStringList& userGroups,
                            const QStringList& customGroups)
{
    beginResetModel();

    m_userGroups = userGroups;
    m_customGroups = customGroups;

    endResetModel();
}

GtTaskGroup::SCOPE
GtTaskGroupModel::rowScope(int row) const
{
    if (row < 0 || row >= rowCount())
    {
        return GtTaskGroup::UNDEFINED;
    }

    if (row == 0)
    {
        return GtTaskGroup::UNDEFINED;
    }
    else if (row == m_userGroups.size() + 1)
    {
        return GtTaskGroup::UNDEFINED;
    }
    else if (row < m_userGroups.size() + 1)
    {
        return GtTaskGroup::USER;
    }

    return GtTaskGroup::CUSTOM;
}

int
GtTaskGroupModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_userGroups.size() + m_customGroups.size() + 2;
}

QVariant
GtTaskGroupModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.model() != this)
    {
        return QVariant();
    }

    const int row = index.row();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            return rowText(row);
        }
    }

    return QVariant();
}

Qt::ItemFlags
GtTaskGroupModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    const int row = index.row();

    if (!rowIsSelectable(row))
    {
        defaultFlags =
                defaultFlags & ~Qt::ItemIsSelectable & ~Qt::ItemIsEnabled;
    }

    return defaultFlags;
}

QString
GtTaskGroupModel::rowText(int row) const
{
    // row out of bounds
    if (row < 0 || row >= rowCount())
    {
        return {};
    }

    if (row == 0)
    {
        // row is user scope title
        return tr("User:");
    }
    else if (row == m_userGroups.size() + 1)
    {
        // row is custom scope title
        return tr("Custom:");
    }
    else if (row < m_userGroups.size() + 1)
    {
        // row is user scope entry
        return m_userGroups[row - 1];
    }

    // row is custom scope entry
    return m_customGroups[row - 2 - m_userGroups.size()];
}

bool
GtTaskGroupModel::rowIsSelectable(int row) const
{
    if (row < 0 || row >= rowCount())
    {
        return false;
    }

    if ((row == 0) || (row == m_userGroups.size() + 1))
    {
        return false;
    }

    return true;
}
