/* GTlab - Gas Turbine laboratory
 * copyright 2023 by DLR
 *
 * Author: Martin (TheBrain) Siggel (AT-TWK)
 */

#include "gt_qmlobjectlistmodel.h"

GtQmlObjectListModel::GtQmlObjectListModel(QObject *parent) : QAbstractListModel(parent)
{}

void
GtQmlObjectListModel::addObject(QObject *obj)
{
    if (!obj) return;

    auto start = objects.size();
    beginInsertRows(QModelIndex(), start, start);
    objects.push_back(obj);
    endInsertRows();
}

int
GtQmlObjectListModel::rowCount(const QModelIndex &) const
{
    return objects.size();
}

QVariant
GtQmlObjectListModel::data(const QModelIndex &index, int) const
{
    if (!index.isValid())
    {
        return {};
    }

    QObject* item = objects.at(index.row());
    return QVariant::fromValue(item);
}

QHash<int, QByteArray>
GtQmlObjectListModel::roleNames() const
{
    return {std::make_pair(Qt::UserRole + 1, "object")};
}
