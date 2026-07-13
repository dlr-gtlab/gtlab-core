/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "logfilterproxymodel.h"

#include <QRegExp>
#include <algorithm>
#include "gt_logging.h"

LogFilterProxyModel::LogFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel(parent)
{
    // Filter only the message column (column 3)
    setFilterKeyColumn(3);
}

void
LogFilterProxyModel::setFilterText(const QString& text)
{
    if (m_filterState.text == text)
        return;

    m_filterState.text = text;
    invalidateFilter();
}

void
LogFilterProxyModel::setLevelFilter(const QSet<int>& levels)
{
    if (m_filterState.levels == levels)
        return;

    m_filterState.levels = levels;
    emit levelFilterChanged(m_filterState.levels);
    invalidateFilter();
}

void
LogFilterProxyModel::setCategoryFilter(const QSet<QString>& categories)
{
    if (m_filterState.categories == categories)
        return;

    m_filterState.categories = categories;
    emit categoryFilterChanged(m_filterState.categories);
    invalidateFilter();
}

QStringList
LogFilterProxyModel::availableCategories() const
{
    if (!sourceModel())
        return {};

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

QList<QPair<QString, QString>>
LogFilterProxyModel::availableCategoriesWithStorage() const
{
    if (!sourceModel())
        return {};

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
LogFilterProxyModel::availableLevels() const
{
    return {tr("Trace"), tr("Debug"), tr("Info"), tr("Warning"), tr("Error"), tr("Fatal")};
}

void
LogFilterProxyModel::clearFilters()
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

    if (changed)
    {
        invalidateFilter();
    }
}

bool
LogFilterProxyModel::filterAcceptsRow(int source_row,
                                      const QModelIndex& source_parent) const
{
    // All filters must match (AND logic)
    return matchesTextFilter(source_row, source_parent) &&
           matchesLevelFilter(source_row, source_parent) &&
           matchesCategoryFilter(source_row, source_parent);
}

bool
LogFilterProxyModel::matchesTextFilter(int source_row,
                                       const QModelIndex& source_parent) const
{
    if (m_filterState.text.isEmpty())
        return true;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QRegExp regexp(m_filterState.text, Qt::CaseInsensitive, QRegExp::Wildcard);
    const QModelIndex index = sourceModel()->index(source_row, 3, source_parent);
    const QString message = sourceModel()->data(index).toString();
    return regexp.indexIn(message) >= 0;
#else
    QRegularExpression regexp(m_filterState.text, QRegularExpression::CaseInsensitiveOption);
    const QModelIndex index = sourceModel()->index(source_row, 3, source_parent);
    const QString message = sourceModel()->data(index).toString();
    return regexp.match(message).hasMatch();
#endif
}

bool
LogFilterProxyModel::matchesLevelFilter(int source_row,
                                        const QModelIndex& source_parent) const
{
    if (m_filterState.levels.isEmpty())
        return true;

    const QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    const int level = sourceModel()->data(index, Qt::UserRole).toInt();

    return m_filterState.levels.contains(level);
}

bool
LogFilterProxyModel::matchesCategoryFilter(int source_row,
                                           const QModelIndex& source_parent) const
{
    if (m_filterState.categories.isEmpty())
        return true;

    const QModelIndex index = sourceModel()->index(source_row, 2, source_parent);
    const QString category = sourceModel()->data(index, Qt::DisplayRole).toString();

    return m_filterState.categories.contains(category);
}