/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelusagemodel.cpp
 *
 *  Created on: 21.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_label.h"

#include "gt_labelusagemodel.h"

GtLabelUsageModel::GtLabelUsageModel(QObject* parent) :
    QAbstractListModel(parent)
{

}

int
GtLabelUsageModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_labelPaths.size();
}

QVariant
GtLabelUsageModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.model() != this)
    {
        return QVariant();
    }

    switch (role)
    {
//        case Qt::EditRole:
        case Qt::DisplayRole:
        {
            return m_labelPaths[index.row()];
        }
    }

    return QVariant();
}

void
GtLabelUsageModel::setLabelUsage(const QList<GtLabel*>& labels)
{
    beginResetModel();
    m_labelPaths.clear();
    foreach (GtLabel* label, labels)
    {
        m_labelPaths << label->objectPath();
    }
    endResetModel();
}
