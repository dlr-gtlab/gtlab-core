/* GTlab - Gas Turbine laboratory
 * Source File: gt_environmentmodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_environmentmodel.h"

#include "gt_environment.h"
#include "gt_colors.h"
#include "gt_application.h"
#include "gt_algorithms.h"

#include <QFont>


GtEnvironmentModel::GtEnvironmentModel(const QStringList& vars,
                                       QObject* parent) :
    QAbstractTableModel(parent)
{
    init(vars);
}

int
GtEnvironmentModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

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

    if (index.row() < 0)
    {
        return QVariant();
    }

    const int row = index.row();
    const int col = index.column();

    if (row >= m_vars.size())
    {
        return QVariant();
    }

    //auto iterator = gt::ith_iter(m_vars, size_t(row));

    //const QString valId = iterator.key();

    const QString valId = m_vars.keys().at(row);

    QVariant retVal = QVariant();

    switch (role)
    {
    case Qt::EditRole:
        break;

    case Qt::DisplayRole:
        if (col == 0)
        {
            retVal = valId;
        }
        else if (col == 1)
        {
            retVal = m_vars.value(valId);
        }
        break;

    case Qt::FontRole:
    {
        QFont fnt;
        fnt.setPointSize(8);
        fnt.setFamily("Arial");
        return fnt;
    }
    case Qt::BackgroundRole:
        if (col == 1)
        {
            if (m_vars.value(valId).isNull())
            {
                return gt::gui::color::environmentModelBack();
            }
        }
        break;
    }

    return retVal;


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

    const QString valId = gt::ith_iter(m_vars, size_t(row)).key();

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

        if (section == 1)
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
    gt::for_each_key(m_vars, [this](const QString& e)
    {
        if (!gtEnvironment->environmentVariableExists(e))
        {
            gtEnvironment->addEnvironmentVariable(e);
        }

        QVariant variant = m_vars.value(e);
        gtEnvironment->setValue(e, variant);
    });

    gtEnvironment->saveEnvironment();
}

void
GtEnvironmentModel::init(const QStringList& vars)
{
    beginInsertRows(QModelIndex(), m_vars.size(), m_vars.size());

    foreach (const QString& var, vars)
    {
        m_vars.insert(var, gtEnvironment->value(var));
    }

    endInsertRows();
}
