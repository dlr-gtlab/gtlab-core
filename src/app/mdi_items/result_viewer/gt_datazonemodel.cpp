/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonemodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QBrush>
#include <QFont>

#include "gt_datazone.h"
#include "gt_datazone0d.h"
#include "gt_logging.h"

#include "gt_datazonemodel.h"

namespace
{
    double value2D(int row, int column, int axOfInterest, int indexOfInterest,
                   const GtDataZone& dz, const GtDataZone::Data& data)
    {
        const auto& params = data.params();

        if (params.size() < row - 1)
        {
            gtDebug() << "Non mathing sizes: params" << params.size()
                      << " rowIndex" << row;
            return -1;
        }

        auto firstAxisTicks = dz.allAxisTicks().first();
        auto secondAxisTicks = dz.allAxisTicks().at(1);

        if (axOfInterest == 0)
        {
            if (firstAxisTicks.size() < indexOfInterest)
            {
                return -1;
            }
            if (secondAxisTicks.size() < column - 2)
            {
                return -1;
            }

            return data.value2D(params[row - 1],
                    firstAxisTicks.at(indexOfInterest),
                    secondAxisTicks.at(column - 2));
        }

        if (axOfInterest == 1)
        {
            if (firstAxisTicks.size() < column - 2)
            {
                return -1;
            }
            if (secondAxisTicks.size() < indexOfInterest)
            {
                return -1;
            }

            return data.value2D(params[row - 1],
                    firstAxisTicks.at(column - 2),
                    secondAxisTicks.at(indexOfInterest));
        }

        return -1;
    }
}

GtDataZoneModel::GtDataZoneModel(QObject* parent) :
    QAbstractTableModel(parent)
{

}

void
GtDataZoneModel::setResultData(GtAbstractDataZone* dataZone)
{
    if (!dataZone)
    {
        return;
    }

    auto data{dataZone->fetchData()};

    if (!data.isValid())
    {
        return;
    }

    if (m_result)
    {
        disconnect(m_result.get(), SIGNAL(dataChanged(GtObject*)), this,
                   SLOT(onResultChanged()));

        disconnect(this, SIGNAL(axIndexChanges(int)), this,
                   SLOT(indexChanged()));
        disconnect(this, SIGNAL(tickIndexChanges(int)), this,
                   SLOT(indexChanged()));
    }

    beginResetModel();

    m_result.set(dataZone);

    connect(m_result.get(), SIGNAL(dataChanged(GtObject*)), this,
            SLOT(onResultChanged()));

    connect(this, SIGNAL(axIndexChanges(int)), this,
               SLOT(indexChanged()));
    connect(this, SIGNAL(tickIndexChanges(int)), this,
               SLOT(indexChanged()));

    endResetModel();
}

GtAbstractDataZone*
GtDataZoneModel::getResultData()
{
    return m_result.get();
}

void
GtDataZoneModel::clearResultData()
{
    beginResetModel();
    m_result.clear();
    endResetModel();
}

int
GtDataZoneModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_result)
    {
        return 0;
    }

    if (m_result->nDims() > 2)
    {
        return 0;
    }

    if (m_result->nDims() == 0)
    {
        return m_result.data()->params().count();
    }
    return m_result.data()->params().count() + 1;
}

int
GtDataZoneModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !m_result)
    {
        return 0;
    }

    if (auto* dz = qobject_cast<GtDataZone*>(m_result))
    {
        auto allTicks = dz->allAxisTicks();

        if (dz->nDims() == 1)
        {
            return allTicks.first().size() + 2;
        }

        if (dz->nDims() == 2)
        {
            int index = std::abs(m_axOfInterest - 1);

            if (allTicks.size() < index)
            {
                //gtWarning() << "Error in ColumnCount for DataZone";
                //gtDebug() << "This error only occurs in Debug-Mode";
                return 0;
            }

            return allTicks.at(index).size() + 2;
        }

        return 0;
    }

    return 3;
}

