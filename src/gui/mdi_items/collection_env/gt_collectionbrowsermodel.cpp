/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionbrowsermodel.cpp
 *
 *  Created on: 08.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_icons.h"
#include "gt_colors.h"

#include "gt_collectionbrowsermodel.h"

GtCollectionBrowserModel::GtCollectionBrowserModel(QObject* parent) :
    QAbstractItemModel(parent)
{

}

int
GtCollectionBrowserModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        return groupTypes().size();
    }

    const auto itemType = static_cast<CollectionItemType>
                                        (parent.internalId());

    switch (itemType)
    {
        case UpdateAvailableItemGroup:
            return m_updateAvailableItems.size();

        case AvailableItemGroup:
            return m_availableItems.size();

        case InstalledItemGroup:
            return m_installedItems.size();
        case InstalledItem:
        case AvailableItem:
        case UpdateAvailableItem:
            ;// do nothing
    }

    return 0;
}

int
GtCollectionBrowserModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 4;
}

QVariant
GtCollectionBrowserModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const int col = index.column();

    const auto itemType = static_cast<CollectionItemType>
                                        (index.internalId());

    switch (itemType)
    {
        case UpdateAvailableItemGroup:
        {
            if (role == Qt::DisplayRole && col == 0)
            {
                return tr("Update available!");
            }

            if (role == Qt::DecorationRole && col == 0)
            {
                return gt::gui::icon::update();
            }

            if (role == Qt::BackgroundRole)
            {
                return gt::gui::color::collectionAvailableItemBackground();

            }

            break;
        }

        case AvailableItemGroup:
        {
            if (role == Qt::DisplayRole && col == 0)
            {
                return tr("Available");
            }

            if (role == Qt::DecorationRole && col == 0)
            {
                return gt::gui::icon::layers();
            }

            break;
        }

        case InstalledItemGroup:
        {
            if (role == Qt::DisplayRole && col == 0)
            {
                return tr("Installed");
            }

            if (role == Qt::DecorationRole && col == 0)
            {
                return gt::gui::icon::server();
            }

            break;
        }

        case UpdateAvailableItem:
        {
            if (role == Qt::DisplayRole)
            {
                const int row = index.row();

                if (row < 0 || row >= m_updateAvailableItems.size())
                {
                    return QVariant();
                }

                if (col == 0)
                {
                    return m_updateAvailableItems[row].item().ident();
                }

                if (col == 2)
                {
                    return QString::number(
                               m_updateAvailableItems[row].item().version());
                }

                if (col == 3)
                {
                    return QString::number(
                               m_updateAvailableItems[row].item().installedVersion());
                }
            }
            else if (role == Qt::DecorationRole)
            {
                if (col == 0)
                {
                    return gt::gui::icon::plugin();
                }

                if (col == 1)
                {
                    return gt::gui::icon::info2();
                }
            }
            else if (role == Qt::CheckStateRole && col == 0)
            {
                const int row = index.row();

                if (row < 0 || row >= m_updateAvailableItems.size())
                {
                    return QVariant();
                }

                if (m_updateAvailableItems[row].isSelected())
                {
                    return Qt::Checked;
                }

                return Qt::Unchecked;
            }

            break;
        }

        case AvailableItem:
        {
            if (role == Qt::DisplayRole)
            {
                const int row = index.row();

                if (row < 0 || row >= m_availableItems.size())
                {
                    return QVariant();
                }

                if (col == 0)
                {
                    return m_availableItems[row].item().ident();
                }

                if (col == 2)
                {
                    return QString::number(m_availableItems[row].item().version());
                }
            }
            else if (role == Qt::DecorationRole)
            {
                if (col == 0)
                {
                    return gt::gui::icon::plugin();
                }

                if (col == 1)
                {
                    return gt::gui::icon::info2();
                }
            }
            else if (role == Qt::CheckStateRole && col == 0)
            {
                const int row = index.row();

                if (row < 0 || row >= m_availableItems.size())
                {
                    return QVariant();
                }

                if (m_availableItems[row].isSelected())
                {
                    return Qt::Checked;
                }

                return Qt::Unchecked;
            }

            break;
        }

        case InstalledItem:
        {
            if (role == Qt::DisplayRole)
            {
                const int row = index.row();

                if (row < 0 || row >= m_installedItems.size())
                {
                    return QVariant();
                }

                if (col == 0)
                {
                    return m_installedItems[row].item().ident();
                }

                if (col == 2)
                {
                    return QString::number(m_installedItems[row].item().version());
                }

                if (col == 3)
                {
                    return QString::number(
                               m_installedItems[row].item().installedVersion());
                }
            }
            else if (role == Qt::DecorationRole)
            {
                if (col == 0)
                {
                    return gt::gui::icon::plugin();
                }

                if (col == 1)
                {
                    return gt::gui::icon::info2();
                }
            }
            else if (role == Qt::ForegroundRole)
            {
                return gt::gui::color::disabled();
            }

            break;
        }
    }

    return QVariant();
}

