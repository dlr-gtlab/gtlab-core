/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulemodel.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.07.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_controlschedulemodel.h"

#include <QIcon>

#include "gt_logging.h"
#include "gt_application.h"
#include "gt_controlscheduleplot.h"

GtControlScheduleModel::GtControlScheduleModel(QObject* parent):
    QAbstractItemModel(parent), m_plot(Q_NULLPTR), m_editAllowed(false)
{

}

GtControlScheduleModel::GtControlScheduleModel(GtControlSchedulePlot* plot,
                                               QObject* parent) :
    QAbstractItemModel(parent), m_plot(plot), m_editAllowed(false)
{

}

int
GtControlScheduleModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int
GtControlScheduleModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (m_plot == Q_NULLPTR)
    {
        return 0;
    };

    return m_plot->nPoints();
}

QVariant
GtControlScheduleModel::data(const QModelIndex &index, int role) const
{
    QVariant retval;

    if (!index.isValid())
    {
        return QVariant();
    }

    if (m_plot == Q_NULLPTR)
    {
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (row >= m_plot->nPoints())
    {
        return QVariant();
    }

    if (column >= 2)
    {
        return QVariant();
    }

    switch(role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        GtControlScheduleMarker* marker = m_plot->tmpMarkerList().at(row);
        if (marker == Q_NULLPTR)
        {
            return QVariant();
        }

        if (column == 0)
        {
            retval = QString::number(marker->value().x());
        }
        else if (column == 1)
        {
            retval = QString::number(marker->value().y());
        }
        else
        {
            return QVariant();
        }

        return retval;
    }

    default:
        break;
    }

    return retval;
}

QModelIndex
GtControlScheduleModel::index(int row, int column,
                              const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_plot == Q_NULLPTR)
    {
        return QModelIndex();
    }
    else if (row >= m_plot->nPoints())
    {
        return QModelIndex();
    }
    else if (row < 0)
    {
        return QModelIndex();
    }
    else if (column < 0)
    {
        return QModelIndex();
    }
    else if (column >= 2)
    {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex
GtControlScheduleModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

bool
GtControlScheduleModel::setData(const QModelIndex &index,
                                const QVariant &value, int role)
{
    if (!index.isValid())
    {
        return false;
    }

    if (m_plot == Q_NULLPTR)
    {
        return false;
    }

    int row = index.row();
    int column = index.column();

    if (row >= m_plot->tmpMarkerList().size())
    {
        return false;
    }

    if (column >= 2)
    {
        return false;
    }

    switch(role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        GtControlScheduleMarker* marker = m_plot->tmpMarkerList().at(row);

        if (marker == Q_NULLPTR)
        {
            return false;
        }

        if (column == 0)
        {
            m_plot->setMarkerPosition(marker,
                                      value.toDouble(), marker->value().y());

            m_plot->selectMarker(marker);
        }
        else if (column == 1)
        {
            m_plot->setMarkerPosition(marker,
                                      marker->value().x(), value.toDouble());

            m_plot->selectMarker(marker);
        }
    }

    default:
        break;
    }

    return true;
}

void
GtControlScheduleModel::setPlot(GtControlSchedulePlot* plot)
{
    if (m_plot != Q_NULLPTR)
    {
        disconnect(m_plot);
    }

    m_plot = plot;

    connect(m_plot, SIGNAL(editModeChanged(bool)),
            this, SLOT(onEditModeChanged(bool)));

    connect(m_plot, SIGNAL(pointAddedOrRemoved()),
            this, SLOT(onDataChanged()));

    onDataChanged();
}

Qt::ItemFlags
GtControlScheduleModel::flags(const QModelIndex &index) const
{
    if (m_editAllowed)
    {
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    else
    {
        return QAbstractItemModel::flags(index) & ~Qt::ItemIsEditable
                & ~Qt::ItemIsSelectable;
    }
}

QVariant
GtControlScheduleModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (m_plot == Q_NULLPTR)
    {
        return QVariant();
    }

    switch(role)
    {
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Vertical)
        {
            if (section < 0 ||
                    section >= m_plot->tmpMarkerList().size())
            {
                return QVariant();
            }

            return QString::number(section + 1);
        }
        else if (orientation == Qt::Horizontal)
        {
            if (section < 0 || section >= columnCount())
            {
                return QVariant();
            }

            if (section == 0)
            {
                return m_plot->xTitle();
            }
            else if (section == 1)
            {
                return m_plot->yTitle();
            }
        }
    }

    default:
        break;
    }

    return QVariant();
}

void
GtControlScheduleModel::onEditModeChanged(bool active)
{
    m_editAllowed = active;
}

void
GtControlScheduleModel::onDataChanged()
{
    beginResetModel();
    endResetModel();
}
