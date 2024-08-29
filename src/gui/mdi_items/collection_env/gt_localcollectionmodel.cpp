/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_localcollectionmodel.cpp
 *
 *  Created on: 08.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QDir>

#include "gt_icons.h"
#include "gt_collectionitem.h"

#include "gt_localcollectionmodel.h"

GtLocalCollectionModel::GtLocalCollectionModel(QObject* parent) :
    QAbstractListModel(parent), m_showInfoColumns(true)
{

}

int
GtLocalCollectionModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_items.size();
}

int
GtLocalCollectionModel::columnCount(const QModelIndex& /*parent*/) const
{
    if (m_showInfoColumns)
    {
        return 3 + m_propIds.size();
    }
    else
    {
        return 1;
    }
}

QVariant
GtLocalCollectionModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const int row = index.row();
    const int col = index.column();

    if (row < 0 || row >= m_items.size())
    {
        return QVariant();
    }

    GtCollectionItem item = m_items[row];

    switch (role) {
    case Qt::DisplayRole:
        if (col == 0)
        {
            return item.ident();
        }
        else if (col == 2)
        {
            return QString::number(item.version());
        }
        else if (col > 2 && !m_propIds.isEmpty() && col - 3 < m_propIds.size())
        {
            return item.property(m_propIds.at(col - 3));
        }
        break;
    case Qt::ToolTipRole:
            return item.description();
        break;
    case Qt::DecorationRole:
        if (col == 0)
        {
            return gt::gui::icon::property();
        }
        else if (col == 1)
        {
            return gt::gui::icon::info2();
        }
        break;
    default:
        break;
    }

    return QVariant();
}

QVariant
GtLocalCollectionModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QAbstractListModel::headerData(section, orientation, role);
    }

    QStringList ids = propIds();


    switch (role) {
    case Qt::DisplayRole:
        if (section == 0)
        {
            return tr("Name");
        }
        else if (section == 1)
        {
            return QString();
        }
        else if (section == 2)
        {
            return tr("Version");
        }
        else if (section > 2 && !ids.isEmpty() && section - 3 < ids.size())
        {
            return ids.at(section - 3);
        }

        break;
    default:
        break;
    }

    return QAbstractListModel::headerData(section, orientation, role);
}

void
GtLocalCollectionModel::setItems(const QList<GtCollectionItem>& items)
{
    beginResetModel();
    m_items = items;
    endResetModel();

    m_propIds.clear();
    if (!m_items.isEmpty())
    {
        GtCollectionItem const& item = m_items[0];
        if (item.isValid())
        {
            m_propIds = item.propertyIds();
        }
    }
}

GtCollectionItem
GtLocalCollectionModel::itemFromIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return GtCollectionItem();
    }

    if (index.model() != this)
    {
        return GtCollectionItem();
    }

    const int row = index.row();

    if (row < 0 || row >= m_items.size())
    {
        return GtCollectionItem();
    }

    return m_items[row];
}

bool
GtLocalCollectionModel::uninstallItem(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return false;
    }

    if (index.model() != this)
    {
        return false;
    }

    const int row = index.row();

    if (row < 0 || row >= m_items.size())
    {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row);

    GtCollectionItem item = m_items.takeAt(row);

    QDir dir(item.localPath());

    if (dir.exists())
    {
        dir.removeRecursively();
    }

    endRemoveRows();

    return true;
}

void
GtLocalCollectionModel::setShowInfoColumns(bool val)
{
    if (m_showInfoColumns == val)
    {
        return;
    }

    beginResetModel();
    m_showInfoColumns = val;
    endResetModel();
}

void
GtLocalCollectionModel::sortByColumn(int column, Qt::SortOrder order)
{
    sort(column, order);
    emit dataChanged(QModelIndex(), QModelIndex());
}

QStringList const&
GtLocalCollectionModel::propIds() const
{
    return m_propIds;
}

void
GtLocalCollectionModel::sort(int column, Qt::SortOrder order)
{
    using T = GtCollectionItem;
    std::function<bool(T const&, T const&)> function;

    bool ascending = order == Qt::AscendingOrder;

    switch (column)
    {
    case 0:
        function = [=](const T& x, const T& y){
            return ascending == (x.ident() < y.ident());
        };
        break;
    case 1:
        // dont sort this
        break;
    case 2:
        function = [=](const T& x, const T& y){
            return ascending == (x.version() < y.version());
        };
        break;
    default:
        if (m_propIds.empty()) return;

        if (column > 2 && column - 3 < m_propIds.size())
        {
            QString const& prop = m_propIds.at(column - 3);
            function = [ascending, &prop](const T& x, const T& y){
                // The "<" operator is deprecated, but the replacement "compare" fails under linux
                return ascending == (x.property(prop) < y.property(prop));
            };
        }
        break;
    }

    if (function)
    {
        beginResetModel();
        std::sort(std::begin(m_items), std::end(m_items), function);
        endResetModel();
    }

}
