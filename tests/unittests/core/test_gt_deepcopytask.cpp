/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *  Author: Matthias Schuff (SG-VTM)
 */

#include "gtest/gtest.h"

#include <memory>

#include "gt_objectuuidmap.h"


#include "test_gt_processtestclasses.h"
#include "gt_task.h"

#include "gt_deepcopytask.h"
#include "gt_taskanalysis.h"
#include "gt_makepropertyconnection.h"

#include "gt_processdata.h"
#include "gt_propertyconnectionfunctions.h"
#include "gt_objectfactory.h"
#include "gt_taskfactory.h"


class TestGtTaskPropertyConnections : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        if (!gtObjectFactory->knownClass(GT_CLASSNAME(GtPropertyConnection)))
            gtObjectFactory->registerClass(
                GtPropertyConnection::staticMetaObject);

        if (!gtObjectFactory->knownClass(GT_CLASSNAME(TestGtCalculator)))
            gtObjectFactory->registerClass(TestGtCalculator::staticMetaObject);

        if (!gtObjectFactory->knownClass(GT_CLASSNAME(TestTaskWithProperty)))
            gtObjectFactory->registerClass(
                TestTaskWithProperty::staticMetaObject);

        roottask = new TestTaskWithProperty;
        roottask->setObjectName("rt");

        subtask1 = new TestTaskWithProperty;
        subtask1->setObjectName("st1");

        subtask2 = new TestTaskWithProperty;
        subtask2->setObjectName("st2");


        calc1 = new TestGtCalculator;
        calc1->setObjectName("calc1");

        calc2 = new TestGtCalculator;
        calc2->setObjectName("calc2");

        calc3 = new TestGtCalculator;
        calc3->setObjectName("calc3");

        subtask1->appendChild(calc1);
        subtask1->appendChild(calc2);

        subtask2->appendChild(calc3);

        roottask->appendChild(subtask1);
        roottask->appendChild(subtask2);

        taskgroup = new GtTaskGroup;
        taskgroup->appendChild(roottask);

        con1 = gt::utils::process::makePropertyConnection(
            roottask, "doubleProp1", subtask1, "doubleProp1");
        con2 = gt::utils::process::makePropertyConnection(
            roottask, "doubleProp1", subtask2, "doubleProp1");

        con3 = gt::utils::process::makePropertyConnection(
            subtask1, "doubleProp1", calc1, "doubleProp1");
        con4 = gt::utils::process::makePropertyConnection(calc1, "doubleProp2",
                                                          calc2, "doubleProp1");

        con5 = gt::utils::process::makePropertyConnection(calc2, "doubleProp2",
                                                          calc3, "doubleProp1");
        con6 = gt::utils::process::makePropertyConnection(
            subtask1, "doubleProp1", subtask2, "doubleProp2");

        con7 = gt::utils::process::makePropertyConnection(
            subtask2, "doubleProp1", calc3, "doubleProp2");

        con_obsolete = new GtPropertyConnection;
        con_obsolete->setSourceUuid("no-real-calc");
        con_obsolete->setSourceProp("fakeprop");
        con_obsolete->setTargetUuid("other-not-real-calc");
        con_obsolete->setTargetProp("fakeprop");

        con_stale1 = new GtPropertyConnection;
        con_stale1->setSourceUuid(calc1->uuid());
        con_stale1->setSourceProp("doubleProp1");
        con_stale1->setTargetUuid("other-not-real-calc");
        con_stale1->setTargetProp("fakeprop");

        con_stale2 = new GtPropertyConnection;
        con_stale2->setSourceUuid("no-real-calc");
        con_stale2->setSourceProp("fakeprop");
        con_stale2->setTargetUuid(calc1->uuid());
        con_stale2->setTargetProp("doubleProp1");

        roottask->appendChild(con_obsolete);
        roottask->appendChild(con_stale1);
        roottask->appendChild(con_stale2);

        //gt::debug::helper::printTaskStructure(roottask, 0, "The tested task structure:");

        con1->setObjectName("con1");
        con2->setObjectName("con2");
        con3->setObjectName("con3");
        con4->setObjectName("con4");
        con5->setObjectName("con5");
        con6->setObjectName("con6");
        con7->setObjectName("con7");
        con_stale1->setObjectName("con_stale1");
        con_stale2->setObjectName("con_stale2");
        con_obsolete->setObjectName("con_obsolete");

        cons.append(con1);
        cons.append(con2);
        cons.append(con3);
        cons.append(con4);
        cons.append(con5);
        cons.append(con6);
        cons.append(con7);
        cons.append(con_obsolete);
        cons.append(con_stale1);
        cons.append(con_stale2);

        tasks.append(roottask);
        tasks.append(subtask1);
        tasks.append(subtask2);

        calcs.append(calc1);
        calcs.append(calc2);
        calcs.append(calc3);

        //setFactories();
    }

    virtual void TearDown()
    {
        gtDebug() << "---- END OF TEST ----";
        delete taskgroup;
    }

    // void setFactories()
    // {
    //     taskgroup->setFactory(gtObjectFactory);

    //     for(auto* x: cons)
    //     {
    //         x->setFactory(gtObjectFactory);
    //     }

    //     for(auto* x: tasks)
    //     {
    //         x->setFactory(gtTaskFactory);
    //     }

    //     for(auto* x: calcs)
    //     {
    //         x->setFactory(gtObjectFactory);
    //     }
    // }


    GtTaskGroup* taskgroup;


    QList<GtPropertyConnection*> cons;
    QList<TestTaskWithProperty*> tasks;
    QList<TestGtCalculator*> calcs;

    TestTaskWithProperty* roottask;
    TestTaskWithProperty* subtask1;
    TestGtCalculator* calc1;
    TestGtCalculator* calc2;
    TestTaskWithProperty* subtask2;
    TestGtCalculator* calc3;

    GtPropertyConnection* con1;
    GtPropertyConnection* con2;
    GtPropertyConnection* con3;
    GtPropertyConnection* con4;
    GtPropertyConnection* con5;
    GtPropertyConnection* con6;
    GtPropertyConnection* con7;

    GtPropertyConnection* con_obsolete;
    GtPropertyConnection* con_stale1;
    GtPropertyConnection* con_stale2;
};


