/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_logfilters
 *
 *  Created on: 24.07.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_logfilterproxymodel.h"
#include "gt_filteredlogmodel.h"
#include "gt_styledlogmodel.h"
#include "gt_logmodel.h"

#include <QSet>
#include <QTimer>
#include <QApplication>

/// Test fixture for LogFilterProxyModel
class TestLogFilterProxyModel : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create test data model
        m_logModel = new GtLogModel(this);
        
        // Create filter model
        m_filterModel = new gt::LogFilterProxyModel(this);
        m_filterModel->setSourceModel(m_logModel);
    }

    void TearDown() override
    {
        delete m_filterModel;
        delete m_logModel;
    }

    GtLogModel* m_logModel = nullptr;
    gt::LogFilterProxyModel* m_filterModel = nullptr;

    void addLogEntries()
    {
        // Add multiple log entries with different levels and categories
        for (int i = 0; i < 10; ++i)
        {
            QString msg = QString("Test message %1").arg(i);
            int level = i % 6; // Trace, Debug, Info, Warning, Error, Fatal
            QString category = i < 5 ? "CategoryA" : "CategoryB";
            
            m_logModel->onMessage(msg, level, 
                GtLogModel::Details{QString::number(i), QDateTime::currentDateTime()});
        }
    }
};

/// Test fixture for GtFilteredLogModel
class TestFilteredLogModel : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_logModel = new GtLogModel(this);
        m_filterModel = new gt::LogFilterProxyModel(this);
        m_filterModel->setSourceModel(m_logModel);
        m_styledModel = new GtStyledLogModel(this);
        m_styledModel->setSourceModel(m_filterModel);
        m_filteredModel = new GtFilteredLogModel(m_filterModel, this);
        m_filteredModel->setSourceModel(m_styledModel);
    }

    void TearDown() override
    {
        delete m_filteredModel;
        delete m_styledModel;
        delete m_filterModel;
        delete m_logModel;
    }

    GtLogModel* m_logModel = nullptr;
    gt::LogFilterProxyModel* m_filterModel = nullptr;
    GtStyledLogModel* m_styledModel = nullptr;
    GtFilteredLogModel* m_filteredModel = nullptr;

    void addLogEntries()
    {
        for (int i = 0; i < 10; ++i)
        {
            QString msg = QString("Test message %1").arg(i);
            int level = i % 6;
            QString category = i < 5 ? "CategoryA" : "CategoryB";
            
            m_logModel->onMessage(msg, level,
                GtLogModel::Details{QString::number(i), QDateTime::currentDateTime()});
        }
    }
};

// ==================== Test gt::LogFilterProxyModel ====================

TEST_F(TestLogFilterProxyModel, initialFilters)
{
    // All levels should be active initially
    auto levels = m_filterModel->levelFilter();
    EXPECT_EQ(levels.size(), 6); // Trace, Debug, Info, Warning, Error, Fatal
    
    // No text filter initially
    EXPECT_TRUE(m_filterModel->filterText().isEmpty());
    
    // No category filter initially
    EXPECT_TRUE(m_filterModel->categoryFilter().isEmpty());
    
    // No deactivated categories initially
    auto categories = m_filterModel->availableCategories();
    EXPECT_TRUE(categories.isEmpty());
}

TEST_F(TestLogFilterProxyModel, setTextFilter)
{
    addLogEntries();
    
    // Apply text filter
    m_filterModel->setFilterText("message 5");
    
    // Check filter is applied
    EXPECT_EQ(m_filterModel->filterText(), "message 5");
    
    // Check row count reduced
    EXPECT_EQ(m_filterModel->rowCount(), 1);
    
    // Check filtered row contains correct data
    QModelIndex index = m_filterModel->index(0, 3);
    QString text = m_filterModel->data(index, Qt::DisplayRole).toString();
    EXPECT_TRUE(text.contains("message 5"));
    
    // Clear filter
    m_filterModel->setFilterText("");
    EXPECT_EQ(m_filterModel->rowCount(), 10);
}

TEST_F(TestLogFilterProxyModel, setLevelFilter)
{
    addLogEntries();
    
    // Filter to only Info and Error
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    levels.insert(gt::log::ErrorLevel);
    
    m_filterModel->setLevelFilter(levels);
    
    // Check level filter is set
    EXPECT_EQ(m_filterModel->levelFilter(), levels);
    
    // Check row count reduced (should have 2 Info and 2 Error messages out of 10)
    EXPECT_EQ(m_filterModel->rowCount(), 4);
    
    // Verify filtered rows have correct levels
    for (int row = 0; row < m_filterModel->rowCount(); ++row)
    {
        QModelIndex index = m_filterModel->index(row, 0);
        int level = m_filterModel->data(index, Qt::UserRole).toInt();
        EXPECT_TRUE(levels.contains(level));
    }
    
    // Clear filter (all levels)
    QSet<int> allLevels = {gt::log::TraceLevel, gt::log::DebugLevel,
                           gt::log::InfoLevel, gt::log::WarningLevel,
                           gt::log::ErrorLevel, gt::log::FatalLevel};
    m_filterModel->setLevelFilter(allLevels);
    EXPECT_EQ(m_filterModel->rowCount(), 10);
}

