/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_logfilterproxymodel.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QRegExp>
#else
#include <QRegularExpression>
#endif

#include <gt_loglevel.h>

gt::LogFilterProxyModel::LogFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    // Filter only the message column (column 3)
    setFilterKeyColumn(3);
}

void
gt::LogFilterProxyModel::setFilterText(const QString& text)
{
    if (m_filterState.text == text) return;

    m_filterState.text = text;
    emit filterTextChanged(m_filterState.text);
    invalidateFilter();
}

void
gt::LogFilterProxyModel::setLevelFilter(const QSet<int>& levels)
{
    if (m_filterState.levels == levels) return;

    m_filterState.levels = levels;
    emit levelFilterChanged(m_filterState.levels);
    invalidateFilter();
}

void
gt::LogFilterProxyModel::setCategoryFilter(const QSet<QString>& categories)
{
    if (m_filterState.categories == categories) return;

    m_filterState.categories = categories;
    emit categoryFilterChanged(m_filterState.categories);
    invalidateFilter();
}

void
gt::LogFilterProxyModel::setDeactivatedCategories(const QSet<QString>& categories)
{
    if (m_filterState.deactivatedCategories == categories) return;

    m_filterState.deactivatedCategories = categories;
    invalidateFilter();
}

QStringList
gt::LogFilterProxyModel::availableCategories() const
{
    if (!sourceModel()) return {};

    QSet<QString> categories;
    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 2);
        const QString category = sourceModel()->data(index).toString();
        if (!category.isEmpty())
        {
            categories.insert(category);
        }
    }

    return QStringList(categories.values());
}

bool
gt::LogFilterProxyModel::hasActiveFilters() const
{
    if (!m_filterState.text.isEmpty()) return true;

    if (!m_filterState.levels.isEmpty())
    {
        QSet<int> allLevels = {gt::log::TraceLevel, gt::log::DebugLevel,
                               gt::log::InfoLevel, gt::log::WarningLevel,
                               gt::log::ErrorLevel, gt::log::FatalLevel};

        if (m_filterState.levels != allLevels) return true;
    }

    if (!m_filterState.categories.isEmpty()) return true;

    return false;
}

bool
gt::LogFilterProxyModel::hasActiveFiltersForColumn(int column) const
{
    switch (column)
    {
        case 0: // Level column
            if (!m_filterState.levels.isEmpty())
            {
                QSet<int> allLevels = {gt::log::TraceLevel, gt::log::DebugLevel,
                                       gt::log::InfoLevel, gt::log::WarningLevel,
                                       gt::log::ErrorLevel, gt::log::FatalLevel};
                return m_filterState.levels != allLevels;
            }
            return false;
        case 1: // Time stamp column
            return false;
        case 2: // Category column
            if (!m_filterState.categories.isEmpty())
            {
                QStringList allCategories = availableCategories();
                if (allCategories.isEmpty()) {
                    return false;
                }
                return m_filterState.categories.size() != allCategories.size();
            }
            if (!m_filterState.deactivatedCategories.isEmpty())
            {
                return true;
            }
            return false;
        case 3: // Message column
            return !m_filterState.text.isEmpty();
        default:
            return false;
    }
}

QList<QPair<QString, QString>>
gt::LogFilterProxyModel::availableCategoriesWithStorage() const
{
    if (!sourceModel()) return {};

    QList<QPair<QString, QString>> result;
    QSet<QString> seenStorage;

    const int rowCount = sourceModel()->rowCount();

    for (int row = 0; row < rowCount; ++row)
    {
        const QModelIndex index = sourceModel()->index(row, 2);
        QString storageValue = sourceModel()->data(index).toString();

        if (storageValue.isEmpty())
        {
            if (!seenStorage.contains(""))
            {
                result << qMakePair(QString("EmptyID"), QString(""));
                seenStorage.insert("");
            }
        }
        else
        {
            if (!seenStorage.contains(storageValue))
            {
                result << qMakePair(storageValue, storageValue);
                seenStorage.insert(storageValue);
            }
        }
    }

    return result;
}

QStringList
gt::LogFilterProxyModel::availableLevels() const
{
    return {tr("Trace"), tr("Debug"), tr("Info"), tr("Warning"),
            tr("Error"), tr("Fatal")};
}

void
gt::LogFilterProxyModel::clearFilters()
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

QSet<int>
gt::LogFilterProxyModel::levelFilter() const
{
    return m_filterState.levels;
}

QSet<QString>
gt::LogFilterProxyModel::categoryFilter() const
{
    return m_filterState.categories;
}

QString
gt::LogFilterProxyModel::filterText() const
{
    return m_filterState.text;
}

bool
gt::LogFilterProxyModel::filterAcceptsRow(int source_row,
                                          const QModelIndex& source_parent) const
{
    // All filters must match (AND logic)
    return matchesTextFilter(source_row, source_parent) &&
           matchesLevelFilter(source_row, source_parent) &&
           matchesCategoryFilter(source_row, source_parent);
}

bool
gt::LogFilterProxyModel::matchesTextFilter(int source_row,
                                           const QModelIndex& source_parent) const
{
    if (m_filterState.text.isEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 3, source_parent);
    const QString message = sourceModel()->data(index).toString();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp regexp(m_filterState.text, Qt::CaseInsensitive, QRegExp::Wildcard);
    return regexp.indexIn(message) >= 0;
#else
    QRegularExpression regexp(m_filterState.text, QRegularExpression::CaseInsensitiveOption);
    return regexp.match(message).hasMatch();
#endif
}

bool
gt::LogFilterProxyModel::matchesLevelFilter(int source_row,
                                        const QModelIndex& source_parent) const
{
    if (m_filterState.levels.isEmpty()) return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    const int level = sourceModel()->data(index, Qt::UserRole).toInt();

    return m_filterState.levels.contains(level);
}

bool
gt::LogFilterProxyModel::matchesCategoryFilter(int source_row,
                                            const QModelIndex& source_parent) const
{
    // If no activated categories and deactivated categories exist, hide all
    if (m_filterState.categories.isEmpty() && !m_filterState.deactivatedCategories.isEmpty())
    {
        return false;
    }

    // If no activated categories, show all rows
    if (m_filterState.categories.isEmpty()) return true;

    // If category is deactivated, hide it
    const QModelIndex index = sourceModel()->index(source_row, 2, source_parent);
    const QString category = sourceModel()->data(index, Qt::DisplayRole).toString();

    if (m_filterState.deactivatedCategories.contains(category)) return false;

    return m_filterState.categories.contains(category);
}
