/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_runnable.cpp
 */

#include "gtest/gtest.h"

#include <QDir>

#include "gt_abstractrunnable.h"
#include "gt_calculator.h"

class TestCalculatorRunnable : public GtAbstractRunnable
{
public:
    int clearTempDirCalls = 0;
    QString clearedPath;

    void run() override
    {
    }

    QDir tempDir() override
    {
        return {};
    }

    bool clearTempDir(const QString& path) override
    {
        ++clearTempDirCalls;
        clearedPath = path;
        return true;
    }

    QString projectPath() override
    {
        return {};
    }
};

class TestableCalculator : public GtCalculator
{
public:
    bool runResult = true;
    bool warning = false;
    bool useTempPath = false;
    int runCalls = 0;

    bool run() override
    {
        ++runCalls;

        if (warning)
        {
            setWarningFlag(true);
        }

        if (useTempPath)
        {
            setTempPath("temp/calculator");
        }

        return runResult;
    }

    void enableTempRemoval(bool value)
    {
        m_deleteTempPath = value;
    }
};

class TestGtRunnable : public ::testing::Test
{
};

TEST_F(TestGtRunnable, skippedCalculatorDoesNotRun)
{
    TestableCalculator calc;

    calc.setSkipped(true);

    EXPECT_TRUE(calc.exec());
    EXPECT_EQ(calc.runCalls, 0);
    EXPECT_EQ(calc.currentState(), GtProcessComponent::SKIPPED);
}

TEST_F(TestGtRunnable, execFailsWithoutRunnableParent)
{
    TestableCalculator calc;

    EXPECT_FALSE(calc.exec());
    EXPECT_EQ(calc.runCalls, 0);
    EXPECT_EQ(calc.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtRunnable, localExecRunsAndFinishes)
{
    TestCalculatorRunnable runnable;
    TestableCalculator calc;
    calc.setParent(&runnable);

    bool transferred = false;
    QObject::connect(&calc, &GtCalculator::transferMonitoringProperties,
                     [&transferred]() { transferred = true; });

    EXPECT_TRUE(calc.exec());
    EXPECT_EQ(calc.runCalls, 1);
    EXPECT_EQ(calc.currentState(), GtProcessComponent::FINISHED);
    EXPECT_TRUE(transferred);
}

TEST_F(TestGtRunnable, localExecWithWarningFinishesWithWarningState)
{
    TestCalculatorRunnable runnable;
    TestableCalculator calc;
    calc.warning = true;
    calc.setParent(&runnable);

    EXPECT_TRUE(calc.exec());
    EXPECT_EQ(calc.runCalls, 1);
    EXPECT_EQ(calc.currentState(), GtProcessComponent::WARN_FINISHED);
}

TEST_F(TestGtRunnable, localExecFailureMarksCalculatorFailed)
{
    TestCalculatorRunnable runnable;
    TestableCalculator calc;
    calc.runResult = false;
    calc.setParent(&runnable);

    EXPECT_FALSE(calc.exec());
    EXPECT_EQ(calc.runCalls, 1);
    EXPECT_EQ(calc.currentState(), GtProcessComponent::FAILED);
}

TEST_F(TestGtRunnable, successfulRunClearsTempDirectoryWhenEnabled)
{
    TestCalculatorRunnable runnable;
    TestableCalculator calc;
    calc.useTempPath = true;
    calc.enableTempRemoval(true);
    calc.setParent(&runnable);

    ASSERT_TRUE(calc.exec());
    EXPECT_EQ(runnable.clearTempDirCalls, 1);
    EXPECT_EQ(runnable.clearedPath, QString("temp/calculator"));
}
