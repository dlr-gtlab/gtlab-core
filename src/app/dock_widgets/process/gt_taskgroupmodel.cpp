/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
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
    if (row < 0 || row >= rowCount())
    {
        return {};
    }

    if (row == 0)
    {
        return "user:";
    }
    else if (row == m_userGroups.size() + 1)
    {
        return "custom:";
    }
    else if (row < m_userGroups.size() + 1)
    {
        return m_userGroups[row - 1];
    }

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
