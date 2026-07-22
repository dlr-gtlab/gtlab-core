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
#include "gt_logfilterproxymodel.h"

#include <gt_loglevel.h>

GtFilteredLogModel::GtFilteredLogModel(gt::LogFilterProxyModel* filterModel,
                                        QObject* parent) :
    QSortFilterProxyModel(parent),
    m_filterModel(filterModel),
    m_filter(0x3F)
{
    setFilterKeyColumn(-1);
    m_activeLevels = {gt::log::TraceLevel, gt::log::DebugLevel, 
                      gt::log::InfoLevel, gt::log::WarningLevel,
                      gt::log::ErrorLevel, gt::log::FatalLevel};

    if (m_filterModel && m_filterModel->sourceModel())
    {
        updateCategoryFilter();
    }

    // Initialize saved deactivated categories set
    m_savedDeactivatedCategories.clear();
}

void
GtFilteredLogModel::setSourceModel(QAbstractItemModel* model)
{
    QSortFilterProxyModel::setSourceModel(model);
    updateCategoryFilter();
}

gt::LogFilterProxyModel*
GtFilteredLogModel::filterModel() const
{
    return m_filterModel;
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

void
GtFilteredLogModel::updateCategoryFilter()
{
    if (!m_filterModel || !m_filterModel->sourceModel()) {
        return;
    }

    QSet<QString> currentCategories = m_filterModel->categoryFilter();

    QSet<QString> availableCategories;
    const int rowCount = m_filterModel->sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = m_filterModel->sourceModel()->index(row, 2);
        const QString category = m_filterModel->sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    if (!availableCategories.isEmpty())
    {
        QSet<QString> updatedCategories = currentCategories;
        for (const QString& cat : availableCategories)
        {
            if (!currentCategories.contains(cat) && !m_savedDeactivatedCategories.contains(cat))
            {
                updatedCategories.insert(cat);
            }
        }

        if (updatedCategories != currentCategories)
        {
            m_filterModel->setCategoryFilter(updatedCategories);
        }
    }
}

void
GtFilteredLogModel::saveAndPreserveDeactivatedCategories(
    const QSet<QString>& currentActivated)
{
    if (!m_filterModel || !m_filterModel->sourceModel()) return;

    QSet<QString> availableCategories;
    const int rowCount = m_filterModel->sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = m_filterModel->sourceModel()->index(row, 2);
        const QString category = m_filterModel->sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    // Use provided currentActivated or get from filter model
    QSet<QString> activated = currentActivated.isEmpty()
                                  ? m_filterModel->categoryFilter()
                                  : currentActivated;

    // Only update if we have available categories
    if (!availableCategories.isEmpty())
    {
        // Save deactivated categories (available but not activated)
        m_savedDeactivatedCategories.clear();
        for (const QString& cat : availableCategories)
        {
            if (!activated.contains(cat))
            {
                m_savedDeactivatedCategories.insert(cat);
            }
        }

        // Set deactivated categories in filter model
        m_filterModel->setDeactivatedCategories(m_savedDeactivatedCategories);
    }
}

void
GtFilteredLogModel::setCategoryFilterWithSave(const QSet<QString>& categories)
{
    // Update saved deactivated categories
    if (!m_filterModel || !m_filterModel->sourceModel()) return;

    QSet<QString> availableCategories;
    const int rowCount = m_filterModel->sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = m_filterModel->sourceModel()->index(row, 2);
        const QString category = m_filterModel->sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    // Calculate deactivated categories (available but not in new categories)
    m_savedDeactivatedCategories.clear();
    for (const QString& cat : availableCategories)
    {
        if (!categories.contains(cat))
        {
            m_savedDeactivatedCategories.insert(cat);
        }
    }

    // Set both activated and deactivated categories
    m_filterModel->setCategoryFilter(categories);
    m_filterModel->setDeactivatedCategories(m_savedDeactivatedCategories);
}

QSet<QString>
GtFilteredLogModel::savedDeactivatedCategories() const
{
    return m_savedDeactivatedCategories;
}

void
GtFilteredLogModel::resetCategoryFilter()
{
    if (!m_filterModel || !m_filterModel->sourceModel()) return;

    QSet<QString> currentCategories = m_filterModel->categoryFilter();

    QSet<QString> availableCategories;
    const int rowCount = m_filterModel->sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = m_filterModel->sourceModel()->index(row, 2);
        const QString category = m_filterModel->sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    // If no available categories, keep current filter unchanged
    if (availableCategories.isEmpty())
    {
        m_filterModel->setDeactivatedCategories(m_savedDeactivatedCategories);
        if (!currentCategories.isEmpty())
        {
            m_filterModel->setCategoryFilter(currentCategories);
        }
        return;
    }

    // Clear activated categories and set deactivated
    m_filterModel->setCategoryFilter(QSet<QString>());
    m_filterModel->setDeactivatedCategories(m_savedDeactivatedCategories);

    // Add available categories that are not in saved deactivated
    QSet<QString> updatedCategories;
    for (const QString& cat : availableCategories)
    {
        if (!m_savedDeactivatedCategories.contains(cat))
        {
            updatedCategories.insert(cat);
        }
    }

    if (!updatedCategories.isEmpty())
    {
        m_filterModel->setCategoryFilter(updatedCategories);
    }
}

bool
GtFilteredLogModel::filterAcceptsRow(int srcRow,
                                     const QModelIndex& srcParent) const
{
    return true;
}

