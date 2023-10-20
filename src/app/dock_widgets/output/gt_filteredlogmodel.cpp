/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_filteredlogmodel.h"
#include "gt_logmodel.h"

#include <gt_loglevel.h>

void
GtFilteredLogModel::filterTraceLevel(bool val)
{
    setFilter(FilterTrace, val);
}

void
GtFilteredLogModel::filterDebugLevel(bool val)
{
    setFilter(FilterDebug, val);
}

void
GtFilteredLogModel::filterInfoLevel(bool val)
{
    setFilter(FilterInfo, val);
}

void
GtFilteredLogModel::filterWarningLevel(bool val)
{
    setFilter(FilterWarning, val);
}

void
GtFilteredLogModel::filterErrorLevel(bool val)
{
    setFilter(FilterError, val);
}

void
GtFilteredLogModel::filterFatalLevel(bool val)
{
    setFilter(FilterFatal, val);
}

void
GtFilteredLogModel::filterData(const QString& val)
{
    setFilterRegExp(val);
    invalidate();
}

void
GtFilteredLogModel::setFilter(FilterLevel level, bool enabled)
{
    beginResetModel();
    if (enabled)
    {
        m_filter |= level;
    }
    else
    {
        m_filter &= ~level;
    }
    endResetModel();
}

GtFilteredLogModel::GtFilteredLogModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    // filter all columns
    setFilterKeyColumn(-1);
}

bool
GtFilteredLogModel::filterAcceptsRow(int srcRow,
                                     const QModelIndex& srcParent) const
{
    bool doFilter = true;

    const QModelIndex index = sourceModel()->index(srcRow, 0, srcParent);

    const int level = gtLogModel->data(index, GtLogModel::LevelRole).toInt();

    switch (level)
    {
    case gt::log::TraceLevel:
        doFilter = m_filter & FilterTrace;
        break;
    case gt::log::DebugLevel:
        doFilter = m_filter & FilterDebug;
        break;
    case gt::log::InfoLevel:
        doFilter = m_filter & FilterInfo;
        break;
    case gt::log::WarningLevel:
        doFilter = m_filter & FilterWarning;
        break;
    case gt::log::ErrorLevel:
        doFilter = m_filter & FilterError;
        break;
    case gt::log::FatalLevel:
        doFilter = m_filter & FilterFatal;
        break;
    }

    return doFilter &&
           QSortFilterProxyModel::filterAcceptsRow(srcRow, srcParent);
}

