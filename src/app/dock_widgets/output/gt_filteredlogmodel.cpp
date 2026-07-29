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
    m_filterState.initAllLevels();
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
GtFilteredLogModel::setLevelFilter(gt::LogLevelFlags levels)
{
    if (m_filterState.levels == levels) return;

    m_filterState.levels = levels;

    invalidateFilter();
    emit levelFilterChanged(gt::gtLogLevelSetByFlags(levels));
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

gt::LogLevelFlags
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
        QString storageValue = sourceModel()->data(index).toString();

        if (storageValue.isEmpty())
        {
            if (!seenStorage.contains(""))
            {
                result << gt::EmptyIDText;
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
        case static_cast<int>(gt::LogColumn::LevelColumn):
        if (!m_filterState.levelsEmpty())
            {
                return !m_filterState.allLevelActive();
            }
            return false;

        case static_cast<int>(gt::LogColumn::TimeColumn):
            return false;

        case static_cast<int>(gt::LogColumn::CategoryColumn):
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

        case static_cast<int>(gt::LogColumn::MessageColumn):
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

    if (!m_filterState.levelsEmpty())
    {
        m_filterState.clearLevels();
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
    setFilterLevel(gt::TraceLevelFlag, val);
}

void
GtFilteredLogModel::filterDebugLevel(bool val)
{
    setFilterLevel(gt::DebugLevelFlag, val);
}

void
GtFilteredLogModel::filterInfoLevel(bool val)
{
    setFilterLevel(gt::InfoLevelFlag, val);
}

void
GtFilteredLogModel::filterWarningLevel(bool val)
{
    setFilterLevel(gt::WarningLevelFlag, val);
}

void
GtFilteredLogModel::filterErrorLevel(bool val)
{
    setFilterLevel(gt::ErrorLevelFlag, val);
}

void
GtFilteredLogModel::filterFatalLevel(bool val)
{
    setFilterLevel(gt::FatalLevelFlag, val);
}

void
GtFilteredLogModel::filterData(const QString& val)
{
    setFilterText(val);
}

void
GtFilteredLogModel::setFilterLevel(gt::LogLevelFlag levelFlag, bool enabled)
{
    gt::LogLevelFlags flags = m_filterState.levels;
    flags.setFlag(levelFlag, enabled);
    setLevelFilter(flags);
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
        static_cast<int>(gt::LogColumn::MessageColumn), source_parent);
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
    if (m_filterState.levelsEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 
        static_cast<int>(gt::LogColumn::LevelColumn), source_parent);
    const int level = sourceModel()->data(index, Qt::UserRole).toInt();

    if (level == gt::log::TraceLevel) return m_filterState.gtLogLevelActive(gt::log::TraceLevel);
    if (level == gt::log::DebugLevel) return m_filterState.gtLogLevelActive(gt::log::DebugLevel);
    if (level == gt::log::InfoLevel) return m_filterState.gtLogLevelActive(gt::log::InfoLevel);
    if (level == gt::log::WarningLevel) return m_filterState.gtLogLevelActive(gt::log::WarningLevel);
    if (level == gt::log::ErrorLevel) return m_filterState.gtLogLevelActive(gt::log::ErrorLevel);
    if (level == gt::log::FatalLevel) return m_filterState.gtLogLevelActive(gt::log::FatalLevel);

    return false;
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
        static_cast<int>(gt::LogColumn::CategoryColumn), source_parent);
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
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
            static_cast<int>(gt::LogColumn::CategoryColumn));
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

bool
GtFilteredLogModel::FilterState::allLevelActive() const
{
    return levels == gt::AllLogLevels;
}

void
GtFilteredLogModel::FilterState::initAllLevels()
{
    levels = gt::AllLogLevels;
}

bool
GtFilteredLogModel::FilterState::levelsEmpty() const
{
    return levels.testFlag(gt::NoLogLevelFlag);
}

void
GtFilteredLogModel::FilterState::clearLevels()
{
    levels.setFlag(gt::NoLogLevelFlag);
}

bool
GtFilteredLogModel::FilterState::gtLogLevelActive(gt::log::Level l) const
{
    switch (l) {
    case gt::log::TraceLevel:
        return levels.testFlag(gt::TraceLevelFlag);
        break;
    case gt::log::DebugLevel:
        return levels.testFlag(gt::DebugLevelFlag);
        break;
    case gt::log::InfoLevel:
        return levels.testFlag(gt::InfoLevelFlag);
        break;
    case gt::log::WarningLevel:
        return levels.testFlag(gt::WarningLevelFlag);
        break;
    case gt::log::ErrorLevel:
        return levels.testFlag(gt::ErrorLevelFlag);
        break;
    case gt::log::FatalLevel:
        return levels.testFlag(gt::FatalLevelFlag);
        break;
    default:
        break;
    }

    return false;
}

QSet<int>
GtFilteredLogModel::FilterState::levelSet() const
{
    return gt::gtLogLevelSetByFlags(levels);
}

void
GtFilteredLogModel::FilterState::setBySet(QSet<int> newSet)
{
    levels = gt::levelFlagsFromSet(newSet);
}

gt::LogLevelFlags
gt::levelFlagsFromSet(QSet<int> const& gtLogLevelSet)
{
    gt::LogLevelFlags levels;
    levels.setFlag(gt::TraceLevelFlag, gtLogLevelSet.contains(gt::log::TraceLevel));
    levels.setFlag(gt::DebugLevelFlag, gtLogLevelSet.contains(gt::log::DebugLevel));
    levels.setFlag(gt::InfoLevelFlag, gtLogLevelSet.contains(gt::log::InfoLevel));
    levels.setFlag(gt::WarningLevelFlag, gtLogLevelSet.contains(gt::log::WarningLevel));
    levels.setFlag(gt::ErrorLevelFlag, gtLogLevelSet.contains(gt::log::ErrorLevel));
    levels.setFlag(gt::FatalLevelFlag, gtLogLevelSet.contains(gt::log::FatalLevel));
    return levels;
}

QSet<int>
gt::gtLogLevelSetByFlags(LogLevelFlags flags)
{
    QSet<int> retVal;

    if (flags.testFlag(gt::TraceLevelFlag)) retVal.insert(gt::log::TraceLevel);
    if (flags.testFlag(gt::DebugLevelFlag)) retVal.insert(gt::log::DebugLevel);
    if (flags.testFlag(gt::InfoLevelFlag)) retVal.insert(gt::log::InfoLevel);
    if (flags.testFlag(gt::WarningLevelFlag)) retVal.insert(gt::log::WarningLevel);
    if (flags.testFlag(gt::ErrorLevelFlag)) retVal.insert(gt::log::ErrorLevel);
    if (flags.testFlag(gt::FatalLevelFlag)) retVal.insert(gt::log::FatalLevel);

    return retVal;
}