TEST_F(TestGtTaskPropertyConnections, highestParent)
{
    ASSERT_EQ(gt::utils::process::findRootTask(roottask), roottask);
    ASSERT_EQ(gt::utils::process::findRootTask(subtask1), roottask);
    ASSERT_EQ(gt::utils::process::findRootTask(calc1), roottask);

    auto standaloneTask = new GtTask;
    ASSERT_EQ(gt::utils::process::findRootTask(standaloneTask), standaloneTask);

    auto standaloneCalc = new TestGtCalculator;
    ASSERT_EQ(gt::utils::process::findRootTask(standaloneCalc), nullptr);

    delete standaloneTask;
    delete standaloneCalc;
}

TEST_F(TestGtTaskPropertyConnections, checkTaskPropertyConnectionsErrorBehavior)
{
    bool ok;

    // nullptr send
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        nullptr, &ok);
    Q_UNUSED(retval)

    ASSERT_FALSE(ok);
}

TEST_F(TestGtTaskPropertyConnections, analyzeConnectionsRoot)
{
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        roottask, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 7);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 2);
    ASSERT_EQ(retval.obsolete.size(), 1);

    ASSERT_TRUE(retval.internal.contains(con1));
    ASSERT_TRUE(retval.internal.contains(con2));
    ASSERT_TRUE(retval.internal.contains(con3));
    ASSERT_TRUE(retval.internal.contains(con4));
    ASSERT_TRUE(retval.internal.contains(con5));
    ASSERT_TRUE(retval.internal.contains(con6));
    ASSERT_TRUE(retval.internal.contains(con7));

    ASSERT_TRUE(retval.stale.contains(con_stale1));
    ASSERT_TRUE(retval.stale.contains(con_stale2));

    ASSERT_TRUE(retval.obsolete.contains(con_obsolete));
}

TEST_F(TestGtTaskPropertyConnections, analyzeConnectionsSubtask1)
{
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        subtask1, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 2);
    ASSERT_EQ(retval.external.size(), 3);
    ASSERT_EQ(retval.foreign.size(), 2);
    ASSERT_EQ(retval.stale.size(), 2);
    ASSERT_EQ(retval.obsolete.size(), 1);

    ASSERT_TRUE(retval.internal.contains(con3));
    ASSERT_TRUE(retval.internal.contains(con4));

    ASSERT_TRUE(retval.external.contains(con1));
    ASSERT_TRUE(retval.external.contains(con5));
    ASSERT_TRUE(retval.external.contains(con6));

    ASSERT_TRUE(retval.foreign.contains(con2));
    ASSERT_TRUE(retval.foreign.contains(con7));

    ASSERT_TRUE(retval.stale.contains(con_stale1));
    ASSERT_TRUE(retval.stale.contains(con_stale2));

    ASSERT_TRUE(retval.obsolete.contains(con_obsolete));
}


