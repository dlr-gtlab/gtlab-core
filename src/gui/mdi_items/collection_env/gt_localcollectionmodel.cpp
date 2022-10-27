/* GTlab - Gas Turbine laboratory
 * Source File: gt_localcollectionmodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QDir>

#include "gt_icons.h"
#include "gt_collectionitem.h"
#include "gt_logging.h"

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
        return 3;
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
        break;
    case Qt::ToolTipRole:
            return item.description();
        break;
    case Qt::DecorationRole:
        if (col == 0)
        {
            return gt::gui::icon::property16();
        }
        else if (col == 1)
        {
            return gt::gui::icon::infoBlue16();
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

bool GtLocalCollectionModel::uninstallItem(const QModelIndex& index)
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
