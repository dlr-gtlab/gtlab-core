/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_processdock
 * copyright 2009-2017 by DLR
 *
 *  Created on: 09.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gtest/gtest.h"

#include <QPointer>

#include "test_gt_processdock.h"
#include "gt_task.h"
#include "gt_loop.h"
#include "gt_calculator.h"

#include "gt_processdata.h"
#include "gt_processdock.h"
#include "gt_objectfactory.h"
#include "gt_taskfactory.h"
#include "gt_calculatorfactory.h"


class TestGtProcessDock : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        con1 = new GtPropertyConnection;
        con1->setObjectName("Connection1");
        con1->setFactory(gtObjectFactory);

        con2 = new GtPropertyConnection;
        con2->setObjectName("Connection2");
        con2->setFactory(gtObjectFactory);

        con3 = new GtPropertyConnection;
        con3->setObjectName("Connection3");
        con3->setFactory(gtObjectFactory);

        con4 = new GtPropertyConnection;
        con4->setObjectName("Connection4");
        con4->setFactory(gtObjectFactory);

        con5 = new GtPropertyConnection;
        con5->setObjectName("Connection5");
        con5->setFactory(gtObjectFactory);

        gtObjectFactory->registerClass(GtPropertyConnection::staticMetaObject);

        calc1 = new TestGtCalculator;
        calc1->setObjectName("Calc1");
        calc1->setFactory(gtTaskFactory);
        calc2 = new TestGtCalculator;
        calc1->setObjectName("Calc2");
        calc2->setFactory(gtTaskFactory);
        calc3 = new TestGtCalculator;
        calc1->setObjectName("Calc3");
        calc3->setFactory(gtTaskFactory);
        calc4 = new TestGtCalculator;
        calc1->setObjectName("Calc4");
        calc4->setFactory(gtTaskFactory);

        gtObjectFactory->registerClass(TestGtCalculator::staticMetaObject);

        task = new GtTask;
        task->setFactory(gtObjectFactory);

        loop = new TestGtLoop;
        loop->setFactory(gtObjectFactory);
        loop2 = new TestGtLoop;
        loop2->setFactory(gtObjectFactory);
        loop3 = new TestGtLoop;
        loop3->setFactory(gtObjectFactory);

        gtObjectFactory->registerClass(TestGtLoop::staticMetaObject);
    }

    virtual void TearDown()
    {
        if (con1.data() != Q_NULLPTR)
        {
            delete con1.data();
        }

        if (con2.data() != Q_NULLPTR)
        {
            delete con2.data();
        }

        if (con3.data() != Q_NULLPTR)
        {
            delete con3.data();
        }

        if (con4.data() != Q_NULLPTR)
        {
            delete con4.data();
        }

        if (con5.data() != Q_NULLPTR)
        {
            delete con5.data();
        }

        if (calc1.data() != Q_NULLPTR)
        {
            delete calc1.data();
        }

        if (calc2.data() != Q_NULLPTR)
        {
            delete calc2.data();
        }

        if (calc3.data() != Q_NULLPTR)
        {
            delete calc3.data();
        }

        if (calc4.data() != Q_NULLPTR)
        {
            delete calc4.data();
        }

        if (task.data() != Q_NULLPTR)
        {
            delete task.data();
        }

        if (loop.data() != Q_NULLPTR)
        {
            delete loop.data();
        }

        if (loop2.data() != Q_NULLPTR)
        {
            delete loop2.data();
        }

        if (loop3.data() != Q_NULLPTR)
        {
            delete loop3.data();
        }
    }

    QPointer<GtPropertyConnection> con1;
    QPointer<GtPropertyConnection> con2;
    QPointer<GtPropertyConnection> con3;
    QPointer<GtPropertyConnection> con4;
    QPointer<GtPropertyConnection> con5;

    QPointer<TestGtCalculator> calc1;
    QPointer<TestGtCalculator> calc2;
    QPointer<TestGtCalculator> calc3;
    QPointer<TestGtCalculator> calc4;

    QPointer<GtTask> task;
    QPointer<TestGtLoop> loop;
    QPointer<TestGtLoop> loop2;
    QPointer<TestGtLoop> loop3;
};

