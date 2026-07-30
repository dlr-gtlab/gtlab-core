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
#include "gt_object.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"
#include "gt_project.h"
#include "gt_runnable.h"

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

    void addLinkedObject(GtObject* object)
    {
        m_linkedObjects.append(object);
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

class TestableLinkedCalculator : public TestableCalculator
{
public:
    TestableLinkedCalculator() :
        linkProp("link", "Link", "Link", this, {GT_CLASSNAME(GtObject)}),
        pathProp("path", "Path", "Path", "", this, {GT_CLASSNAME(GtObject)})
    {
        registerProperty(linkProp);
        registerProperty(pathProp);
    }

    void setLinkedUuid(const QString& uuid)
    {
        linkProp.setVal(uuid);
    }

    void setLinkedPath(const QString& path)
    {
        pathProp.setVal(path);
    }

private:
    GtObjectLinkProperty linkProp;
    GtObjectPathProperty pathProp;
};

class ContextObservingCalculator : public TestableCalculator
{
public:
    GtProject* observedProject = nullptr;
    QString observedPath;
    bool contextWasActive = false;

    bool run() override
    {
        auto const* context = GtExecutionContext::current();
        contextWasActive = context != nullptr;
        observedProject = context ? context->project() : nullptr;
        observedPath = projectPath();
        return true;
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

TEST_F(TestGtRunnable, unknownExecutionModeLeavesCalculatorInLocalMode)
{
    TestableCalculator calc;

    EXPECT_EQ(calc.execMode(), QString("local"));

    calc.setExecMode("missing-executor");

    EXPECT_EQ(calc.execMode(), QString("local"));

    calc.setExecModeLocal();
    EXPECT_EQ(calc.execMode(), QString("local"));
}

TEST_F(TestGtRunnable, executionLabelRoundTrips)
{
    TestableCalculator calc;

    calc.setExecutionLabel("worker-A");

    EXPECT_EQ(calc.executionLabel(), QString("worker-A"));
}

TEST_F(TestGtRunnable, missingLinkedObjectsDoNotPreventSuccessfulRun)
{
    TestCalculatorRunnable runnable;
    TestableLinkedCalculator calc;
    calc.setParent(&runnable);
    calc.setLinkedUuid("missing-uuid");
    calc.setLinkedPath("missing/path");

    ASSERT_TRUE(calc.exec());
    EXPECT_TRUE(calc.linkedObjects().isEmpty());
    EXPECT_EQ(calc.currentState(), GtProcessComponent::FINISHED);
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

TEST_F(TestGtRunnable, customProjectPathRemainsAvailableWithoutContext)
{
    GtRunnable runnable(QStringLiteral("custom-project-path"));

    EXPECT_EQ(runnable.projectPath(), QStringLiteral("custom-project-path"));
}

TEST_F(TestGtRunnable, executionContextIsInstalledForRunAndRestoredAfterwards)
{
    GtExecutionContext context(nullptr, QStringLiteral("project-path"));
    GtRunnable runnable(QString{}, context);
    ContextObservingCalculator observer;
    ASSERT_TRUE(runnable.appendProcessComponent(&observer));

    runnable.run();

    EXPECT_TRUE(observer.contextWasActive);
    EXPECT_EQ(observer.observedPath, QStringLiteral("project-path"));
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST_F(TestGtRunnable, runWithoutContextPreservesAnExistingContext)
{
    GtExecutionContext outerContext(nullptr, QStringLiteral("outer-project"));
    GtExecutionContextScope outerScope(outerContext);
    GtRunnable runnable;

    runnable.run();

    EXPECT_EQ(GtExecutionContext::current(), &outerContext);
}

TEST_F(TestGtRunnable, sequentialRunsUseDifferentExecutionContexts)
{
    class TestProject : public GtProject
    {
    public:
        explicit TestProject(QString path) : GtProject(std::move(path)) {}
    } firstProject(QStringLiteral("first")),
        secondProject(QStringLiteral("second"));

    GtExecutionContext firstContext(&firstProject,
                                    QStringLiteral("first-path"));
    GtRunnable firstRunnable({}, firstContext);
    ContextObservingCalculator firstCalculator;
    ASSERT_TRUE(firstRunnable.appendProcessComponent(&firstCalculator));
    firstRunnable.run();

    GtExecutionContext secondContext(&secondProject,
                                     QStringLiteral("second-path"));
    GtRunnable secondRunnable({}, secondContext);
    ContextObservingCalculator secondCalculator;
    ASSERT_TRUE(secondRunnable.appendProcessComponent(&secondCalculator));
    secondRunnable.run();

    EXPECT_EQ(firstCalculator.observedProject, &firstProject);
    EXPECT_EQ(firstCalculator.observedPath, QStringLiteral("first-path"));
    EXPECT_EQ(secondCalculator.observedProject, &secondProject);
    EXPECT_EQ(secondCalculator.observedPath, QStringLiteral("second-path"));
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}
