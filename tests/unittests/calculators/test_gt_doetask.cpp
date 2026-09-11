// /* GTlab - Gas Turbine laboratory
//  *
//  * SPDX-License-Identifier: MPL-2.0+
//  * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
//  * Source File: test_gt_doetask.cpp
//  */

// #include "gtest/gtest.h"

// #include <QDir>
// #include <QMetaObject>

// #include "core/test_gt_processtestclasses.h"

// #include "gt_doetask.h"
// #include "gt_runnable.h"
// #include "gt_doubleproperty.h"
// #include "gt_object.h"
// #include "gt_objectmemento.h"
// #include "gt_objectlinkproperty.h"
// #include "gt_objectpathproperty.h"
// #include "gt_task.h"
// #include "gt_calculator.h"

// class TestDOERunnable : public GtRunnable
// {
// public:
//     void run() override
//     {
//     }
// };

// class SimpleCalculator : public GtCalculator
// {
// public:
//     SimpleCalculator()
//     {
//         m_valueProp = new GtDoubleProperty("value", "Value");
//         registerProperty(*m_valueProp);
//     }

//     ~SimpleCalculator() override
//     {
//         delete m_valueProp;
//     }

//     bool run() override
//     {
//         m_valueProp->setVal(m_valueProp->get() + 1.0);
//         return true;
//     }

//     GtDoubleProperty* valueProp()
//     {
//         return m_valueProp;
//     }

// private:
//     GtDoubleProperty* m_valueProp;
// };

// TEST(DOETaskTest, BasicParallelExecution)
// {
//     TestDOERunnable* runnable = new TestDOERunnable();
//     runnable->setAutoDelete(false);

//     GtObject* obj = new GtObject();
//     obj->setUuid("test-uuid-001");
//     runnable->addLinkedObject(obj);

//     GtObjectMemento memento(obj, true);
//     runnable->appendSourceData(memento);

//     GtDOETask task;
//     task.publicSetRunnable(runnable);

//     SimpleCalculator* calc1 = new SimpleCalculator();
//     calc1->setExecutionLabel("Calculator 1");
//     task.appendChild(calc1);

//     SimpleCalculator* calc2 = new SimpleCalculator();
//     calc2->setExecutionLabel("Calculator 2");
//     task.appendChild(calc2);

//     EXPECT_EQ(task.processComponents().count(), 2);

//     bool result = task.runIteration();

//     EXPECT_TRUE(result);

//     EXPECT_DOUBLE_EQ(calc1->valueProp()->get(), 1.0);
//     EXPECT_DOUBLE_EQ(calc2->valueProp()->get(), 1.0);

//     EXPECT_EQ(task.currentIterationStep(), 1);
// }

// TEST(DOETaskTest, DOEContextTracking)
// {
//     TestDOERunnable* runnable = new TestDOERunnable();
//     runnable->setAutoDelete(false);

//     GtObject* obj = new GtObject();
//     obj->setUuid("test-uuid-002");
//     runnable->addLinkedObject(obj);

//     GtObjectMemento memento(obj, true);
//     runnable->appendSourceData(memento);

//     GtDOETask task;
//     task.publicSetRunnable(runnable);

//     SimpleCalculator* calc1 = new SimpleCalculator();
//     calc1->setExecutionLabel("Calculator 1");
//     task.appendChild(calc1);

//     SimpleCalculator* calc2 = new SimpleCalculator();
//     calc2->setExecutionLabel("Calculator 2");
//     task.appendChild(calc2);

//     bool result = task.runIteration();

//     EXPECT_TRUE(result);

//     EXPECT_EQ(calc1->doeContext().totalRuns, 2);
//     EXPECT_EQ(calc1->doeContext().runIndex, 0);
//     EXPECT_EQ(calc1->doeContext().iteration, 1);

//     EXPECT_EQ(calc2->doeContext().totalRuns, 2);
//     EXPECT_EQ(calc2->doeContext().runIndex, 1);
//     EXPECT_EQ(calc2->doeContext().iteration, 1);

//     EXPECT_EQ(calc1->doeContext().runId, "iter-1-run-0");
//     EXPECT_EQ(calc2->doeContext().runId, "iter-1-run-1");
// }

// TEST(DOETaskTest, MultipleIterations)
// {
//     TestDOERunnable* runnable = new TestDOERunnable();
//     runnable->setAutoDelete(false);

//     GtObject* obj = new GtObject();
//     obj->setUuid("test-uuid-003");
//     runnable->addLinkedObject(obj);

//     GtObjectMemento memento(obj, true);
//     runnable->appendSourceData(memento);

//     GtDOETask task;
//     task.publicSetRunnable(runnable);
//     task.setMaxIteration(3);

//     SimpleCalculator* calc = new SimpleCalculator();
//     calc->setExecutionLabel("Calculator");
//     task.appendChild(calc);

//     bool result = task.runIteration();

//     EXPECT_TRUE(result);
//     EXPECT_EQ(task.currentIterationStep(), 3);

//     EXPECT_DOUBLE_EQ(calc->valueProp()->get(), 3.0);
// }