TEST_F(TestGtProcessDock, findConnectionCopy)
{
    con1->setSourceUuid("sourceUuid1");
    con1->setTargetUuid("targetUuid1");
    con1->setSourceProp("sourceProp1");
    con1->setTargetProp("targetProp1");

    con2->setSourceUuid("sourceUuid2");
    con2->setTargetUuid("targetUuid2");
    con2->setSourceProp("sourceProp2");
    con2->setTargetProp("targetProp2");

    con3->setSourceUuid("sourceUuid3");
    con3->setTargetUuid("targetUuid3");
    con3->setSourceProp("sourceProp3");
    con3->setTargetProp("targetProp3");

    GtPropertyConnection* conCopy =
        qobject_cast<GtPropertyConnection*>(con1->copy());

    ASSERT_TRUE(conCopy != Q_NULLPTR);

    QList<GtPropertyConnection*> cons;
    cons << conCopy << con2 << con3;

    GtPropertyConnection* foundCopy = GtProcessDock::findConnectionCopy(con1,
                                      cons);

    ASSERT_TRUE(foundCopy != Q_NULLPTR);
    ASSERT_TRUE(foundCopy->sourceUuid() == con1->sourceUuid());
    ASSERT_TRUE(foundCopy->targetUuid() == con1->targetUuid());
    ASSERT_TRUE(foundCopy->sourceProp() == con1->sourceProp());
    ASSERT_TRUE(foundCopy->targetProp() == con1->targetProp());
    ASSERT_TRUE(foundCopy->uuid() != con1->uuid());

    foundCopy = GtProcessDock::findConnectionCopy(Q_NULLPTR, cons);

    ASSERT_TRUE(foundCopy == Q_NULLPTR);

    cons.clear();
    foundCopy = GtProcessDock::findConnectionCopy(con1, cons);

    ASSERT_TRUE(foundCopy == Q_NULLPTR);

    foundCopy = GtProcessDock::findConnectionCopy(Q_NULLPTR, cons);

    ASSERT_TRUE(foundCopy == Q_NULLPTR);

    if (conCopy != Q_NULLPTR)
    {
        delete conCopy;
    }
}