TEST_F(TestGtTaskPropertyConnections, analyzeConnectionsSubtask2)
{
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        subtask2, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 1);
    ASSERT_EQ(retval.external.size(), 3);
    ASSERT_EQ(retval.foreign.size(), 5);
    ASSERT_EQ(retval.stale.size(), 0);
    ASSERT_EQ(retval.obsolete.size(), 1);

    ASSERT_TRUE(retval.internal.contains(con7));

    ASSERT_TRUE(retval.external.contains(con2));
    ASSERT_TRUE(retval.external.contains(con5));
    ASSERT_TRUE(retval.external.contains(con6));

    ASSERT_TRUE(retval.foreign.contains(con1));
    ASSERT_TRUE(retval.foreign.contains(con3));
    ASSERT_TRUE(retval.foreign.contains(con4));
    ASSERT_TRUE(retval.foreign.contains(con_stale1));
    ASSERT_TRUE(retval.foreign.contains(con_stale2));

    ASSERT_TRUE(retval.obsolete.contains(con_obsolete));
}

TEST_F(TestGtTaskPropertyConnections, deepCopyRoottask)
{
    auto roottask_copied = gt::utils::process::deepCopyTask(roottask);

    //gt::debug::helper::printTaskStructure(roottask_copied, 0, "The copied task structure:");
    //gt::utils::process::printWarningForLostPropertyConnections(roottask_copied, true);

    ASSERT_NE(roottask_copied, nullptr);

    // get the copied sub components
    auto subtask1_copied = roottask_copied->findDirectChild<GtTask*>("st1");
    auto subtask2_copied = roottask_copied->findDirectChild<GtTask*>("st2");

    ASSERT_NE(subtask1_copied, nullptr);
    ASSERT_NE(subtask2_copied, nullptr);

    auto calc1_copied =
        subtask1_copied->findDirectChild<GtCalculator*>("calc1");
    auto calc2_copied =
        subtask1_copied->findDirectChild<GtCalculator*>("calc2");
    auto calc3_copied =
        subtask2_copied->findDirectChild<GtCalculator*>("calc3");

    ASSERT_NE(calc1_copied, nullptr);
    ASSERT_NE(calc2_copied, nullptr);
    ASSERT_NE(calc3_copied, nullptr);

    // test UUID has changed
    ASSERT_NE(roottask->uuid(), roottask_copied->uuid());
    ASSERT_NE(subtask1->uuid(), subtask1_copied->uuid());
    ASSERT_NE(subtask2->uuid(), subtask2_copied->uuid());
    ASSERT_NE(calc1->uuid(), calc1_copied->uuid());
    ASSERT_NE(calc2->uuid(), calc2_copied->uuid());
    ASSERT_NE(calc3->uuid(), calc3_copied->uuid());

    // anaylze the property connections
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        roottask_copied, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 7);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 2);
    ASSERT_EQ(retval.obsolete.size(), 1);

    // get the copied connections
    auto con1_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con1");
    auto con2_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con2");
    auto con3_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con3");
    auto con4_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con4");
    auto con5_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con5");
    auto con6_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con6");
    auto con7_copied =
        roottask_copied->findChild<GtPropertyConnection*>("con7");

    ASSERT_NE(con1_copied, nullptr);
    ASSERT_NE(con2_copied, nullptr);
    ASSERT_NE(con3_copied, nullptr);
    ASSERT_NE(con4_copied, nullptr);
    ASSERT_NE(con5_copied, nullptr);
    ASSERT_NE(con6_copied, nullptr);
    ASSERT_NE(con7_copied, nullptr);

    // check if connections have been transferred to new UUIDs
    ASSERT_EQ(con1_copied->sourceUuid(), roottask_copied->uuid());
    ASSERT_EQ(con1_copied->targetUuid(), subtask1_copied->uuid());

    ASSERT_EQ(con2_copied->sourceUuid(), roottask_copied->uuid());
    ASSERT_EQ(con2_copied->targetUuid(), subtask2_copied->uuid());

    ASSERT_EQ(con3_copied->sourceUuid(), subtask1_copied->uuid());
    ASSERT_EQ(con3_copied->targetUuid(), calc1_copied->uuid());

    ASSERT_EQ(con4_copied->sourceUuid(), calc1_copied->uuid());
    ASSERT_EQ(con4_copied->targetUuid(), calc2_copied->uuid());

    ASSERT_EQ(con5_copied->sourceUuid(), calc2_copied->uuid());
    ASSERT_EQ(con5_copied->targetUuid(), calc3_copied->uuid());

    ASSERT_EQ(con6_copied->sourceUuid(), subtask1_copied->uuid());
    ASSERT_EQ(con6_copied->targetUuid(), subtask2_copied->uuid());

    ASSERT_EQ(con7_copied->sourceUuid(), subtask2_copied->uuid());
    ASSERT_EQ(con7_copied->targetUuid(), calc3_copied->uuid());

    delete roottask_copied;
}


