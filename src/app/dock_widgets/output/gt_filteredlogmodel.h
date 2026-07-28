/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFILTEREDLOGMODEL_H
#define GTFILTEREDLOGMODEL_H

#include <QSortFilterProxyModel>
#include <QSet>
#include <QStringList>
#include <QPair>
#include <QMap>

class GtLogDetails;
class GtOutputDock;

/**
 * @brief Log Filter Level Constants (bit-basiert)
 * Sammelt alle Magic Numbers für Filter-Level
 */
enum class LogFilterLevel
{
    Trace   = 1 << 0,  // 0x01 (1)
    Debug   = 1 << 1,  // 0x02 (2)
    Info    = 1 << 2,  // 0x04 (4)
    Warning = 1 << 3,  // 0x08 (8)
    Error   = 1 << 4,  // 0x10 (16)
    Fatal   = 1 << 5   // 0x20 (32)
};

// Alle Level als Bitset (0x3F = 63 = alle 6 Level aktiv)
constexpr int ALL_LOG_LEVELS = 0x3F;

/**
 * @brief Log Column Constants
 * Sammelt alle Magic Numbers für Spalten-Indizes
 */
enum class LogColumn
{
    Level    = 0,
    Time     = 1,
    Category = 2,
    Message  = 3
};

/**
 * @brief The GtFilteredLogModel class
 * 
 * Main filter model that handles all log filtering:
 * - Text search (message column only)
 * - Multi-select level filter (Trace, Debug, Info, Warning, Error, Fatal)
 * - Multi-select category filter (auto-extracted from model)
 * - Deactivated categories (hidden from view)
 * - All filters combined with AND logic
 */
class GtFilteredLogModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    using Details = GtLogDetails;

    // === Constructor ===
    explicit GtFilteredLogModel(QObject* parent = nullptr);

    // === Filter Methods (public API) ===
    void setFilterText(const QString& text);
    void setLevelFilter(const QSet<int>& levels);
    void setCategoryFilter(const QSet<QString>& categories);
    void setDeactivatedCategories(const QSet<QString>& categories);

    // === Getters ===
    QSet<int> levelFilter() const;
    QSet<QString> categoryFilter() const;
    QString filterText() const;
    QStringList availableCategories() const;
    QList<QPair<QString, QString>> availableCategoriesWithStorage() const;

    // === UI Helper Methods ===
    bool hasActiveFilters() const;
    bool hasActiveFiltersForColumn(int column) const;
    void clearFilters();

    // === UI Slots (bestehendes Interface behalten) ===
public slots:
    /**
     * @brief filterTraceLevel
     * @param val
     */
    void filterTraceLevel(bool val);

    /**
     * @brief filterDebugLevel
     * @param val
     */
    void filterDebugLevel(bool val);

    /**
     * @brief filterInfoLevel
     * @param val
     */
    void filterInfoLevel(bool val);

    /**
     * @brief filterWarningLevel
     * @param val
     */
    void filterWarningLevel(bool val);

    /**
     * @brief filterErrorLevel
     * @param val
     */
    void filterErrorLevel(bool val);

    /**
     * @brief filterFatalLevel
     * @param val
     */
    void filterFatalLevel(bool val);

    /**
     * @brief filterData
     * @param val
     */
    void filterData(const QString& val);

    void setCategoryFilterWithSave(const QSet<QString>& categories);
    void resetCategoryFilter();
    void updateCategoryFilter();

signals:
    void levelFilterChanged(const QSet<int>& levels);
    void categoryFilterChanged(const QSet<QString>& categories);
    void filterTextChanged(const QString& text);

    // === Public methods for GtOutputDock ===
public:
    void setSourceModel(QAbstractItemModel* model) override;

protected:
    bool filterAcceptsRow(int srcRow,
                          const QModelIndex& srcParent) const override;

private:
    // === Filter State (zentral) ===
    struct FilterState
    {
        QString text;
        QSet<int> levels;
        QSet<QString> categories;
        QSet<QString> deactivatedCategories;
    };

    FilterState m_filterState;

    // === Helper Methods ===
    bool matchesTextFilter(int source_row,
                          const QModelIndex& source_parent) const;
    bool matchesLevelFilter(int source_row,
                           const QModelIndex& source_parent) const;
    bool matchesCategoryFilter(int source_row,
                              const QModelIndex& source_parent) const;

    void setFilterLevel(int levelBit, bool enabled);

    // === Category Save/Restore (UI-spezifisch) ===
public:
    void saveAndPreserveDeactivatedCategories(
        const QSet<QString>& currentActivated = {});
    QSet<QString> savedDeactivatedCategories() const;

private:
    QSet<QString> m_savedDeactivatedCategories;
};

#endif // GTFILTEREDLOGMODEL_H
