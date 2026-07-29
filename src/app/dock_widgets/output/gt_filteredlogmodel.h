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
#include "gt_logmodel.h"

class GtLogDetails;
class GtOutputDock;

namespace gt {
/**
 * @brief Log Column Constants
 * Sammelt alle Magic Numbers für Spalten-Indizes
 */
enum class LogColumn
{
    LevelColumn    = 0,
    TimeColumn     = 1,
    CategoryColumn = 2,
    MessageColumn  = 3
};

gt::LogLevelFlags levelFlagsFromSet(const QSet<int>& gtLogLevelSet);

QSet<int> gtLogLevelSetByFlags(gt::LogLevelFlags flags);

const QString EmptyIDText = "EmptyID";

} //namespace gt

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
    void setLevelFilter(gt::LogLevelFlags levels);
    void setCategoryFilter(const QSet<QString>& categories);
    void setDeactivatedCategories(const QSet<QString>& categories);

    // === Getters ===
    gt::LogLevelFlags levelFilter() const;
    QSet<QString> categoryFilter() const;
    QString filterText() const;
    QStringList availableCategories() const;
    QStringList availableCategoriesWithStorage() const;

    // === UI Helper Methods ===
    bool hasActiveFiltersForColumn(int column) const;
    void clearFilters();

    // === Category Save/Restore (UI-specific) ===
    void saveAndPreserveDeactivatedCategories(
        const QSet<QString>& currentActivated = {});
    QSet<QString> savedDeactivatedCategories() const;

    // === Public methods for GtOutputDock ===
    void setSourceModel(QAbstractItemModel* model) override;

public slots:
    // === UI Slots  ===
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

    /**
     * @brief sets the categories and stores manually deactived categoties
     * in a storage set
     * @param categories
     */
    void setCategoryFilterWithSave(const QSet<QString>& categories);

    /**
     * @brief reset the category filter
     */
    void resetCategoryFilter();

    /**
     * @brief update the category filter for the changed data
     */
    void updateCategoryFilter();

signals:
    void levelFilterChanged(const QSet<int>& levels);
    void categoryFilterChanged(const QSet<QString>& categories);
    void filterTextChanged(const QString& text);


protected:
    bool filterAcceptsRow(int srcRow,
                          const QModelIndex& srcParent) const override;

private:
    // === Filter State ===
    struct FilterState
    {
        QString text;

        QSet<QString> categories;
        QSet<QString> deactivatedCategories;

        bool allLevelActive() const;

        void initAllLevels();

        bool levelsEmpty() const;

        void clearLevels();

        bool gtLogLevelActive(gt::log::Level l) const;

        gt::LogLevelFlags levels;
    };

    FilterState m_filterState;

    // === Helper Methods ===
    bool matchesTextFilter(int source_row,
                          const QModelIndex& source_parent) const;
    bool matchesLevelFilter(int source_row,
                           const QModelIndex& source_parent) const;
    bool matchesCategoryFilter(int source_row,
                              const QModelIndex& source_parent) const;

    void setFilterLevel(gt::LogLevelFlag levelFlag, bool enabled);

    QSet<QString> m_savedDeactivatedCategories;
};

#endif // GTFILTEREDLOGMODEL_H