TEST_F(TestGtProcessDock, propertyConnectionRelations)
{
    task->appendChild(calc1);
    task->appendChild(calc2);
    task->appendChild(loop);

    loop->appendChild(calc3);
    loop->appendChild(calc4);

    task->appendChild(con1);
    task->appendChild(con2);
    task->appendChild(con3);
    task->appendChild(con4);
    task->appendChild(con5);

    con1->setSourceUuid(calc1->uuid());
    con1->setTargetUuid(calc2->uuid());
    con1->setSourceProp(calc1->prop1()->ident());
    con1->setTargetProp(calc2->prop1()->ident());

    con2->setSourceUuid(calc2->uuid());
    con2->setTargetUuid(calc3->uuid());
    con2->setSourceProp(calc2->prop2()->ident());
    con2->setTargetProp(calc3->prop1()->ident());

    con3->setSourceUuid(calc3->uuid());
    con3->setTargetUuid(calc4->uuid());
    con3->setSourceProp(calc3->prop2()->ident());
    con3->setTargetProp(calc4->prop1()->ident());

    con4->setSourceUuid(calc3->uuid());
    con4->setTargetUuid(calc4->uuid());
    con4->setSourceProp(calc3->prop3()->ident());
    con4->setTargetProp(calc4->prop2()->ident());

    con5->setSourceUuid(loop->uuid());
    con5->setTargetUuid(calc4->uuid());
    con5->setSourceProp(loop->prop1()->ident());
    con5->setTargetProp(calc4->prop3()->ident());

    /// INTERNAL CONNECTIONS

    QList<GtPropertyConnection*> internals =
        GtProcessDock::internalPropertyConnections(task);

    ASSERT_FALSE(internals.isEmpty());
    ASSERT_TRUE(internals.size() == 5);
    ASSERT_TRUE(internals.contains(con1));
    ASSERT_TRUE(internals.contains(con2));
    ASSERT_TRUE(internals.contains(con3));
    ASSERT_TRUE(internals.contains(con4));
    ASSERT_TRUE(internals.contains(con5));

    internals = GtProcessDock::internalPropertyConnections(loop);

    ASSERT_FALSE(internals.isEmpty());
    ASSERT_TRUE(internals.size() == 3);
    ASSERT_TRUE(internals.contains(con3));
    ASSERT_TRUE(internals.contains(con4));

    GtCalculator* calc = Q_NULLPTR;
    GtTask* task = Q_NULLPTR;
    GtProcessComponent* comp = Q_NULLPTR;

    internals = GtProcessDock::internalPropertyConnections(task);
    ASSERT_TRUE(internals.isEmpty());

    internals = GtProcessDock::internalPropertyConnections(comp);
    ASSERT_TRUE(internals.isEmpty());

    /// LOST CONNECTIONS IN CASE OF COPY/CUT/DELETE
    QList<GtPropertyConnection*> lost =
        GtProcessDock::lostPropertyConnections(loop);

    ASSERT_FALSE(lost.isEmpty());
    ASSERT_EQ(lost.size(), 1);
    ASSERT_TRUE(lost.contains(con2));

    lost = GtProcessDock::lostPropertyConnections(task);
    ASSERT_TRUE(lost.isEmpty());

    lost = GtProcessDock::lostPropertyConnections(calc1);
    ASSERT_FALSE(lost.isEmpty());
    ASSERT_EQ(lost.size(), 1);
    ASSERT_TRUE(lost.contains(con1));

    lost = GtProcessDock::lostPropertyConnections(calc2);
    ASSERT_FALSE(lost.isEmpty());
    ASSERT_EQ(lost.size(), 2);
    ASSERT_TRUE(lost.contains(con1));
    ASSERT_TRUE(lost.contains(con2));

    lost = GtProcessDock::lostPropertyConnections(calc3);
    ASSERT_FALSE(lost.isEmpty());
    ASSERT_EQ(lost.size(), 3);
    ASSERT_TRUE(lost.contains(con3));
    ASSERT_TRUE(lost.contains(con4));

    lost = GtProcessDock::lostPropertyConnections(calc4);
    ASSERT_FALSE(lost.isEmpty());
    ASSERT_EQ(lost.size(), 3);
    ASSERT_TRUE(lost.contains(con3));
    ASSERT_TRUE(lost.contains(con4));
    ASSERT_TRUE(lost.contains(con5));

    lost = GtProcessDock::lostPropertyConnections(calc);
    ASSERT_TRUE(lost.isEmpty());

    lost = GtProcessDock::lostPropertyConnections(comp);
    ASSERT_TRUE(lost.isEmpty());

    /// ALL RELATED CONNECTIONS
    QList<GtPropertyConnection*> related =
        GtProcessDock::relatedPropertyConnections(loop);

    ASSERT_FALSE(related.isEmpty());
    ASSERT_EQ(related.size(), 4);
    ASSERT_TRUE(related.contains(con2));
    ASSERT_TRUE(related.contains(con3));
    ASSERT_TRUE(related.contains(con4));
    ASSERT_TRUE(related.contains(con5));

    related = GtProcessDock::relatedPropertyConnections(calc1);

    ASSERT_FALSE(related.isEmpty());
    ASSERT_TRUE(related.size() == 1);
    ASSERT_TRUE(related.contains(con1));

    related = GtProcessDock::relatedPropertyConnections(calc2);

    ASSERT_FALSE(related.isEmpty());
    ASSERT_TRUE(related.size() == 2);
    ASSERT_TRUE(related.contains(con1));
    ASSERT_TRUE(related.contains(con2));

    related = GtProcessDock::relatedPropertyConnections(calc3);

    ASSERT_FALSE(related.isEmpty());
    ASSERT_TRUE(related.size() == 3);
    ASSERT_TRUE(related.contains(con2));
    ASSERT_TRUE(related.contains(con3));
    ASSERT_TRUE(related.contains(con4));

    related = GtProcessDock::relatedPropertyConnections(calc4);

    ASSERT_FALSE(related.isEmpty());
    ASSERT_TRUE(related.size() == 3);
    ASSERT_TRUE(related.contains(con3));
    ASSERT_TRUE(related.contains(con4));
    ASSERT_TRUE(related.contains(con5));

    related = GtProcessDock::relatedPropertyConnections(calc);
    ASSERT_TRUE(related.isEmpty());

    related = GtProcessDock::relatedPropertyConnections(task);
    ASSERT_TRUE(related.isEmpty());

    related = GtProcessDock::relatedPropertyConnections(comp);
    ASSERT_TRUE(related.isEmpty());
}