QVariant
GtDataZoneModel::data(const QModelIndex& index,
                      int role) const
{
    if (!m_result)
    {
        gtDebug() << "Error - GtDataZoneModel::data - m_result == nullptr";
        return QVariant();
    }

    auto* absData = m_result.data();

    QStringList params = absData->params();
    QStringList units = absData->units();

    auto col = index.column();
    auto row = index.row();

    if (row < 0 || row >= params.size() + 1)
    {
        gtDebug() << "Error - GtDataZoneModel::data - row not ok";
        gtDebug() << "row:" << row << "params.size" << params.size();
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (auto* dz = qobject_cast<GtDataZone*>(m_result))
        {
            if (dz->nDims() == 1)
            {
                auto axisTicks = dz->allAxisTicks().first();

                if (col < 0 || col >= axisTicks.size() + 2)
                {
                    gtDebug() << "Error - GtDataZoneModel::data - 1D -"
                                 " column not ok";
                    return QVariant();
                }

                /// header line
                if (row == 0)
                {
                    QString name = dz->axisNames().first();
                    int pIdx = params.indexOf(name);

                    switch (col)
                    {
                        /// parameter
                        case 0:
                            return name;
                        /// unit
                        case 1:
                            if (pIdx >= units.size() || pIdx == -1)
                            {
                                return "unit";
                            }
                            return units.at(pIdx);
                        /// tick
                        default:
                            return axisTicks.at(col - 2);
                    }
                }

                /// data row
                switch (col)
                {
                    /// parameter
                    case 0:
                        return params[row - 1];
                    /// unit
                    case 1:
                        return units[row - 1];
                    /// value
                    default:
                        return dz->fetchData().value1D(params[row - 1],
                                                       axisTicks.at(col - 2));
                }
            }
            else if (dz->nDims() == 2)
            {
                int variableAxisIndex = std::abs(m_axOfInterest - 1);
                auto axisTicks = dz->allAxisTicks().at(variableAxisIndex);

                if (col < 0 || col >= axisTicks.size() + 2)
                {
                    gtDebug() << "Error - GtDataZoneModel::data - 2D -"
                                 " column not ok";
                    return QVariant();
                }

                /// header line
                if (row == 0)
                {
                    auto axisNames = dz->axisNames();
                    if (axisNames.size() < variableAxisIndex)
                    {
                        return "name";
                    }

                    auto name = axisNames.at(variableAxisIndex);
                    int pIdx = params.indexOf(name);

                    switch (col)
                    {
                        /// parameter
                        case 0:
                            return name;
                        /// unit
                        case 1:
                            if (pIdx >= units.size() || pIdx == -1)
                            {
                                return "unit";
                            }
                            return units.at(pIdx);
                        /// tick
                        default:
                            if (axisTicks.size() < col - 2)
                            {
                                return -1;
                            }
                            return axisTicks.at(col - 2);
                    }
                }

                /// data row
                switch (col)
                {
                    /// parameter
                    case 0:
                        if (params.size() < row - 1)
                        {
                            return "name";
                        }
                        return params[row - 1];
                    /// unit
                    case 1:
                        if (units.size() < row - 1)
                        {
                            return "unit";
                        }
                        return units[row - 1];
                    /// value
                    default:
                        return value2D(row, col, m_axOfInterest,
                                       m_indexOfInterestAx1,
                                       *dz, dz->fetchData());
                }
            }
        }
        else if (auto* dz0D{qobject_cast<GtDataZone0D*>(m_result)})
        {
            if (col < 0 || col >= 3)
            {
                return QVariant();
            }

            /// data row
            switch (col)
            {
                /// parameter
                case 0:
                    return params[row];
                /// unit
                case 1:
                    return units[row];
                /// value
                case 2:
                    return dz0D->fetchData().values()[row];
            }
        }
    }
    else if (role == Qt::FontRole)
    {
        /// header line
        if (row == 0 && !m_result->is0D() && m_result->nDims() <= 2)
        {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    return QVariant();
}

QVariant
GtDataZoneModel::headerData(int section, Qt::Orientation orientation,
                            int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            switch (section)
            {
                case 0:
                    return tr("Parameter");
                case 1:
                    return tr("Unit");
                default:
                    return tr("Value");
            }
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

void
GtDataZoneModel::onResultChanged()
{
    setResultData(m_result);
}

void
GtDataZoneModel::indexChanged()
{
    setResultData(m_result);
}