bool
GtCollectionBrowserModel::setData(const QModelIndex& index,
                                  const QVariant& value, int role)
{
    if (!index.isValid())
    {
        return QAbstractItemModel::setData(index, value, role);
    }

    const int col = index.column();

    const auto itemType = static_cast<CollectionItemType>
                                        (index.internalId());

    switch (itemType)
    {

        case UpdateAvailableItem:
        {
            if (role == Qt::CheckStateRole && col == 0)
            {
                const int row = index.row();

                if (row < 0 || row >= m_updateAvailableItems.size())
                {
                    return QAbstractItemModel::setData(index, value, role);
                }

                Qt::CheckState state =
                    static_cast<Qt::CheckState>(value.toInt());

                if (state == Qt::Checked)
                {
                    m_updateAvailableItems[row].setSelected(true);
                }
                else
                {
                    m_updateAvailableItems[row].setSelected(false);
                }

                emit dataChanged(index, index);
                emit selectionChanged();
            }

            break;
        }

        case AvailableItem:
        {
            if (role == Qt::CheckStateRole && col == 0)
            {
                const int row = index.row();

                if (row < 0 || row >= m_availableItems.size())
                {
                    return QAbstractItemModel::setData(index, value, role);
                }

                Qt::CheckState state =
                    static_cast<Qt::CheckState>(value.toInt());

                if (state == Qt::Checked)
                {
                    m_availableItems[row].setSelected(true);
                }
                else
                {
                    m_availableItems[row].setSelected(false);
                }

                emit dataChanged(index, index);
                emit selectionChanged();
            }

            break;
        }
        case InstalledItem:
        case InstalledItemGroup:
        case AvailableItemGroup:
        case UpdateAvailableItemGroup:
            ;// do nothing
    }

    return QAbstractItemModel::setData(index, value, role);
}

QVariant
GtCollectionBrowserModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QAbstractItemModel::headerData(section, orientation, role);
    }

    switch (role)
    {
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
            else if (section == 3)
            {
                return tr("Installed Version");
            }

            break;

        default:
            break;
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void
GtCollectionBrowserModel::setCollectionData(const
        QList<GtCollectionNetworkItem>& installedItems,
        const QList<GtCollectionNetworkItem>& availableItems,
        const QList<GtCollectionNetworkItem>& updataAvailableItems)
{
    beginResetModel();

    m_installedItems.clear();
    m_availableItems.clear();
    m_updateAvailableItems.clear();

    foreach (GtCollectionNetworkItem item, installedItems)
    {
        m_installedItems << GtCollectionBrowserModelItem(item);
    }

    foreach (GtCollectionNetworkItem item, availableItems)
    {
        m_availableItems << GtCollectionBrowserModelItem(item);
    }

    foreach (GtCollectionNetworkItem item, updataAvailableItems)
    {
        m_updateAvailableItems << GtCollectionBrowserModelItem(item);
    }

    endResetModel();

    emit selectionChanged();
}

QModelIndex
GtCollectionBrowserModel::index(int row, int column,
                                const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return {};
    }

    if (!parent.isValid())
    {
        QList<GtCollectionBrowserModel::CollectionItemType> groups =
            groupTypes();

        if (row < 0 || row >= groups.size())
        {
            return {};
        }

        return createIndex(row, column, groups[row]);
    }

    const auto itemType = static_cast<CollectionItemType>(parent.internalId());

    if (itemType == UpdateAvailableItemGroup)
    {
        return createIndex(row, column, UpdateAvailableItem);
    }

    if (itemType == AvailableItemGroup)
    {
        return createIndex(row, column, AvailableItem);
    }

    if (itemType == InstalledItemGroup)
    {
        return createIndex(row, column, InstalledItem);
    }

    return {};

}

