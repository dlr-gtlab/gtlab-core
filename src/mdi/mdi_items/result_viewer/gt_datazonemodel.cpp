/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonemodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 04.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QColor>
#include <QBrush>
#include <QFont>

#include "gt_datazone.h"
#include "gt_datazone0d.h"

#include "gt_datazonemodel.h"

GtDataZoneModel::GtDataZoneModel(QObject* parent) : QAbstractTableModel(parent),
    m_result(Q_NULLPTR)
{

}

void
GtDataZoneModel::setResultData(GtAbstractDataZone* data)
{
    if (data == Q_NULLPTR)
    {
        return;
    }

    if (m_result != Q_NULLPTR)
    {
        disconnect(m_result.data(), SIGNAL(dataChanged(GtObject*)), this,
                   SLOT(onResultChanged()));


        disconnect(this, SIGNAL(axIndexChanges(int)), this,
                   SLOT(indexChanged()));
        disconnect(this, SIGNAL(tickIndexChanges(int)), this,
                   SLOT(indexChanged()));
    }

    beginResetModel();

    m_result = data;
    connect(m_result.data(), SIGNAL(dataChanged(GtObject*)), this,
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
    return m_result;
}

double
value2D(int row, int column, int axOfInterest, int indexOfInterest,
        GtDataZone* dz)
{
    if (!dz)
    {
        gtDebug() << "Datazone is NULLPTR";
        return -1;
    }

    QStringList params = dz->params();

    if (params.size() < row - 1)
    {
        gtDebug() << "Non mathing sizes: params" << params.size()
                  << " rowIndex" << row;
        return -1;
    }

    if (axOfInterest == 0)
    {
        if (dz->allAxisTicks().first().size() < indexOfInterest)
        {
            return -1;
        }
        if (dz->allAxisTicks().at(1).size() < column - 2)
        {
            return -1;
        }


        return dz->value2D(params[row - 1],
                dz->allAxisTicks().first().at(indexOfInterest),
                dz->allAxisTicks().at(1).at(column - 2));
    }
    else if (axOfInterest == 1)
    {
        if (dz->allAxisTicks().first().size() < column - 2)
        {
            return -1;
        }
        if (dz->allAxisTicks().at(1).size() < indexOfInterest)
        {
            return -1;
        }

        return dz->value2D(params[row - 1],
                dz->allAxisTicks().first().at(column - 2),
                dz->allAxisTicks().at(1).at(indexOfInterest));

    }
    else
    {
        return -1;
    }
}

void
GtDataZoneModel::clearResultData()
{
    beginResetModel();
    m_result = Q_NULLPTR;
    endResetModel();
}

int
GtDataZoneModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || m_result == Q_NULLPTR)
    {
        return 0;
    }

    if (m_result->nDims() > 2)
    {
        return 0;
    }

    if (m_result->nDims() == 0)
    {
        return m_result->params().count();
    }
    return m_result->params().count() + 1;
}

int
GtDataZoneModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid() || m_result == Q_NULLPTR)
    {
        return 0;
    }

    GtDataZone* dz = qobject_cast<GtDataZone*>(m_result);

    if (dz)
    {
        if (dz->nDims() == 1)
        {
            return dz->allAxisTicks().first().size() + 2;
        }
        else if (dz->nDims() == 2)
        {
            int index = abs(m_axOfInterest - 1);

            if (dz->allAxisTicks().size() < index)
            {
                //gtWarning() << "Error in ColumnCount for DataZone";
                //gtDebug() << "This error only occurs in Debug-Mode";
                return 0;
            }

            return dz->allAxisTicks().at(index).size() + 2;
        }
        else
        {
            return 0;
        }
    }

    return 3;
}