TEST_F(TestGtTaskPropertyConnections, deepCopySubtask1)
{
    auto subtask1_copied = gt::utils::process::deepCopyTask(subtask1);

    // get the copied sub components
    ASSERT_NE(subtask1_copied, nullptr);

    auto calc1_copied =
        subtask1_copied->findDirectChild<GtCalculator*>("calc1");
    auto calc2_copied =
        subtask1_copied->findDirectChild<GtCalculator*>("calc2");

    ASSERT_NE(calc1_copied, nullptr);
    ASSERT_NE(calc2_copied, nullptr);

    // test UUID has changed
    ASSERT_NE(subtask1->uuid(), subtask1_copied->uuid());
    ASSERT_NE(calc1->uuid(), calc1_copied->uuid());
    ASSERT_NE(calc2->uuid(), calc2_copied->uuid());

    // anaylze the property connections
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        subtask1_copied, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 2);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 0);
    ASSERT_EQ(retval.obsolete.size(), 0);

    // get the copied connections
    auto con3_copied =
        subtask1_copied->findChild<GtPropertyConnection*>("con3");
    auto con4_copied =
        subtask1_copied->findChild<GtPropertyConnection*>("con4");

    ASSERT_NE(con3_copied, nullptr);
    ASSERT_NE(con4_copied, nullptr);

    // check if connections have been transferred to new UUIDs
    ASSERT_EQ(con3_copied->sourceUuid(), subtask1_copied->uuid());
    ASSERT_EQ(con3_copied->targetUuid(), calc1_copied->uuid());

    ASSERT_EQ(con4_copied->sourceUuid(), calc1_copied->uuid());
    ASSERT_EQ(con4_copied->targetUuid(), calc2_copied->uuid());

    delete subtask1_copied;
}


TEST_F(TestGtTaskPropertyConnections, deepCopySubtask2)
{
    auto subtask2_copied = gt::utils::process::deepCopyTask(subtask2);

    // get the copied sub components
    ASSERT_NE(subtask2_copied, nullptr);

    auto calc3_copied =
        subtask2_copied->findDirectChild<GtCalculator*>("calc3");

    ASSERT_NE(calc3_copied, nullptr);

    // test UUID has changed
    ASSERT_NE(subtask2->uuid(), subtask2_copied->uuid());
    ASSERT_NE(calc3->uuid(), calc3_copied->uuid());

    // anaylze the property connections
    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        subtask2_copied, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 1);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 0);
    ASSERT_EQ(retval.obsolete.size(), 0);

    // get the copied connections
    auto con7_copied =
        subtask2_copied->findChild<GtPropertyConnection*>("con7");

    ASSERT_NE(con7_copied, nullptr);

    // check if connections have been transferred to new UUIDs
    ASSERT_EQ(con7_copied->sourceUuid(), subtask2_copied->uuid());
    ASSERT_EQ(con7_copied->targetUuid(), calc3_copied->uuid());

    delete subtask2_copied;
}


TEST_F(TestGtTaskPropertyConnections, cleanRoottask)
{
    gt::utils::process::cleanupPropertyConnections(roottask);

    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        roottask, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 7);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 0);
    ASSERT_EQ(retval.obsolete.size(), 0);
}

