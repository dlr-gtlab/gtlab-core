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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExp>
#else
#include <QRegularExpression>
#endif

GtFilteredLogModel::GtFilteredLogModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    m_filterState.levels = gt::allLevels;
}

void
GtFilteredLogModel::setFilterText(const QString& text)
{
    if (m_filterState.text == text) return;

    m_filterState.text = text;
    invalidateFilter();
    emit filterTextChanged(text);
}

void
GtFilteredLogModel::setLevelFilter(const QSet<int>& levels)
{
    if (m_filterState.levels == levels) return;

    m_filterState.levels = levels;
    invalidateFilter();
    emit levelFilterChanged(levels);
}

void
GtFilteredLogModel::setCategoryFilter(const QSet<QString>& categories)
{
    if (m_filterState.categories == categories) return;

    m_filterState.categories = categories;
    invalidateFilter();
    emit categoryFilterChanged(categories);
}

void
GtFilteredLogModel::setDeactivatedCategories(const QSet<QString>& categories)
{
    if (m_filterState.deactivatedCategories == categories) return;

    m_filterState.deactivatedCategories = categories;
    invalidateFilter();
}

QSet<int>
GtFilteredLogModel::levelFilter() const
{
    return m_filterState.levels;
}

QSet<QString>
GtFilteredLogModel::categoryFilter() const
{
    return m_filterState.categories;
}

QString
GtFilteredLogModel::filterText() const
{
    return m_filterState.text;
}

QStringList
GtFilteredLogModel::availableCategories() const
{
    if (!sourceModel()) return {};

    QSet<QString> categories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        const QString category = sourceModel()->data(index).toString();

        categories.insert(category);
    }

    return QStringList(categories.values());
}

QStringList
GtFilteredLogModel::availableCategoriesWithStorage() const
{
    if (!sourceModel()) return {};

    QStringList result;
    QSet<QString> seenStorage;

    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        QString storageValue = sourceModel()->data(index).toString();

        if (storageValue.isEmpty())
        {
            if (!seenStorage.contains(""))
            {
                result << gt::emptyIDText;
                seenStorage.insert("");
            }
        }
        else
        {
            if (!seenStorage.contains(storageValue))
            {
                result << storageValue;
                seenStorage.insert(storageValue);
            }
        }
    }

    return result;
}

bool
GtFilteredLogModel::hasActiveFiltersForColumn(int column) const
{
    switch (column)
    {
        case static_cast<int>(gt::LogColumn::Level):
            if (!m_filterState.levels.isEmpty())
            {
                QSet<int> allLevels = gt::allLevels;

                return m_filterState.levels != allLevels;
            }
            return false;

        case static_cast<int>(gt::LogColumn::Time):
            return false;

        case static_cast<int>(gt::LogColumn::Category):
            if (!m_filterState.categories.isEmpty())
            {
                QStringList allCategories = availableCategories();

                if (allCategories.isEmpty()) return false;

                return std::any_of(allCategories.begin(), allCategories.end(),
                    [this](const auto& category)
                    {
                        return !m_filterState.categories.contains(category);
                    });
            }

            if (!m_filterState.deactivatedCategories.isEmpty()) return true;

            return false;

        case static_cast<int>(gt::LogColumn::Message):
            return !m_filterState.text.isEmpty();

        default:
            return false;
    }
}

void
GtFilteredLogModel::clearFilters()
{
    bool changed = false;

    if (!m_filterState.text.isEmpty())
    {
        m_filterState.text.clear();
        changed = true;
    }

    if (!m_filterState.levels.isEmpty())
    {
        m_filterState.levels.clear();
        changed = true;
    }

    if (!m_filterState.categories.isEmpty())
    {
        m_filterState.categories.clear();
        changed = true;
    }

    if (!m_filterState.deactivatedCategories.isEmpty())
    {
        m_filterState.deactivatedCategories.clear();
        changed = true;
    }

    if (changed)
    {
        invalidateFilter();
    }
}

void
GtFilteredLogModel::filterTraceLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Trace), val);
}

void
GtFilteredLogModel::filterDebugLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Debug), val);
}

void
GtFilteredLogModel::filterInfoLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Info), val);
}

void
GtFilteredLogModel::filterWarningLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Warning), val);
}

void
GtFilteredLogModel::filterErrorLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Error), val);
}

void
GtFilteredLogModel::filterFatalLevel(bool val)
{
    setFilterLevel(static_cast<int>(gt::LogFilterLevel::Fatal), val);
}

void
GtFilteredLogModel::filterData(const QString& val)
{
    setFilterText(val);
}

void
GtFilteredLogModel::setFilterLevel(int levelBit, bool enabled)
{
    QSet<int> levels = m_filterState.levels;

    if (enabled)
    {
        levels.insert(levelBit);
    }
    else
    {
        levels.remove(levelBit);
    }

    setLevelFilter(levels);
}

