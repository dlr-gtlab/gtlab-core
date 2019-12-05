/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkablestringlistmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 03.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_checkablestringlistmodel.h"

GtCheckableStringListModel::GtCheckableStringListModel(
    const QStringList& strings, QObject* parent)
    : QAbstractListModel(parent),
      m_mode(MultipleSelection)
{
    setStringList(strings);

    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            SLOT(onDataChanged()));
}

GtCheckableStringListModel::GtCheckableStringListModel(QObject* parent)
    : QAbstractListModel(parent),
      m_mode(MultipleSelection)
{
    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex, QVector<int>)),
            SLOT(onDataChanged()));
}

QStringList
GtCheckableStringListModel::stringList() const
{
    QStringList retval;

    foreach (const StringBoolPair& d, m_data)
    {
        retval << d.first;
    }

    return retval;
}

void
GtCheckableStringListModel::setStringList(const QStringList& strings,
                                          Qt::CheckState state)
{
    beginResetModel();
    m_data.clear();

    foreach (const QString& str, strings)
    {
        m_data << StringBoolPair(str, state);
    }

    endResetModel();
}

void
GtCheckableStringListModel::setSelectionMode(
    GtCheckableStringListModel::SelectionModes mode)
{
    if (m_mode == mode)
    {
        return;
    }

    m_mode = mode;

    if (m_mode == SingleSelection)
    {
        clearSelection();
    }
}

GtCheckableStringListModel::SelectionModes
GtCheckableStringListModel::selectionMode()
{
    return m_mode;
}

int
GtCheckableStringListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_data.count();
}

Qt::ItemFlags
GtCheckableStringListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags flags;

    if (index.isValid())
    {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |
                 Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    else
    {
        flags =  Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |
                 Qt::ItemIsDropEnabled  | Qt::ItemIsEnabled;
    }

    return flags;
}

QVariant
GtCheckableStringListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_data.size())
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            return m_data[index.row()].first;
        }

        case Qt::CheckStateRole:
        {
            return m_data[index.row()].second;
        }

        default:
            break;
    }

    return QVariant();
}

bool
GtCheckableStringListModel::setData(const QModelIndex& index,
                                    const QVariant& value,
                                    int role)
{
    if (index.row() < 0 || index.row() >= m_data.size())
    {
        return false;
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            m_data[index.row()].first = value.toString();
            emit dataChanged(index, index);
            return true;
        }
        case Qt::CheckStateRole:
        {
            if (m_mode == MultipleSelection)
            {
                m_data[index.row()].second =
                    static_cast<Qt::CheckState>(value.toInt());
                emit dataChanged(index, index);
                return true;
            }
            else if (m_mode == SingleSelection)
            {
                for (int i = 0; i < m_data.size(); i++)
                {
                    if  (i == index.row())
                    {
                        m_data[index.row()].second =
                            static_cast<Qt::CheckState>(value.toInt());
                    }
                    else
                    {
                        m_data[i].second = Qt::Unchecked;
                    }
                }
                emit dataChanged(createIndex(0, 0),
                                 createIndex(m_data.size() - 1, 0));
            }
        }
        default:
            break;
    }

    return false;
}

void
GtCheckableStringListModel::selectItems(const QStringList& strings)
{
    beginResetModel();


    if (m_mode == MultipleSelection)
    {
        for (int i = 0; i < m_data.size(); i++)
        {
            if (strings.contains(m_data[i].first))
            {
                m_data[i].second = Qt::Checked;
            }
            else
            {
                m_data[i].second = Qt::Unchecked;
            }
        }
    }
    else if (m_mode == SingleSelection)
    {
        clearSelection();

        if (!strings.isEmpty())
        {
            // check only first entry
            for (int i = 0; i < m_data.size(); i++)
            {
                if (m_data[i].first == strings.first())
                {
                    m_data[i].second = Qt::Checked;
                }
            }
        }
    }

    endResetModel();

    emit selectionChanged();
}

void
GtCheckableStringListModel::selectItem(const QString& string)
{
    if (string.isEmpty())
    {
        return;
    }

    if (m_mode == SingleSelection)
    {
        selectItems(QStringList() << string);
    }
    else if (m_mode == MultipleSelection)
    {
        for (int i = 0; i < m_data.size(); i++)
        {
            if (m_data[i].first == string)
            {
                m_data[i].second = Qt::Checked;
            }
        }
    }

    emit selectionChanged();
}

void
GtCheckableStringListModel::clearSelection()
{
    beginResetModel();

    for (int i = 0; i < m_data.size(); ++i)
    {
        m_data[i].second = Qt::Unchecked;
    }

    endResetModel();

    emit selectionChanged();
}

void
GtCheckableStringListModel::selectAll()
{
    beginResetModel();

    for (int i = 0; i < m_data.size(); ++i)
    {
        m_data[i].second = Qt::Checked;
    }

    endResetModel();

    emit selectionChanged();
}

QStringList
GtCheckableStringListModel::selectedStringList()
{
    QStringList retval;

    foreach (const StringBoolPair& d, m_data)
    {
        if (d.second == Qt::Checked)
        {
            retval << d.first;
        }
    }

    return retval;
}

bool
GtCheckableStringListModel::insertRows(int row, int count,
                                       const QModelIndex& parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
    {
        return false;
    }

    beginInsertRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r)
    {
        m_data.insert(row, StringBoolPair(QString(), Qt::Checked));
    }

    endInsertRows();

    return true;
}

bool
GtCheckableStringListModel::removeRows(int row, int count,
                                       const QModelIndex& parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
    {
        return false;
    }

    beginRemoveRows(QModelIndex(), row, row + count - 1);

    for (int r = 0; r < count; ++r)
    {
        m_data.removeAt(row);
    }

    endRemoveRows();

    return true;
}

Qt::DropActions
GtCheckableStringListModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void
GtCheckableStringListModel::onDataChanged()
{
    QStringList retval = selectedStringList();

    if (retval.isEmpty())
    {
        emit noItemSelected(true);
    }
    else
    {
        emit noItemSelected(false);
    }

    selectItems(retval);
}
