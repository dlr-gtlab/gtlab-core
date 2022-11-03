/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_filteredlogmodel.h"
#include "gt_logmodel.h"

#include <gt_loglevel.h>

void
GtFilteredLogModel::toggleDebugLevel(bool val)
{
    beginResetModel();
    m_debugLevel = val;
    endResetModel();
}

void
GtFilteredLogModel::toggleInfoLevel(bool val)
{
    beginResetModel();
    m_infoLevel = val;
    endResetModel();
}

void
GtFilteredLogModel::toggleWarningLevel(bool val)
{
    beginResetModel();
    m_warningLevel = val;
    endResetModel();
}

void
GtFilteredLogModel::toggleErrorLevel(bool val)
{
    beginResetModel();
    m_errorLevel = val;
    endResetModel();
}

void
GtFilteredLogModel::toggleFatalLevel(bool val)
{
    beginResetModel();
    m_fatalLevel = val;
    endResetModel();
}

void
GtFilteredLogModel::filterData(const QString& val)
{
    setFilterRegExp(val);
    invalidate();
}

GtFilteredLogModel::GtFilteredLogModel(QObject* parent) :
    QSortFilterProxyModel(parent),
    m_debugLevel(true),
    m_infoLevel(true),
    m_warningLevel(true),
    m_errorLevel(true),
    m_fatalLevel(true)
{

}

bool
GtFilteredLogModel::filterAcceptsRow(int source_row,
                                     const QModelIndex& source_parent) const
{
    bool doFilter = true;

    const QModelIndex index = sourceModel()->index(source_row, 0,
                                                   source_parent);

    const int level = gtLogModel->data(index, GtLogModel::LogLevelRole).toInt();

    switch (level)
    {
    case gt::log::DebugLevel:
        doFilter = m_debugLevel;
        break;
    case gt::log::InfoLevel:
        doFilter = m_infoLevel;
        break;
    case gt::log::WarnLevel:
        doFilter = m_warningLevel;
        break;
    case gt::log::ErrorLevel:
        doFilter = m_errorLevel;
        break;
    case gt::log::FatalLevel:
        doFilter = m_fatalLevel;
        break;
    }

    if (doFilter)
    {
        return QSortFilterProxyModel::filterAcceptsRow(source_row,
                                                       source_parent);
    }

    return false;
}

