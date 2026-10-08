/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 * Source File: test_gt_processcomponent.cpp
 */

#include "gtest/gtest.h"

#include <QDir>
#include <QTemporaryDir>
#include <QUuid>
#include <cassert>
#include <memory>

#include "test_gt_processtestclasses.h"

#include "gt_abstractrunnable.h"
#include "gt_environment.h"
#include "gt_intproperty.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_objectfactory.h"
#include "gt_task.h"
#include "gt_taskgroup.h"

namespace
{
QString uniqueEnvVar()
{
    return QStringLiteral("unittest_processcomponent_%1")
        .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
}
}

class TestProcessRunnable : public GtAbstractRunnable
{
public:
    explicit TestProcessRunnable(QString project = {}) :
        m_projectPath(std::move(project))
    {
    }

    int tempDirCalls = 0;
    QString tempDirPath;

    void run() override
    {
    }

    QDir tempDir() override
    {
        ++tempDirCalls;
        return QDir(tempDirPath);
    }

    bool clearTempDir(const QString&) override
    {
        return true;
    }

    QString projectPath() override
    {
        return m_projectPath;
    }

private:
    QString m_projectPath;
};

class TestableProcessComponent : public TestGtProcessComponent
{
public:
    using GtProcessComponent::appendToLinkObjects;
    using GtProcessComponent::environmentVariable;
    using GtProcessComponent::projectPath;
    using GtProcessComponent::registerMonitoringProperty;
    using GtProcessComponent::setRunnable;
    using GtProcessComponent::setTempPath;
    using GtProcessComponent::setWarningFlag;
    using GtProcessComponent::tempPath;

    TestableProcessComponent() :
        directMonitoring("directMonitoring", "Direct Monitoring", "Direct", 5),
        readOnlyProp("readOnlyProp", "Read Only", "Read only", 7),
        writableProp("writableProp", "Writable", "Writable", 9)
    {
        registerMonitoringProperty(directMonitoring);
        registerProperty(readOnlyProp);
        registerProperty(writableProp);

        readOnlyProp.setReadOnly(true);
    }

    GtIntProperty directMonitoring;
    GtIntProperty readOnlyProp;
    GtIntProperty writableProp;
};

/// Process component with an additional, test-only "remote" execution mode
/// so that inheriting concrete non-local modes can be verified.
class TestableModeComponent : public TestGtProcessComponent
{
public:
    TestableModeComponent()
    {
        auto* modeProp =
            qobject_cast<GtModeProperty*>(findProperty("execMode"));
        assert(modeProp);

        auto* remoteMode = new GtModeTypeProperty("remote", tr("remote"));
        remoteMode->setParent(this);
        modeProp->registerSubProperty(*remoteMode);
    }
};

class TestGtProcessComponentFunctions : public ::testing::Test
{
protected:
    void SetUp() override
    {
        processComp.setFactory(gtObjectFactory);

        gtObjectFactory->registerClass(
            TestGtProcessComponent::staticMetaObject);
    }

    TestGtProcessComponent processComp;
};

TEST_F(TestGtProcessComponentFunctions, findMonitoringDynamicSizeProperty)
{
    ASSERT_TRUE(processComp.findPropertyContainer("monitoringVars"));
}

TEST_F(TestGtProcessComponentFunctions, checkContainerMonitoringPropSize)
{
    const auto& monProps = processComp.monitoringProperties();
    const auto& containerMonPropRefs =
            processComp.containerMonitoringPropertyRefs();

    EXPECT_EQ(0u, monProps.size());
    EXPECT_EQ(0u, containerMonPropRefs.size());

    auto* props = processComp.findPropertyContainer("monitoringVars");
    ASSERT_TRUE(props);

    EXPECT_EQ(0u, props->size());

    processComp.addMonitoringVar("name", 42);

    EXPECT_EQ(1u, props->size());

    EXPECT_EQ(0u, monProps.size());
    EXPECT_EQ(1u, containerMonPropRefs.size());

    props->removeEntry(props->begin());

    EXPECT_EQ(0u, containerMonPropRefs.size());
}

TEST(TestGtProcessComponent, skippedStateOverridesExplicitState)
{
    TestableProcessComponent comp;

    comp.setState(GtProcessComponent::FINISHED);
    EXPECT_EQ(comp.currentState(), GtProcessComponent::FINISHED);

    comp.setSkipped(true);
    EXPECT_TRUE(comp.isSkipped());
    EXPECT_EQ(comp.currentState(), GtProcessComponent::SKIPPED);
}