TEST_F(TestGtTaskPropertyConnections, cleanSubtask1ShouldHaveNoEffect)
{
    ASSERT_FALSE(gt::utils::process::cleanupPropertyConnections(subtask1));

    bool ok = false;
    auto retval = gt::utils::process::analyzePropertyConnectionsRelationship(
        roottask, &ok);
    ASSERT_TRUE(ok);

    ASSERT_EQ(retval.internal.size(), 7);
    ASSERT_EQ(retval.external.size(), 0);
    ASSERT_EQ(retval.foreign.size(), 0);
    ASSERT_EQ(retval.stale.size(), 2);
    ASSERT_EQ(retval.obsolete.size(), 1);


    bool ok2 = false;
    auto retval2 = gt::utils::process::analyzePropertyConnectionsRelationship(
        subtask1, &ok2);
    ASSERT_TRUE(ok2);

    ASSERT_EQ(retval2.internal.size(), 2);
    ASSERT_EQ(retval2.external.size(), 3);
    ASSERT_EQ(retval2.foreign.size(), 2);
    ASSERT_EQ(retval2.stale.size(), 2);
    ASSERT_EQ(retval2.obsolete.size(), 1);
}

TEST_F(TestGtTaskPropertyConnections, deepCopySubtask1AndPaste)
{
    auto subtask1_copied = gt::utils::process::deepCopyTask(subtask1);

    // get the copied sub components
    ASSERT_TRUE(subtask1_copied);

    auto propCons =
        subtask1_copied->findDirectChildren<GtPropertyConnection*>();

    // make new taskgroup with task and add subtask
    auto taskgroup = new GtTaskGroup;
    auto newtask = new GtTask;

    taskgroup->appendChild(newtask);

    gt::utils::process::pasteProcessComponent(newtask, subtask1_copied);

    // check if subtask connections have been transferred into the new roottask
    auto newtaskChildren = newtask->findDirectChildren();

    for (auto p : qAsConst(propCons))
    {
        ASSERT_TRUE(newtaskChildren.contains(p));
    }
}


class TestGtTaskRelativeObjectLinkProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        if (!gtObjectFactory->knownClass(GT_CLASSNAME(TestGtCalculator)))
            gtObjectFactory->registerClass(TestGtCalculator::staticMetaObject);

        if (!gtObjectFactory->knownClass(
                GT_CLASSNAME(TestTaskWithRelativeObjectLinkProperty)))
            gtObjectFactory->registerClass(
                TestTaskWithRelativeObjectLinkProperty::staticMetaObject);

        roottask = new TestTaskWithRelativeObjectLinkProperty;
        roottask->setObjectName("rt");

        calc1 = new TestGtCalculator;
        calc1->setObjectName("c1");

        roottask->appendChild(calc1);
        roottask->propRelObjLink()->setVal(calc1->uuid());
    }

    virtual void TearDown()
    {
        delete roottask;
    }

    TestTaskWithRelativeObjectLinkProperty* roottask;
    TestGtCalculator* calc1;
};


TEST_F(TestGtTaskRelativeObjectLinkProperty,
       copyTaskWithRelativeObjectLinkProperty)
{
    auto copy = gt::utils::process::deepCopyTask(roottask);

    ASSERT_TRUE(copy);

    auto copyCasted =
        qobject_cast<TestTaskWithRelativeObjectLinkProperty*>(copy);

    auto copiedRelObjLinkProp = copyCasted->propRelObjLink();
    auto copiedCalc = copyCasted->findDirectChild<TestGtCalculator*>("c1");

    ASSERT_EQ(copiedRelObjLinkProp->getVal(), copiedCalc->uuid());
}

TEST_F(TestGtTaskPropertyConnections, incompleteMappingClearsMissingEndpoints)
{
    for (int missing = 1; missing <= 3; ++missing)
    {
        SCOPED_TRACE(missing);
        GtTask task;
        auto* connection = new GtPropertyConnection;
        connection->setSourceUuid("original-source");
        connection->setTargetUuid("original-target");
        task.appendChild(connection);

        GtObjectUUIDMap mapping;
        if (!(missing & 1))
        {
            mapping.insert("original-source", "copied-source");
        }
        if (!(missing & 2))
        {
            mapping.insert("original-target", "copied-target");
        }

        EXPECT_FALSE(gt::utils::process::transferPropertyConnectionsViaMapping(
            &task, &mapping));
        EXPECT_EQ(connection->sourceUuid(),
                  (missing & 1) ? QString() : QString("copied-source"));
        EXPECT_EQ(connection->targetUuid(),
                  (missing & 2) ? QString() : QString("copied-target"));
        EXPECT_FALSE(connection->isConnected());
    }
}