TEST_F(TestGtProcessDock, highestParentTask)
{
    task->appendChild(calc1);
    task->appendChild(calc2);
    task->appendChild(loop);
    loop->appendChild(calc3);
    loop->appendChild(loop2);
    loop2->appendChild(calc4);
    loop2->appendChild(loop3);

    GtTask* highestParent = GtProcessDock::highestParentTask(loop3);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(loop2);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(loop);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(task);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(calc4);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(calc3);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(calc2);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    highestParent = GtProcessDock::highestParentTask(calc1);

    ASSERT_TRUE(highestParent != Q_NULLPTR);
    ASSERT_TRUE(highestParent == task);

    GtCalculator* calc = Q_NULLPTR;
    GtTask* task = Q_NULLPTR;
    GtProcessComponent* comp = Q_NULLPTR;

    highestParent = GtProcessDock::highestParentTask(calc);
    ASSERT_TRUE(highestParent == Q_NULLPTR);

    highestParent = GtProcessDock::highestParentTask(task);
    ASSERT_TRUE(highestParent == Q_NULLPTR);

    highestParent = GtProcessDock::highestParentTask(comp);
    ASSERT_TRUE(highestParent == Q_NULLPTR);
}

TEST_F(TestGtProcessDock, findEquivalentObject)
{
    ASSERT_TRUE(task->appendChild(calc1));
    ASSERT_TRUE(task->appendChild(calc2));
    ASSERT_TRUE(task->appendChild(loop));
    ASSERT_TRUE(loop->appendChild(calc3));
    ASSERT_TRUE(loop->appendChild(loop2));
    ASSERT_TRUE(loop2->appendChild(calc4));
    ASSERT_TRUE(loop2->appendChild(loop3));

    GtTask* taskCopy = qobject_cast<GtTask*>(task->copy());

    ASSERT_EQ(task->findChildren<GtCalculator*>().size(), 4);
    ASSERT_EQ(task->findChildren<GtLoop*>().size(), 3);

    ASSERT_TRUE(taskCopy != Q_NULLPTR);
    ASSERT_EQ(taskCopy->findChildren<GtCalculator*>().size(), 4);
    ASSERT_EQ(taskCopy->findChildren<GtLoop*>().size(), 3);

    GtObject* calc1Copy = GtProcessDock::findEquivalentObject(taskCopy, calc1);
    GtObject* calc2Copy = GtProcessDock::findEquivalentObject(taskCopy, calc2);
    GtObject* calc3Copy = GtProcessDock::findEquivalentObject(taskCopy, calc3);
    GtObject* calc4Copy = GtProcessDock::findEquivalentObject(taskCopy, calc4);
    GtObject* loop1Copy = GtProcessDock::findEquivalentObject(taskCopy, loop);
    GtObject* loop2Copy = GtProcessDock::findEquivalentObject(taskCopy, loop2);
    GtObject* loop3Copy = GtProcessDock::findEquivalentObject(taskCopy, loop3);
    GtObject* null1 = GtProcessDock::findEquivalentObject(taskCopy, Q_NULLPTR);
    GtObject* null2 = GtProcessDock::findEquivalentObject(Q_NULLPTR, loop3);
    GtObject* null3 = GtProcessDock::findEquivalentObject(Q_NULLPTR, Q_NULLPTR);

    ASSERT_TRUE(calc1Copy != Q_NULLPTR);
    ASSERT_TRUE(calc2Copy != Q_NULLPTR);
    ASSERT_TRUE(calc3Copy != Q_NULLPTR);
    ASSERT_TRUE(calc4Copy != Q_NULLPTR);
    ASSERT_TRUE(loop1Copy != Q_NULLPTR);
    ASSERT_TRUE(loop2Copy != Q_NULLPTR);
    ASSERT_TRUE(loop3Copy != Q_NULLPTR);
    ASSERT_TRUE(null1 == Q_NULLPTR);
    ASSERT_TRUE(null2 == Q_NULLPTR);
    ASSERT_TRUE(null3 == Q_NULLPTR);

    ASSERT_TRUE(taskCopy->findDirectChildren<TestGtCalculator*>().first()
                == calc1Copy);

    ASSERT_TRUE(taskCopy->findDirectChildren<TestGtCalculator*>().last()
                == calc2Copy);

    ASSERT_TRUE(taskCopy->findDirectChildren<GtLoop*>().first()
                == loop1Copy);

    ASSERT_TRUE(loop1Copy->findDirectChildren<TestGtCalculator*>().first()
                == calc3Copy);

    ASSERT_TRUE(loop1Copy->findDirectChildren<GtLoop*>().first()
                == loop2Copy);

    ASSERT_TRUE(loop2Copy->findDirectChildren<TestGtCalculator*>().first()
                == calc4Copy);

    ASSERT_TRUE(loop2Copy->findDirectChildren<GtLoop*>().first()
                == loop3Copy);

    delete taskCopy;
}