TEST(TestGtProcessComponent, stateRecursivelyPropagatesToChildren)
{
    auto parent = std::make_unique<TestableProcessComponent>();
    auto child = std::make_unique<TestableProcessComponent>();
    auto* childPtr = child.get();

    ASSERT_TRUE(parent->appendChild(child.release()));

    parent->setStateRecursively(GtProcessComponent::QUEUED);

    EXPECT_EQ(parent->currentState(), GtProcessComponent::QUEUED);
    EXPECT_EQ(childPtr->currentState(), GtProcessComponent::QUEUED);
}

TEST(TestGtProcessComponent, progressIsClampedAndResetByIdleStates)
{
    TestableProcessComponent comp;

    comp.setProgress(120);
    EXPECT_EQ(comp.currentProgress(), 100);

    comp.setState(GtProcessComponent::FINISHED);
    EXPECT_EQ(comp.currentProgress(), 100);

    comp.setState(GtProcessComponent::QUEUED);
    EXPECT_EQ(comp.currentProgress(), 0);

    comp.setProgress(-5);
    EXPECT_EQ(comp.currentProgress(), 0);
}

TEST(TestGtProcessComponent, rootTaskAndReadyStateFollowHierarchy)
{
    GtTaskGroup group("group", true);
    auto rootTask = std::make_unique<GtTask>();
    auto child = std::make_unique<TestableProcessComponent>();
    auto* rootTaskPtr = rootTask.get();
    auto* childPtr = child.get();

    ASSERT_TRUE(group.appendChild(rootTask.release()));
    ASSERT_TRUE(rootTaskPtr->appendChild(child.release()));

    EXPECT_EQ(childPtr->rootTask(), rootTaskPtr);
    EXPECT_TRUE(childPtr->isReady());

    rootTaskPtr->setState(GtProcessComponent::RUNNING);
    EXPECT_FALSE(childPtr->isReady());

    rootTaskPtr->setState(GtProcessComponent::FINISHED);
    EXPECT_TRUE(childPtr->isReady());
}

TEST(TestGtProcessComponent, directMonitoringAndPropertyFiltersBehaveAsExpected)
{
    TestableProcessComponent comp;

    const auto monitoringProps = comp.monitoringProperties();
    const auto readWriteProps = comp.readWriteProperties();
    const auto nonReadOnlyProps = comp.nonReadOnlyProperties();

    EXPECT_EQ(monitoringProps.size(), 1);
    EXPECT_TRUE(monitoringProps.contains(&comp.directMonitoring));
    EXPECT_FALSE(readWriteProps.contains(&comp.directMonitoring));
    EXPECT_TRUE(readWriteProps.contains(&comp.readOnlyProp));
    EXPECT_TRUE(readWriteProps.contains(&comp.writableProp));
    EXPECT_FALSE(nonReadOnlyProps.contains(&comp.readOnlyProp));
    EXPECT_TRUE(nonReadOnlyProps.contains(&comp.writableProp));
}

TEST(TestGtProcessComponent, resetMonitoringPropertiesRevertsDirectMonitoringProperty)
{
    TestableProcessComponent comp;

    comp.directMonitoring.setVal(99);
    ASSERT_EQ(comp.directMonitoring.getVal(), 99);

    comp.resetMonitoringProperties();

    EXPECT_EQ(comp.directMonitoring.getVal(), 5);
}

TEST(TestGtProcessComponent, warningFlagIsReported)
{
    TestableProcessComponent comp;

    EXPECT_FALSE(comp.hasWarnings());

    comp.setWarningFlag(true);

    EXPECT_TRUE(comp.hasWarnings());
}

TEST(TestGtProcessComponent, tempDirAndProjectPathAreForwardedFromRunnable)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    TestProcessRunnable runnable("project-path");
    runnable.tempDirPath = dir.path();

    TestableProcessComponent comp;
    comp.setRunnable(&runnable);

    EXPECT_EQ(comp.projectPath(), QString("project-path"));

    const auto firstDir = comp.tempDir();
    const auto secondDir = comp.tempDir();

    EXPECT_EQ(firstDir.absolutePath(), dir.path());
    EXPECT_EQ(secondDir.absolutePath(), dir.path());
    EXPECT_EQ(runnable.tempDirCalls, 1);
}

TEST(TestGtProcessComponent, appendedLinkedObjectsCanBeResolvedByUuid)
{
    TestableProcessComponent comp;
    GtObject object;

    comp.appendToLinkObjects(&object);

    EXPECT_EQ(comp.data<GtObject*>(object.uuid()), &object);
    EXPECT_EQ(comp.data<GtObject*>(QStringLiteral("missing")), nullptr);
}

