/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTLISTMODEL_H
#define GTLISTMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <vector>

namespace gt
{
    template <typename T>
    QVariant asVariant(T&& t) = delete;
}

template <typename T>
class GtListModel : public QAbstractListModel
{
public:
    using iterator = typename std::vector< T>::iterator;

    explicit GtListModel(QObject* parent = nullptr)
        : QAbstractListModel(parent)
    {
    }

    void setListData(const std::vector<T>& data)
    {
        beginResetModel();
        m_data = data;
        endResetModel();
    }

    bool insertItem(int index, const T& value)
    {
        if (index < 0 || index > static_cast<int>(m_data.size()))
        {
            return false; // Invalid index
        }

        beginInsertRows(QModelIndex(), index, index);
        m_data.insert(m_data.begin() + index, value);
        endInsertRows();
        return true;
    }

    iterator begin()
    {
        return m_data.begin();
    }

    iterator end()
    {
        return m_data.end();
    }

    bool append(const T& value)
    {
        return insertItem(static_cast<int>(m_data.size()), value);
    }

    bool removeItem(int index)
    {
        if (index < 0 || index >= static_cast<int>(m_data.size()))
        {
            return false; // Invalid index
        }

        beginRemoveRows(QModelIndex(), index, index);
        m_data.erase(m_data.begin() + index);
        endRemoveRows();
        return true;
    }

    // Get row count (number of items in the list)
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return static_cast<int>(m_data.size());
    }

    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() ||
            index.row() >= static_cast<int>(m_data.size()) ||
            role != Qt::DisplayRole)
        {
            return {};
        }

        return gt::asVariant(m_data[index.row()]);
    }

    // Define role names for QML
    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[Qt::DisplayRole] = "value";
        return roles;
    }


protected:
    std::vector<T> m_data;
};


#endif // GTLISTMODEL_H
