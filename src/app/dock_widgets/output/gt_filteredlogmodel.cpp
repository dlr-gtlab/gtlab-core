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
#include "logfilterproxymodel.h"
#include "gt_logmodel.h"

#include "gt_logging.h"

GtFilteredLogModel::GtFilteredLogModel(LogFilterProxyModel* filterModel,
                                       QObject* parent) :
    QSortFilterProxyModel(parent),
    m_filterModel(filterModel),
    m_filter(0x3F)
{
    setFilterKeyColumn(-1);
    m_activeLevels = {gt::log::TraceLevel, gt::log::DebugLevel, 
                      gt::log::InfoLevel, gt::log::WarningLevel,
                      gt::log::ErrorLevel, gt::log::FatalLevel};
    if (m_filterModel)
    {
        m_filterModel->setLevelFilter(m_activeLevels);
    }
}

void
GtFilteredLogModel::filterTraceLevel(bool val)
{
    setFilter(1 << 0, val);
}

void
GtFilteredLogModel::filterDebugLevel(bool val)
{
    setFilter(1 << 1, val);
}

void
GtFilteredLogModel::filterInfoLevel(bool val)
{
    setFilter(1 << 2, val);
}

void
GtFilteredLogModel::filterWarningLevel(bool val)
{
    setFilter(1 << 3, val);
}

void
GtFilteredLogModel::filterErrorLevel(bool val)
{
    setFilter(1 << 4, val);
}

void
GtFilteredLogModel::filterFatalLevel(bool val)
{
    setFilter(1 << 5, val);
}

void
GtFilteredLogModel::filterData(const QString& val)
{
    if (m_filterModel)
    {
        m_filterModel->setFilterText(val);
    }
}

void
GtFilteredLogModel::setFilter(int levelBit, bool enabled)
{
    if (enabled)
    {
        m_filter |= levelBit;
        int levelValue;
        switch (levelBit)
        {
        case 1 << 0: levelValue = gt::log::TraceLevel; break;
        case 1 << 1: levelValue = gt::log::DebugLevel; break;
        case 1 << 2: levelValue = gt::log::InfoLevel; break;
        case 1 << 3: levelValue = gt::log::WarningLevel; break;
        case 1 << 4: levelValue = gt::log::ErrorLevel; break;
        case 1 << 5: levelValue = gt::log::FatalLevel; break;
        default: return;
        }
        m_activeLevels.insert(levelValue);
    }
    else
    {
        m_filter &= ~levelBit;
        int levelValue;
        switch (levelBit)
        {
        case 1 << 0: levelValue = gt::log::TraceLevel; break;
        case 1 << 1: levelValue = gt::log::DebugLevel; break;
        case 1 << 2: levelValue = gt::log::InfoLevel; break;
        case 1 << 3: levelValue = gt::log::WarningLevel; break;
        case 1 << 4: levelValue = gt::log::ErrorLevel; break;
        case 1 << 5: levelValue = gt::log::FatalLevel; break;
        default: return;
        }
        m_activeLevels.remove(levelValue);
    }

    updateFilterModel();
}

void
GtFilteredLogModel::updateFilterModel()
{
    if (m_filterModel)
    {
        m_filterModel->setLevelFilter(m_activeLevels);
    }
}

bool
GtFilteredLogModel::filterAcceptsRow(int srcRow,
                                     const QModelIndex& srcParent) const
{
    return true;
}