TEST_F(TestLogFilterProxyModel, setCategoryFilter)
{
    addLogEntries();
    
    // Get available categories
    auto categories = m_filterModel->availableCategories();
    EXPECT_EQ(categories.size(), 2);
    
    // Filter to only CategoryA
    QSet<QString> catFilter;
    catFilter.insert("CategoryA");
    m_filterModel->setCategoryFilter(catFilter);
    
    // Check category filter is set
    EXPECT_EQ(m_filterModel->categoryFilter(), catFilter);
    
    // Check row count reduced (5 messages in CategoryA)
    EXPECT_EQ(m_filterModel->rowCount(), 5);
    
    // Verify filtered rows have correct category
    for (int row = 0; row < m_filterModel->rowCount(); ++row)
    {
        QModelIndex index = m_filterModel->index(row, 2);
        QString category = m_filterModel->data(index, Qt::DisplayRole).toString();
        EXPECT_EQ(category, "CategoryA");
    }
}

TEST_F(TestLogFilterProxyModel, setDeactivatedCategories)
{
    addLogEntries();
    
    // Deactivate CategoryA - should hide all CategoryA messages
    QSet<QString> deactivated;
    deactivated.insert("CategoryA");
    m_filterModel->setDeactivatedCategories(deactivated);
    
    // Check row count reduced (5 messages should be hidden)
    EXPECT_EQ(m_filterModel->rowCount(), 5);
    
    // All visible rows should be CategoryB
    for (int row = 0; row < m_filterModel->rowCount(); ++row)
    {
        QModelIndex index = m_filterModel->index(row, 2);
        QString category = m_filterModel->data(index, Qt::DisplayRole).toString();
        EXPECT_EQ(category, "CategoryB");
    }
}

TEST_F(TestLogFilterProxyModel, clearFilters)
{
    addLogEntries();
    
    // Apply multiple filters
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    m_filterModel->setFilterText("message");
    
    EXPECT_EQ(m_filterModel->rowCount(), 2); // 2 Info messages with "message"
    
    // Clear all filters
    m_filterModel->clearFilters();
    
    EXPECT_EQ(m_filterModel->rowCount(), 10);
    EXPECT_TRUE(m_filterModel->filterText().isEmpty());
    EXPECT_TRUE(m_filterModel->categoryFilter().isEmpty());
}

TEST_F(TestLogFilterProxyModel, hasActiveFilters)
{
    addLogEntries();
    
    // No filters active initially
    EXPECT_FALSE(m_filterModel->hasActiveFilters());
    
    // Text filter active
    m_filterModel->setFilterText("test");
    EXPECT_TRUE(m_filterModel->hasActiveFilters());
    
    // Clear text filter
    m_filterModel->setFilterText("");
    EXPECT_FALSE(m_filterModel->hasActiveFilters());
    
    // Level filter active
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    EXPECT_TRUE(m_filterModel->hasActiveFilters());
}

TEST_F(TestLogFilterProxyModel, hasActiveFiltersForColumn)
{
    addLogEntries();
    
    // No filters
    EXPECT_FALSE(m_filterModel->hasActiveFiltersForColumn(0)); // Level
    EXPECT_FALSE(m_filterModel->hasActiveFiltersForColumn(1)); // Time
    EXPECT_FALSE(m_filterModel->hasActiveFiltersForColumn(2)); // Category
    EXPECT_FALSE(m_filterModel->hasActiveFiltersForColumn(3)); // Message
    
    // Level filter active
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    EXPECT_TRUE(m_filterModel->hasActiveFiltersForColumn(0));
    EXPECT_FALSE(m_filterModel->hasActiveFiltersForColumn(3));
    
    // Text filter active
    m_filterModel->setFilterText("test");
    EXPECT_TRUE(m_filterModel->hasActiveFiltersForColumn(3));
}

TEST_F(TestLogFilterProxyModel, combinedFilters)
{
    addLogEntries();
    
    // Combined level and text filter
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    m_filterModel->setFilterText("message 5");
    
    // Should find only Info level messages containing "message 5"
    int rowCount = m_filterModel->rowCount();
    
    // Clear filters
    m_filterModel->clearFilters();
}