TEST_F(TestGtTaskPropertyConnections, copyDistinguishesCalculatorsWithSameName)
{
    calc1->setObjectName("calculator");
    calc2->setObjectName("calculator");
    calc1->prop2()->setVal(17.0);
    calc2->prop2()->setVal(29.0);
    const double originalTargetValue = calc2->prop1()->getVal();
    std::unique_ptr<GtTask> copy(
        gt::utils::process::deepCopyTask(subtask1, true));
    ASSERT_NE(copy, nullptr);

    auto* connection = copy->findChild<GtPropertyConnection*>("con4");
    ASSERT_NE(connection, nullptr);
    auto* source = qobject_cast<TestGtCalculator*>(
        copy->getObjectByUuid(connection->sourceUuid()));
    auto* target = qobject_cast<TestGtCalculator*>(
        copy->getObjectByUuid(connection->targetUuid()));
    ASSERT_NE(source, nullptr);
    ASSERT_NE(target, nullptr);
    EXPECT_NE(source, target);
    EXPECT_EQ(source->objectName(), "calculator");
    EXPECT_EQ(target->objectName(), "calculator");
    EXPECT_NE(source->uuid(), calc1->uuid());
    EXPECT_NE(target->uuid(), calc2->uuid());
    EXPECT_TRUE(connection->isConnected());

    EXPECT_DOUBLE_EQ(source->prop2()->getVal(), 17.0);
    EXPECT_DOUBLE_EQ(target->prop2()->getVal(), 29.0);
    source->prop2()->setVal(42.0);
    EXPECT_DOUBLE_EQ(target->prop1()->getVal(), 42.0);
    EXPECT_DOUBLE_EQ(calc2->prop1()->getVal(), originalTargetValue);
}

TEST_F(TestGtTaskRelativeObjectLinkProperty, copyRemapsNestedRelativeLink)
{
    auto* nestedTask = new GtTask;
    roottask->appendChild(nestedTask);
    nestedTask->appendChild(calc1);
    auto* target = new TestGtCalculator;
    target->setObjectName("target");
    nestedTask->appendChild(target);
    calc1->relLink()->setVal(target->uuid());

    std::unique_ptr<GtTask> copy(
        gt::utils::process::deepCopyTask(roottask, true));
    ASSERT_NE(copy, nullptr);
    auto* copiedSource = copy->findChild<TestGtCalculator*>("c1");
    auto* copiedTarget = copy->findChild<TestGtCalculator*>("target");
    ASSERT_NE(copiedSource, nullptr);
    ASSERT_NE(copiedTarget, nullptr);
    EXPECT_EQ(copiedSource->relLink()->getVal(), copiedTarget->uuid());
    EXPECT_NE(copiedTarget->uuid(), target->uuid());
    EXPECT_EQ(calc1->relLink()->getVal(), target->uuid());
}

TEST_F(TestGtTaskRelativeObjectLinkProperty,
       externalRelativeLinkHonorsStrictness)
{
    TestGtCalculator external;
    roottask->propRelObjLink()->setVal(external.uuid());

    for (bool strict : {false, true})
    {
        SCOPED_TRACE(strict);
        std::unique_ptr<GtTask> copy(
            gt::utils::process::deepCopyTask(roottask, strict));
        if (strict)
        {
            EXPECT_EQ(copy, nullptr);
        }
        else
        {
            ASSERT_NE(copy, nullptr);
            auto* copiedTask =
                qobject_cast<TestTaskWithRelativeObjectLinkProperty*>(
                    copy.get());
            ASSERT_NE(copiedTask, nullptr);
            EXPECT_EQ(copiedTask->propRelObjLink()->getVal(), external.uuid());
        }
        EXPECT_EQ(roottask->propRelObjLink()->getVal(), external.uuid());
        EXPECT_EQ(calc1->parent(), roottask);
    }
}