TEST(TestGtProcessComponent, environmentVariableReturnsStoredStringValue)
{
    TestableProcessComponent comp;
    const auto var = uniqueEnvVar();

    ASSERT_TRUE(gtEnvironment->addEnvironmentVariable(var));
    gtEnvironment->setValue(var, QStringLiteral("alpha"));

    EXPECT_EQ(comp.environmentVariable(var), QString("alpha"));
    EXPECT_TRUE(comp.environmentVariable(uniqueEnvVar()).isEmpty());
}

TEST(TestGtProcessComponent, defaultParentModeFallsBackToLocalForRootComponent)
{
    // The raw default of the execution mode property is "parent" (first
    // registered sub-property). Without a parent process component the
    // resolution has to fall back to local execution.
    TestGtProcessComponent comp;

    EXPECT_EQ(comp.execMode(), QString("local"));
}

TEST(TestGtProcessComponent, explicitLocalModeOfRootComponentStaysLocal)
{
    TestGtProcessComponent comp;

    comp.setExecModeLocal();

    EXPECT_EQ(comp.execMode(), QString("local"));
}

TEST(TestGtProcessComponent, rootTaskInParentModeFallsBackToLocal)
{
    // The task group is not a process component, so a task in parent mode
    // has no parent process component to inherit from.
    GtTaskGroup group("group", true);
    GtTask task;

    ASSERT_TRUE(group.appendChild(&task));
    task.setExecMode("parent");

    EXPECT_EQ(task.execMode(), QString("local"));
}

TEST(TestGtProcessComponent,
     componentParentedByRunnableInParentModeFallsBackToLocal)
{
    // A runnable is a QObject parent but not a process component, so parent
    // mode has to resolve to local for its direct children.
    TestProcessRunnable runnable;
    TestableProcessComponent comp;

    ASSERT_TRUE(runnable.appendProcessComponent(&comp));
    comp.setExecMode("parent");

    EXPECT_EQ(comp.execMode(), QString("local"));
}

TEST(TestGtProcessComponent, parentModeInheritsLocalModeFromParent)
{
    auto parent = std::make_unique<TestableProcessComponent>();
    auto child = std::make_unique<TestableProcessComponent>();
    auto* childPtr = child.get();

    ASSERT_TRUE(parent->appendChild(child.release()));

    parent->setExecModeLocal();
    childPtr->setExecMode("parent");

    EXPECT_EQ(childPtr->execMode(), QString("local"));
}

TEST(TestGtProcessComponent, parentModeInheritsRemoteModeFromParent)
{
    auto parent = std::make_unique<TestableModeComponent>();
    auto child = std::make_unique<TestableProcessComponent>();
    auto* childPtr = child.get();

    ASSERT_TRUE(parent->appendChild(child.release()));

    parent->setExecMode("remote");
    childPtr->setExecMode("parent");

    EXPECT_EQ(childPtr->execMode(), QString("remote"));
}

TEST(TestGtProcessComponent, parentModeChainOfThreeLevelsFallsBackToLocal)
{
    auto root = std::make_unique<TestableProcessComponent>();
    auto mid = std::make_unique<TestableProcessComponent>();
    auto leaf = std::make_unique<TestableProcessComponent>();
    auto* midPtr = mid.get();
    auto* leafPtr = leaf.get();

    ASSERT_TRUE(root->appendChild(mid.release()));
    ASSERT_TRUE(midPtr->appendChild(leaf.release()));

    root->setExecMode("parent");
    midPtr->setExecMode("parent");
    leafPtr->setExecMode("parent");

    EXPECT_EQ(leafPtr->execMode(), QString("local"));
    EXPECT_EQ(midPtr->execMode(), QString("local"));
    EXPECT_EQ(root->execMode(), QString("local"));
}

TEST(TestGtProcessComponent, parentModeChainInheritsLocalModeFromRoot)
{
    auto root = std::make_unique<TestableProcessComponent>();
    auto mid = std::make_unique<TestableProcessComponent>();
    auto leaf = std::make_unique<TestableProcessComponent>();
    auto* midPtr = mid.get();
    auto* leafPtr = leaf.get();

    ASSERT_TRUE(root->appendChild(mid.release()));
    ASSERT_TRUE(midPtr->appendChild(leaf.release()));

    root->setExecModeLocal();
    midPtr->setExecMode("parent");
    leafPtr->setExecMode("parent");

    EXPECT_EQ(leafPtr->execMode(), QString("local"));
}