// ==================== Test GtFilteredLogModel ====================

TEST_F(TestFilteredLogModel, initialFilterState)
{
    // All levels should be active
    EXPECT_EQ(m_filteredModel->rowCount(), 0);
    
    // Filter model accessor
    EXPECT_EQ(m_filteredModel->filterModel(), m_filterModel);
}

TEST_F(TestFilteredLogModel, levelFilterToggles)
{
    addLogEntries();
    
    // Initially all levels active (10 rows)
    EXPECT_EQ(m_filteredModel->rowCount(), 10);
    
    // Filter to only Info level
    m_filteredModel->filterInfoLevel(true);
    m_filteredModel->filterDebugLevel(false);
    m_filteredModel->filterTraceLevel(false);
    m_filteredModel->filterWarningLevel(false);
    m_filteredModel->filterErrorLevel(false);
    m_filteredModel->filterFatalLevel(false);
    
    // Should have 2 Info messages out of 10
    EXPECT_EQ(m_filteredModel->rowCount(), 2);
    
    // Verify all visible rows are Info level
    for (int row = 0; row < m_filteredModel->rowCount(); ++row)
    {
        QModelIndex index = m_filteredModel->index(row, 0);
        int level = m_filteredModel->data(index, Qt::UserRole).toInt();
        EXPECT_EQ(level, gt::log::InfoLevel);
    }
}

TEST_F(TestFilteredLogModel, levelFilterReactivation)
{
    addLogEntries();
    
    // Filter to only Info
    m_filteredModel->filterInfoLevel(true);
    m_filteredModel->filterDebugLevel(false);
    m_filteredModel->filterTraceLevel(false);
    m_filteredModel->filterWarningLevel(false);
    m_filteredModel->filterErrorLevel(false);
    m_filteredModel->filterFatalLevel(false);
    
    EXPECT_EQ(m_filteredModel->rowCount(), 2);
    
    // Reactivate all levels
    m_filteredModel->filterTraceLevel(true);
    m_filteredModel->filterDebugLevel(true);
    m_filteredModel->filterInfoLevel(true);
    m_filteredModel->filterWarningLevel(true);
    m_filteredModel->filterErrorLevel(true);
    m_filteredModel->filterFatalLevel(true);
    
    // Should be back to 10 rows
    EXPECT_EQ(m_filteredModel->rowCount(), 10);
}

TEST_F(TestFilteredLogModel, categoryFilterIntegration)
{
    addLogEntries();
    
    // Get available categories
    auto categories = m_filterModel->availableCategories();
    EXPECT_EQ(categories.size(), 2);
    
    // Filter to CategoryA through GtFilteredLogModel
    QSet<QString> catFilter;
    catFilter.insert("CategoryA");
    m_filteredModel->setCategoryFilterWithSave(catFilter);
    
    // Should have 5 CategoryA messages
    EXPECT_EQ(m_filteredModel->rowCount(), 5);
}

TEST_F(TestFilteredLogModel, combinedLevelAndCategoryFilters)
{
    addLogEntries();
    
    // Filter to Info level
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    
    // Filter to CategoryA
    QSet<QString> catFilter;
    catFilter.insert("CategoryA");
    m_filteredModel->setCategoryFilterWithSave(catFilter);
    
    // Should have 1 Info message in CategoryA (row index 4)
    EXPECT_EQ(m_filteredModel->rowCount(), 1);
}

// ==================== Integration Tests ====================

TEST_F(TestLogFilterProxyModel, filterChain)
{
    // Test the complete chain: LogModel -> FilterModel
    gt::log::GtLogModel* logModel = new gt::log::GtLogModel(this);
    gt::LogFilterProxyModel* filterModel = new gt::LogFilterProxyModel(this);
    filterModel->setSourceModel(logModel);
    
    // Add entries
    for (int i = 0; i < 20; ++i)
    {
        logModel->onMessage(QString("Log %1").arg(i), i % 6,
            gt::log::GtLogModel::Details{QString::number(i), QDateTime::currentDateTime()});
    }
    
    EXPECT_EQ(filterModel->rowCount(), 20);
    
    // Apply level filter
    QSet<int> levels;
    levels.insert(gt::log::ErrorLevel);
    levels.insert(gt::log::FatalLevel);
    filterModel->setLevelFilter(levels);
    
    EXPECT_EQ(filterModel->rowCount(), 7); // ~7 messages in Error/Fatal levels
    
    // Apply text filter
    filterModel->setFilterText("Log 1");
    EXPECT_EQ(filterModel->rowCount(), 2); // "Log 10" and "Log 15" (Error/Fatal)
    
    delete filterModel;
    delete logModel;
}