bool
GtFilteredLogModel::filterAcceptsRow(int source_row,
                                     const QModelIndex& source_parent) const
{
    return matchesTextFilter(source_row, source_parent) &&
           matchesLevelFilter(source_row, source_parent) &&
           matchesCategoryFilter(source_row, source_parent);
}

bool
GtFilteredLogModel::matchesTextFilter(int source_row,
                                      const QModelIndex& source_parent) const
{
    if (m_filterState.text.isEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 
        static_cast<int>(gt::LogColumn::Message), source_parent);
    const QString message = sourceModel()->data(index).toString();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp regexp(m_filterState.text, Qt::CaseInsensitive, QRegExp::Wildcard);
    return regexp.indexIn(message) >= 0;
#else
    QRegularExpression regexp(m_filterState.text, 
        QRegularExpression::CaseInsensitiveOption);
    return regexp.match(message).hasMatch();
#endif
}

bool
GtFilteredLogModel::matchesLevelFilter(int source_row,
                                       const QModelIndex& source_parent) const
{
    if (m_filterState.levels.isEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 
        static_cast<int>(gt::LogColumn::Level), source_parent);
    const int level = sourceModel()->data(index, Qt::UserRole).toInt();

    return m_filterState.levels.contains(level);
}

bool
GtFilteredLogModel::matchesCategoryFilter(int source_row,
                                          const QModelIndex& source_parent) const
{
    if (m_filterState.categories.isEmpty() && 
        !m_filterState.deactivatedCategories.isEmpty())
    {
        return false;
    }

    if (m_filterState.categories.isEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 
        static_cast<int>(gt::LogColumn::Category), source_parent);
    const QString category = sourceModel()->data(index, Qt::DisplayRole).toString();

    if (m_filterState.deactivatedCategories.contains(category)) return false;

    return m_filterState.categories.contains(category);
}

void
GtFilteredLogModel::updateCategoryFilter()
{
    if (!sourceModel()) return;

    QSet<QString> currentCategories = m_filterState.categories;

    QSet<QString> availableCategories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        const QString category = sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    if (!availableCategories.isEmpty())
    {
        QSet<QString> updatedCategories = currentCategories;
        for (const QString& cat : availableCategories)
        {
            if (!currentCategories.contains(cat) && 
                !m_savedDeactivatedCategories.contains(cat))
            {
                updatedCategories.insert(cat);
            }
        }

        if (updatedCategories != currentCategories)
        {
            setCategoryFilter(updatedCategories);
        }
    }
}

void
GtFilteredLogModel::saveAndPreserveDeactivatedCategories(
    const QSet<QString>& currentActivated)
{
    if (!sourceModel()) return;

    QSet<QString> availableCategories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        const QString category = sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    QSet<QString> activated = currentActivated.isEmpty()
        ? m_filterState.categories
        : currentActivated;

    if (!availableCategories.isEmpty())
    {
        m_savedDeactivatedCategories.clear();
        for (const QString& cat : availableCategories)
        {
            if (!activated.contains(cat))
            {
                m_savedDeactivatedCategories.insert(cat);
            }
        }

        setDeactivatedCategories(m_savedDeactivatedCategories);
    }
}

QSet<QString>
GtFilteredLogModel::savedDeactivatedCategories() const
{
    return m_savedDeactivatedCategories;
}

void
GtFilteredLogModel::setCategoryFilterWithSave(const QSet<QString>& categories)
{
    if (!sourceModel()) return;

    QSet<QString> availableCategories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        const QString category = sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    m_savedDeactivatedCategories.clear();
    for (const QString& cat : availableCategories)
    {
        if (!categories.contains(cat))
        {
            m_savedDeactivatedCategories.insert(cat);
        }
    }

    setCategoryFilter(categories);
    setDeactivatedCategories(m_savedDeactivatedCategories);
}

void
GtFilteredLogModel::resetCategoryFilter()
{
    if (!sourceModel()) return;

    QSet<QString> currentCategories = m_filterState.categories;

    QSet<QString> availableCategories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 
            static_cast<int>(gt::LogColumn::Category));
        const QString category = sourceModel()->data(index).toString();

        availableCategories.insert(category);
    }

    if (availableCategories.isEmpty())
    {
        setDeactivatedCategories(m_savedDeactivatedCategories);
        if (!currentCategories.isEmpty())
        {
            setCategoryFilter(currentCategories);
        }
        return;
    }

    setCategoryFilter(QSet<QString>());
    setDeactivatedCategories(m_savedDeactivatedCategories);

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
        setCategoryFilter(updatedCategories);
    }
}

void
GtFilteredLogModel::setSourceModel(QAbstractItemModel* model)
{
    QSortFilterProxyModel::setSourceModel(model);
    updateCategoryFilter();
}