QModelIndex
GtCollectionBrowserModel::parent(const QModelIndex& index) const
{
    const auto itemType = static_cast<CollectionItemType>(index.internalId());

    switch (itemType)
    {
        case UpdateAvailableItemGroup:
        case AvailableItemGroup:
        case InstalledItemGroup:
        default:
            return {};

        case UpdateAvailableItem:
            return createIndex(0, 0, UpdateAvailableItemGroup);

        case AvailableItem:
            return createIndex(1, 0, AvailableItemGroup);

        case InstalledItem:
            return createIndex(1, 0, InstalledItemGroup);
    }

    return {};
}

Qt::ItemFlags
GtCollectionBrowserModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    if (index.isValid() && index.parent().isValid() && index.column() == 0)
    {
        const CollectionItemType itemType = static_cast<CollectionItemType>
                                            (index.internalId());

        switch (itemType)
        {
            case UpdateAvailableItem:
            case AvailableItem:
            {
                flags = flags | Qt::ItemIsUserCheckable;
            }
            default:
                ;// do nothing

        }
    }

    return flags;
}

GtCollectionItem
GtCollectionBrowserModel::itemFromIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return GtCollectionItem();
    }

    if (index.model() != this)
    {
        return GtCollectionItem();
    }

    QModelIndex parent = index.parent();

    if (!parent.isValid())
    {
        return GtCollectionItem();
    }

    const int row = index.row();

    const auto itemType = static_cast<CollectionItemType>(index.internalId());

    switch (itemType)
    {
        case UpdateAvailableItem:
        {
            if (row < 0 || row >= m_updateAvailableItems.size())
            {
                return GtCollectionItem();
            }

            return m_updateAvailableItems[row].item();
        }

        case AvailableItem:
        {
            if (row < 0 || row >= m_availableItems.size())
            {
                return GtCollectionItem();
            }

            return m_availableItems[row].item();
        }

        case InstalledItem:
        {
            if (row < 0 || row >= m_installedItems.size())
            {
                return GtCollectionItem();
            }

            return m_installedItems[row].item();
        }
        case InstalledItemGroup:
        case AvailableItemGroup:
        case UpdateAvailableItemGroup:
            return {};
    }

    return GtCollectionItem();
}

QList<GtCollectionNetworkItem>
GtCollectionBrowserModel::selectedItems()
{
    QList<GtCollectionNetworkItem> retval;

    foreach (GtCollectionBrowserModelItem modelItem, m_updateAvailableItems)
    {
        if (modelItem.isSelected())
        {
            retval << modelItem.item();
        }
    }

    foreach (GtCollectionBrowserModelItem modelItem, m_availableItems)
    {
        if (modelItem.isSelected())
        {
            retval << modelItem.item();
        }
    }

    return retval;
}

QList<GtCollectionNetworkItem>
GtCollectionBrowserModel::itemsToUpdate()
{
    QList<GtCollectionNetworkItem> retval;

    foreach (GtCollectionBrowserModelItem modelItem, m_updateAvailableItems)
    {
        retval << modelItem.item();
    }

    return retval;
}

void
GtCollectionBrowserModel::selectAll()
{
    beginResetModel();

    for (auto& item : m_updateAvailableItems)
    {
        item.setSelected(true);
    }

    for (auto& item : m_availableItems)
    {
        item.setSelected(true);
    }

    endResetModel();
}

void
GtCollectionBrowserModel::unselectAll()
{
    beginResetModel();

    for (auto& item : m_updateAvailableItems)
    {
        item.setSelected(false);
    }

    for (auto& item : m_availableItems)
    {
        item.setSelected(false);
    }

    endResetModel();
}

QList<GtCollectionBrowserModel::CollectionItemType>
GtCollectionBrowserModel::groupTypes() const
{
    QList<GtCollectionBrowserModel::CollectionItemType> retval;

    if (!m_updateAvailableItems.isEmpty())
    {
        retval << UpdateAvailableItemGroup;
    }

    if (!m_availableItems.isEmpty())
    {
        retval << AvailableItemGroup;
    }

    if (!m_installedItems.isEmpty())
    {
        retval << InstalledItemGroup;
    }

    return retval;
}