QVariant
GtDataZoneModel::data(const QModelIndex& index,
                      int role) const
{
    if (m_result == Q_NULLPTR)
    {
        gtDebug() << "Error - GtDataZoneModel::data - m_result == Q_NULLPTR";
        return QVariant();
    }

    //gtDebug() << "Element" << index.row() << "/" << index.column();

    QStringList params = m_result->params();
    QStringList units = m_result->units();

    if (index.row() < 0 || index.row() >= params.size() + 1)
    {
        gtDebug() << "Error - GtDataZoneModel::data - row not ok";
        gtDebug() << "row:" << index.row() << "params.size" << params.size();
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        GtDataZone* dz = qobject_cast<GtDataZone*>(m_result);

        if (dz)
        {
            if (dz->nDims() == 1)
            {
                QVector<double> axisTicks = dz->allAxisTicks().first();


                if (index.column() < 0 || index.column()
                    >= dz->allAxisTicks().first().size() + 2)
                {
                    gtDebug() << "Error - GtDataZoneModel::data - 1D -"
                                 " column not ok";
                    return QVariant();
                }

                if (index.row() == 0)
                {
                    QString name = dz->axisNames().first();
                    int indexInParams = dz->params().indexOf(name);

                    switch (index.column())
                    {
                        case 0:
                            return name;
                        case 1:
                            if (dz->units().size() < indexInParams
                                || indexInParams == -1)
                            {
                                return "unit";
                            }
                            return dz->units().at(indexInParams);
                        default:
                            return dz->allAxisTicks().first().at(
                                        index.column() - 2);
                    }
                }

                switch (index.column())
                {
                case 0:
                    return params[index.row() - 1];
                case 1:
                    return units[index.row() - 1];
                default:
                    return dz->value1D(params[index.row() - 1],
                            axisTicks.at(index.column() - 2));
                }

            }
            else if (dz->nDims() == 2)
            {
                int variableAxisIndex = abs(m_axOfInterest - 1);


                if (index.column() < 0 || index.column()
                    >= dz->allAxisTicks().at(variableAxisIndex).size() + 2)
                {
                    gtDebug() << "Error - GtDataZoneModel::data - 2D -"
                                 " column not ok";
                    return QVariant();
                }

                if (index.row() == 0)
                {
                    QString name;

                    if (dz->axisNames().size() < variableAxisIndex)
                    {
                        return "name";
                    }

                    name = dz->axisNames().at(variableAxisIndex);

                    int indexInParams = dz->params().indexOf(name);

                    switch (index.column())
                    {
                        case 0:
                            return name;
                        case 1:
                            if (dz->units().size() < indexInParams
                                || indexInParams == -1)
                            {
                                return "unit";
                            }
                            return dz->units().at(indexInParams);
                        default:
                            if (dz->allAxisTicks().at(variableAxisIndex).size()
                                < index.column()-2)
                            {
                                return -1;
                            }

                            return dz->allAxisTicks().at(variableAxisIndex)
                                    .at(index.column()-2);
                    }
                }

                switch (index.column())
                {
                    case 0:
                        if (params.size() < index.row()-1)
                        {
                            return "name";
                        }
                        return params[index.row()-1];
                    case 1:
                        if (units.size() < index.row()-1)
                        {
                            return "unit";
                        }
                        return units[index.row()-1];
                    default:
                        double val = value2D(index.row(), index.column(),
                                             m_axOfInterest,
                                             m_indeOfInterestAx1,
                                             dz);
                        return val;
                }
            }
        }
        else
        {
            GtDataZone0D* dz0D = qobject_cast<GtDataZone0D*>(m_result);
            if (!dz0D)
            {
                return QVariant();
            }

            QVector<double> values = dz0D->values();

            if (index.column() < 0 || index.column() >= 3)
            {
                return QVariant();
            }

            switch (index.column())
            {
                case 0:
                    return params[index.row()];
                case 1:
                    return units[index.row()];
                case 2:
                    return values[index.row()];
            }
        }
    }
    else if (role == Qt::FontRole)
    {
        GtDataZone* dz = qobject_cast<GtDataZone*>(m_result);

        if (dz)
        {
            if (dz->nDims() > 2)
            {
                return QVariant();
            }

            if (index.row() == 0)
            {
                QFont font;
                font.setBold(true);
                return font;
            }
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