TEST_F(TestGtProcessDock, updateConnectionProperties)
{
    task->appendChild(calc1);
    task->appendChild(calc2);
    task->appendChild(loop);
    loop->appendChild(calc3);
    loop->appendChild(loop2);
    loop2->appendChild(calc4);
    loop2->appendChild(loop3);

    task->appendChild(con1);
    task->appendChild(con2);
    task->appendChild(con3);
    task->appendChild(con4);

    con1->setSourceUuid(calc1->uuid());
    con1->setTargetUuid(calc2->uuid());
    con1->setSourceProp(calc1->prop1()->ident());
    con1->setTargetProp(calc2->prop1()->ident());

    con2->setSourceUuid(calc2->uuid());
    con2->setTargetUuid(calc3->uuid());
    con2->setSourceProp(calc2->prop2()->ident());
    con2->setTargetProp(calc3->prop1()->ident());

    con3->setSourceUuid(calc3->uuid());
    con3->setTargetUuid(calc4->uuid());
    con3->setSourceProp(calc3->prop2()->ident());
    con3->setTargetProp(calc4->prop1()->ident());

    con4->setSourceUuid(calc3->uuid());
    con4->setTargetUuid(calc4->uuid());
    con4->setSourceProp(calc3->prop3()->ident());
    con4->setTargetProp(calc4->prop2()->ident());

    GtTask* taskCopy = qobject_cast<GtTask*>(task->copy());

    ASSERT_TRUE(taskCopy != Q_NULLPTR);

    QList<GtPropertyConnection*> origs =
            task->findDirectChildren<GtPropertyConnection*>();

    QList<GtPropertyConnection*> copies =
            taskCopy->findDirectChildren<GtPropertyConnection*>();

    ASSERT_EQ(origs.size(), copies.size());
    ASSERT_EQ(origs.size(), 4);
    ASSERT_EQ(copies.size(), 4);

    foreach(GtPropertyConnection* propCon, origs)
    {
        GtPropertyConnection* conCopy =
                GtProcessDock::findConnectionCopy(propCon, copies);

        ASSERT_TRUE(conCopy != Q_NULLPTR);

        ASSERT_TRUE(GtProcessDock::updateConnectionProperties(propCon, conCopy,
                                                              task, taskCopy));

    }

    TestGtCalculator* calc1Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc1));

    TestGtCalculator* calc2Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc2));

    TestGtCalculator* calc3Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc3));

    TestGtCalculator* calc4Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc4));

    ASSERT_TRUE(calc1Copy != Q_NULLPTR);
    ASSERT_TRUE(calc2Copy != Q_NULLPTR);
    ASSERT_TRUE(calc3Copy != Q_NULLPTR);
    ASSERT_TRUE(calc4Copy != Q_NULLPTR);

    ASSERT_TRUE(copies.first()->sourceUuid() == calc1Copy->uuid());
    ASSERT_TRUE(copies.first()->targetUuid() == calc2Copy->uuid());
    ASSERT_TRUE(copies.first()->sourceProp() == calc1Copy->prop1()->ident());
    ASSERT_TRUE(copies.first()->targetProp() == calc2Copy->prop1()->ident());

    ASSERT_TRUE(copies.at(1)->sourceUuid() == calc2Copy->uuid());
    ASSERT_TRUE(copies.at(1)->targetUuid() == calc3Copy->uuid());
    ASSERT_TRUE(copies.at(1)->sourceProp() == calc2Copy->prop2()->ident());
    ASSERT_TRUE(copies.at(1)->targetProp() == calc3Copy->prop1()->ident());

    ASSERT_TRUE(copies.at(2)->sourceUuid() == calc3Copy->uuid());
    ASSERT_TRUE(copies.at(2)->targetUuid() == calc4Copy->uuid());
    ASSERT_TRUE(copies.at(2)->sourceProp() == calc3Copy->prop2()->ident());
    ASSERT_TRUE(copies.at(2)->targetProp() == calc4Copy->prop1()->ident());

    ASSERT_TRUE(copies.last()->sourceUuid() == calc3Copy->uuid());
    ASSERT_TRUE(copies.last()->targetUuid() == calc4Copy->uuid());
    ASSERT_TRUE(copies.last()->sourceProp() == calc3Copy->prop3()->ident());
    ASSERT_TRUE(copies.last()->targetProp() == calc4Copy->prop2()->ident());
}