TEST(TestGtProcessComponent, parentModeChainInheritsRemoteModeFromRoot)
{
    auto root = std::make_unique<TestableModeComponent>();
    auto mid = std::make_unique<TestableProcessComponent>();
    auto leaf = std::make_unique<TestableProcessComponent>();
    auto* midPtr = mid.get();
    auto* leafPtr = leaf.get();

    ASSERT_TRUE(root->appendChild(mid.release()));
    ASSERT_TRUE(midPtr->appendChild(leaf.release()));

    root->setExecMode("remote");
    midPtr->setExecMode("parent");
    leafPtr->setExecMode("parent");

    EXPECT_EQ(leafPtr->execMode(), QString("remote"));
    EXPECT_EQ(midPtr->execMode(), QString("remote"));
}

TEST(TestGtProcessComponent, parentModeChainStopsAtFirstExplicitMode)
{
    auto root = std::make_unique<TestableModeComponent>();
    auto mid = std::make_unique<TestableProcessComponent>();
    auto leaf = std::make_unique<TestableProcessComponent>();
    auto* midPtr = mid.get();
    auto* leafPtr = leaf.get();

    ASSERT_TRUE(root->appendChild(mid.release()));
    ASSERT_TRUE(midPtr->appendChild(leaf.release()));

    root->setExecMode("remote");
    midPtr->setExecModeLocal();
    leafPtr->setExecMode("parent");

    // The explicit local mode of the middle element must break the
    // inheritance chain - the root's remote mode must not leak through.
    EXPECT_EQ(leafPtr->execMode(), QString("local"));
}

TEST(TestGtProcessComponent, explicitChildModeWinsOverParentChain)
{
    auto root = std::make_unique<TestableModeComponent>();
    auto mid = std::make_unique<TestableProcessComponent>();
    auto leaf = std::make_unique<TestableProcessComponent>();
    auto* midPtr = mid.get();
    auto* leafPtr = leaf.get();

    ASSERT_TRUE(root->appendChild(mid.release()));
    ASSERT_TRUE(midPtr->appendChild(leaf.release()));

    root->setExecMode("remote");
    midPtr->setExecMode("parent");
    leafPtr->setExecModeLocal();

    EXPECT_EQ(leafPtr->execMode(), QString("local"));
}

TEST(TestGtProcessComponent, parentModeReResolvesWhenParentModeChanges)
{
    auto parent = std::make_unique<TestableModeComponent>();
    auto child = std::make_unique<TestableProcessComponent>();
    auto* childPtr = child.get();

    ASSERT_TRUE(parent->appendChild(child.release()));

    parent->setExecModeLocal();
    childPtr->setExecMode("parent");
    EXPECT_EQ(childPtr->execMode(), QString("local"));

    parent->setExecMode("remote");
    EXPECT_EQ(childPtr->execMode(), QString("remote"));
}

TEST(TestGtProcessComponent, invalidExecModeDoesNotChangeResolvedMode)
{
    auto parent = std::make_unique<TestableModeComponent>();
    auto child = std::make_unique<TestableModeComponent>();
    auto* childPtr = child.get();

    ASSERT_TRUE(parent->appendChild(child.release()));

    // Unknown modes are rejected by the mode property, so the previously
    // stored mode keeps being resolved.
    childPtr->setExecMode("remote");
    childPtr->setExecMode("missing-executor");
    EXPECT_EQ(childPtr->execMode(), QString("remote"));

    childPtr->setExecMode("parent");
    parent->setExecMode("remote");
    childPtr->setExecMode("missing-executor");
    EXPECT_EQ(childPtr->execMode(), QString("remote"));
}

TEST(TestGtProcessComponent, siblingsResolveIndependentExecModes)
{
    auto parent = std::make_unique<TestableModeComponent>();
    auto first = std::make_unique<TestableModeComponent>();
    auto second = std::make_unique<TestableModeComponent>();
    auto* firstPtr = first.get();
    auto* secondPtr = second.get();

    ASSERT_TRUE(parent->appendChild(first.release()));
    ASSERT_TRUE(parent->appendChild(second.release()));

    parent->setExecMode("parent");
    firstPtr->setExecModeLocal();
    secondPtr->setExecMode("remote");

    EXPECT_EQ(firstPtr->execMode(), QString("local"));
    EXPECT_EQ(secondPtr->execMode(), QString("remote"));
}
