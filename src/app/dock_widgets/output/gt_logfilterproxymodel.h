/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef LOGFILTERPROXYMODEL_H
#define LOGFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QSet>
#include <QStringList>
#include <QPair>

namespace gt
{
/**
 * @brief The LogFilterProxyModel class
 * 
 * Implements a comprehensive filter concept for log data:
 * - Text search (filtered on message column only)
 * - Multi-select level filter (Trace, Debug, Info, Warning, Error, Fatal)
 * - Multi-select category filter (automatically extracted from model)
 * - All filters combined with AND logic
 * - Within MultiSelect filters: OR logic
 * - Designed to be extensible for future filters (time range, regex, etc.)
 */
class LogFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

signals:
    void levelFilterChanged(const QSet<int>& levels);
    void categoryFilterChanged(const QSet<QString>& categories);
    void filterTextChanged(const QString& text);

public:
    explicit LogFilterProxyModel(QObject* parent = nullptr);

    /// Set text filter (applied only to message column)
    void setFilterText(const QString& text);

    /// Set level filter (multi-select)
    void setLevelFilter(const QSet<int>& levels);

    /// Set category filter (multi-select)
    void setCategoryFilter(const QSet<QString>& categories);

    /// Set deactivated categories (hidden from view)
    void setDeactivatedCategories(const QSet<QString>& categories);

    /// Get all available categories from the source model
    QStringList availableCategories() const;

    /// Get all available categories with storage mapping (display, storage)
    QList<QPair<QString, QString>> availableCategoriesWithStorage() const;

    /// Get all available logging levels
    QStringList availableLevels() const;

    /// Clear all filters
    void clearFilters();

    /// Get current level filter
    QSet<int> levelFilter() const;

    /// Get current category filter
    QSet<QString> categoryFilter() const;

    /// Get current text filter
    QString filterText() const;

    /// Check if any filters are active
    bool hasActiveFilters() const;

    /// Check if specific column has active filters
    /// @param column 0=ID, 1=Level, 2=Category, 3=Message
    bool hasActiveFiltersForColumn(int column) const;

protected:
    bool filterAcceptsRow(int source_row,
                          const QModelIndex& source_parent) const override;

private:
    struct FilterState
    {
        QString text;
        QSet<int> levels;
        QSet<QString> categories;
        QSet<QString> deactivatedCategories;
    };

    FilterState m_filterState;

    /// Check if row matches text filter (message column only)
    bool matchesTextFilter(int source_row,
                           const QModelIndex& source_parent) const;

    /// Check if row matches level filter
    bool matchesLevelFilter(int source_row,
                            const QModelIndex& source_parent) const;

    /// Check if row matches category filter
    bool matchesCategoryFilter(int source_row,
                               const QModelIndex& source_parent) const;
};
} // namespace gt
#endif // LOGFILTERPROXYMODEL_H