TEST_F(TestGtProcessDock, updateRelativeObjectLinks)
{
    task->appendChild(calc1);
    task->appendChild(calc2);
    task->appendChild(loop);
    loop->appendChild(calc3);
    loop->appendChild(loop2);
    loop2->appendChild(calc4);
    loop2->appendChild(loop3);

    task->appendChild(con1);
    task->appendChild(con2);
    task->appendChild(con3);
    task->appendChild(con4);

    calc1->relLink()->setVal(calc3->uuid());
    calc2->relLink()->setVal(calc4->uuid());
    calc3->relLink()->setVal(calc1->uuid());
    calc4->relLink()->setVal(calc2->uuid());

    GtTask* taskCopy = qobject_cast<GtTask*>(task->copy());

    ASSERT_TRUE(taskCopy != Q_NULLPTR);

    TestGtCalculator* calc1Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc1));

    TestGtCalculator* calc2Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc2));

    TestGtCalculator* calc3Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc3));

    TestGtCalculator* calc4Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc4));

    ASSERT_TRUE(calc1Copy != Q_NULLPTR);
    ASSERT_TRUE(calc2Copy != Q_NULLPTR);
    ASSERT_TRUE(calc3Copy != Q_NULLPTR);
    ASSERT_TRUE(calc4Copy != Q_NULLPTR);

    ASSERT_FALSE(GtProcessDock::updateRelativeObjectLinks(Q_NULLPTR, taskCopy));
    ASSERT_FALSE(GtProcessDock::updateRelativeObjectLinks(task, Q_NULLPTR));
    ASSERT_FALSE(GtProcessDock::updateRelativeObjectLinks(Q_NULLPTR,
                                                          Q_NULLPTR));
    ASSERT_TRUE(GtProcessDock::updateRelativeObjectLinks(task, taskCopy));

    ASSERT_EQ(calc1Copy->relLink()->getVal(), calc3Copy->uuid());
    ASSERT_EQ(calc2Copy->relLink()->getVal(), calc4Copy->uuid());
    ASSERT_EQ(calc3Copy->relLink()->getVal(), calc1Copy->uuid());
    ASSERT_EQ(calc4Copy->relLink()->getVal(), calc2Copy->uuid());

    calc1Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc1));

    calc2Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc2));

    calc3Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc3));

    calc4Copy = qobject_cast<TestGtCalculator*>(
                GtProcessDock::findEquivalentObject(taskCopy, calc4));

    ASSERT_TRUE(calc1Copy == Q_NULLPTR);
    ASSERT_TRUE(calc2Copy == Q_NULLPTR);
    ASSERT_TRUE(calc3Copy == Q_NULLPTR);
    ASSERT_TRUE(calc4Copy == Q_NULLPTR);
}