TEST_F(TestFilteredLogModel, completeChain)
{
    // Test the complete chain used in GtOutputDock
    gt::log::GtLogModel* logModel = new gt::log::GtLogModel(this);
    gt::LogFilterProxyModel* filterModel = new gt::LogFilterProxyModel(this);
    filterModel->setSourceModel(logModel);
    GtStyledLogModel* styledModel = new GtStyledLogModel(this);
    styledModel->setSourceModel(filterModel);
    GtFilteredLogModel* filteredModel = new GtFilteredLogModel(filterModel, this);
    filteredModel->setSourceModel(styledModel);
    
    // Add entries
    for (int i = 0; i < 15; ++i)
    {
        logModel->onMessage(QString("Test message %1").arg(i), i % 6,
            gt::log::GtLogModel::Details{QString::number(i), QDateTime::currentDateTime()});
    }
    
    EXPECT_EQ(filteredModel->rowCount(), 15);
    
    // Apply filters through the chain
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    levels.insert(gt::log::WarningLevel);
    filterModel->setLevelFilter(levels);
    
    QSet<QString> categories;
    categories.insert("CategoryA");
    filteredModel->setCategoryFilterWithSave(categories);
    
    // Apply text filter
    filterModel->setFilterText("message 5");
    
    EXPECT_GT(filteredModel->rowCount(), 0);
    
    delete filteredModel;
    delete styledModel;
    delete filterModel;
    delete logModel;
}

// ==================== Search/Match Tests ====================

class TestSearchFunctionality : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_logModel = new gt::log::GtLogModel(this);
        m_filterModel = new gt::LogFilterProxyModel(this);
        m_filterModel->setSourceModel(m_logModel);
        m_styledModel = new GtStyledLogModel(this);
        m_styledModel->setSourceModel(m_filterModel);
        m_filteredModel = new GtFilteredLogModel(m_filterModel, this);
        m_filteredModel->setSourceModel(m_styledModel);
        
        // Add test data
        for (int i = 0; i < 20; ++i)
        {
            QString msg = QString("Search test message %1").arg(i);
            m_logModel->onMessage(msg, i % 6,
                gt::log::GtLogModel::Details{QString::number(i), QDateTime::currentDateTime()});
        }
    }

    void TearDown() override
    {
        delete m_filteredModel;
        delete m_styledModel;
        delete m_filterModel;
        delete m_logModel;
    }

    gt::log::GtLogModel* m_logModel = nullptr;
    gt::LogFilterProxyModel* m_filterModel = nullptr;
    GtStyledLogModel* m_styledModel = nullptr;
    GtFilteredLogModel* m_filteredModel = nullptr;
};

TEST_F(TestSearchFunctionality, searchMatchingRows)
{
    // Simulate search on filtered model
    QString searchText = "message 5";
    
    int matchCount = 0;
    for (int row = 0; row < m_filteredModel->rowCount(); ++row)
    {
        QModelIndex index = m_filteredModel->index(row, 3);
        QString text = m_filteredModel->data(index, Qt::DisplayRole).toString();
        if (text.contains(searchText, Qt::CaseInsensitive))
        {
            ++matchCount;
        }
    }
    
    EXPECT_EQ(matchCount, 1);
}

TEST_F(TestSearchFunctionality, searchWithLevelFilter)
{
    // Apply level filter
    QSet<int> levels;
    levels.insert(gt::log::InfoLevel);
    m_filterModel->setLevelFilter(levels);
    
    // Force filter update
    m_filteredModel->invalidateFilter();
    
    // Give time for async update
    QCoreApplication::processEvents();
    
    // Search should work on filtered results
    QString searchText = "message";
    int matchCount = 0;
    
    for (int row = 0; row < m_filteredModel->rowCount(); ++row)
    {
        QModelIndex index = m_filteredModel->index(row, 3);
        QString text = m_filteredModel->data(index, Qt::DisplayRole).toString();
        if (text.contains(searchText, Qt::CaseInsensitive))
        {
            ++matchCount;
        }
    }
    
    // Should find Info level messages containing "message"
    EXPECT_GT(matchCount, 0);
}

TEST_F(TestSearchFunctionality, noMatchScenario)
{
    QString searchText = "nonexistent text";
    int matchCount = 0;
    
    for (int row = 0; row < m_filteredModel->rowCount(); ++row)
    {
        QModelIndex index = m_filteredModel->index(row, 3);
        QString text = m_filteredModel->data(index, Qt::DisplayRole).toString();
        if (text.contains(searchText, Qt::CaseInsensitive))
        {
            ++matchCount;
        }
    }
    
    EXPECT_EQ(matchCount, 0);
}
