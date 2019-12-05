/* GTlab - Gas Turbine laboratory
 * Source File: gt_environmentmodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFont>
#include <QColor>

#include "gt_environment.h"

#include "gt_environmentmodel.h"

GtEnvironmentModel::GtEnvironmentModel(const QStringList& vars,
                                       QObject* parent) :
    QAbstractTableModel(parent)
{
    init(vars);
}

int
GtEnvironmentModel::rowCount(const QModelIndex& /*parent*/) const
{
    return m_vars.size();
}

int
GtEnvironmentModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

QVariant
GtEnvironmentModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    const int row = index.row();
    const int col = index.column();

    if (row >= m_vars.size())
    {
        return QVariant();
    }

    const QString valId = m_vars.keys()[row];

    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        if (col == 0)
        {
            return valId;
        }
        else if (col == 1)
        {
            return m_vars.value(valId);
        }
    }
    case Qt::FontRole:
    {
        QFont fnt;
        fnt.setPointSize(8);
        fnt.setFamily("Arial");
        return fnt;
    }
    case Qt::BackgroundRole:
    {
        if (col == 0)
        {
            return QColor(245, 245, 245);
        }
        else if (col == 1)
        {
            if (m_vars.value(valId).isNull())
            {
                return QColor(214, 170, 170);
            }
        }

        return QVariant();
    }
    }

    return QVariant();
}

bool
GtEnvironmentModel::setData(const QModelIndex& index,
                            const QVariant& value,
                            int role)
{
    if (!index.isValid())
    {
        return false;
    }

    const int row = index.row();
    const int col = index.column();

    if (row >= m_vars.size())
    {
        return false;
    }

    const QString valId = m_vars.keys()[row];

    switch (role)
    {
    case Qt::EditRole:
    {
        if (col == 1)
        {
            if (value.toString().isEmpty())
            {
                m_vars[valId] = QVariant();
            }
            else
            {
                m_vars[valId] = value;
            }
            emit dataChanged(index, index, QVector<int>() << Qt::DisplayRole);
            return true;
        }
    }
    }

    return QAbstractTableModel::setData(index, value, role);
}

QVariant
GtEnvironmentModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return QStringLiteral("Variable");
        }
        else if (section == 1)
        {
            return QStringLiteral("Value");
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags
GtEnvironmentModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags baseFlags = QAbstractItemModel::flags(index);

    if (index.column() == 1)
    {
        return baseFlags |= Qt::ItemIsEditable;
    }

    return baseFlags;
}

void
GtEnvironmentModel::saveVariables()
{
    for (auto e : m_vars.keys())
    {
        if (!gtEnvironment->environmentVariableExists(e))
        {
            gtEnvironment->addEnvironmentVariable(e);
        }

        QVariant variant = m_vars.value(e);
        gtEnvironment->setValue(e, variant);
    }

    gtEnvironment->saveEnvironment();
}

void
GtEnvironmentModel::init(const QStringList& vars)
{
    foreach (const QString& var, vars)
    {
        m_vars.insert(var, gtEnvironment->value(var));
    }
}
