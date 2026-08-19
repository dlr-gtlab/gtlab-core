/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_processexecutioninfo.cpp
 */

#include "gtest/gtest.h"
#include <QTemporaryDir>
#include <memory>
#include "gt_coreprocessexecutor.h"
#include "gt_objectgroup.h"
#include "gt_objectmementodiff.h"
#include "gt_objectfactory.h"
#include "gt_project.h"

#include "test_gt_processexecutioninfo.h"



class TestGtProcessExecutionInfo : public ::testing::Test
{
public:
    TestGtProcessExecutionInfo()
    {
        gtObjectFactory->registerClass(TestObjectDouble::staticMetaObject);
        gtObjectFactory->registerClass(TestTask1::staticMetaObject);
        gtObjectFactory->registerClass(TestWarningTask1::staticMetaObject);
        gtObjectFactory->registerClass(TestFailingTask1::staticMetaObject);
        gtObjectFactory->registerClass(TestFailingTask2::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorAPlusB::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorWriteToObject::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorFailing::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorWarning::staticMetaObject);
    }

protected:
    void SetUp() override
    {
        source = new GtObject;
        source->setObjectName("ROOT");
    }

    void TearDown() override
    {
        delete source;
    }


protected:
    GtCoreProcessExecutor executor;
    GtObject* source;

};



TEST_F(TestGtProcessExecutionInfo, storeAplusBToObject)
{
    double val_x_ini = 1.1;
    double val_a = 1.2;
    double val_b = 2.3;
    double val_x_new = val_a+val_b;

    TestObjectDouble obj;
    obj.setObjectName("FOO");
    obj.setX(val_x_ini);
    source->appendChild(&obj);

    auto task = std::make_unique<TestTask1>();
    task->setFactory(gtObjectFactory);
    task->setInput(val_a, val_b, &obj);

    auto execinfo = new GtProcessExecutionInfo;
    executor.setSource(source);
    executor.runTask(task.get(), execinfo);

    // qDebug() << "info:";
    // qDebug() << "|-> queuedTime:" << execinfo->queuedTime();
    // qDebug() << "|-> startTime:" << execinfo->startTime();
    // qDebug() << "|-> endTime:" << execinfo->endTime();
    // qDebug() << "|-> processState:" << execinfo->processState();
    // qDebug() << "|-> dataDiffToMerge:" << execinfo->dataDiffToMerge().toByteArray();


    auto diff = execinfo->dataDiffToMerge();
    QDomElement root = diff.documentElement();


    // Check state of task
    ASSERT_EQ(execinfo->processState(), GtProcessComponent::FINISHED);
    ASSERT_EQ(diff.numberOfDiffSteps(), 1);

    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj.metaObject()->className());


    // property diff check
    QDomElement propChange = root.firstChildElement("diff-property-change");
    ASSERT_FALSE(propChange.isNull());

    QString propName = propChange.attribute("name");
    QString propType = propChange.attribute("type");

    ASSERT_STREQ(propName.toStdString().c_str(), "x");
    ASSERT_STREQ(propType.toStdString().c_str(), "double");

    QDomElement oldValue = propChange.firstChildElement("oldVal");
    QDomElement newValue = propChange.firstChildElement("newVal");

    ASSERT_FALSE(oldValue.isNull());
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(newValue.text().toStdString().c_str(),  QString::number(val_x_new).toStdString().c_str());
    ASSERT_STREQ(oldValue.text().toStdString().c_str(),  QString::number(val_x_ini).toStdString().c_str());

}


TEST_F(TestGtProcessExecutionInfo, warningTask1)
{
    auto task = std::make_unique<TestWarningTask1>();
    task->setFactory(gtObjectFactory);

    auto execinfo = new GtProcessExecutionInfo;
    executor.setSource(source);
    executor.runTask(task.get(), execinfo);

    auto diff = execinfo->dataDiffToMerge();
    ASSERT_EQ(execinfo->processState(), GtProcessComponent::WARN_FINISHED);
    ASSERT_TRUE(diff.isNull());
    ASSERT_EQ(diff.numberOfDiffSteps(), 0);
}


TEST_F(TestGtProcessExecutionInfo, failingTask1)
{
    auto task = std::make_unique<TestFailingTask1>();
    task->setFactory(gtObjectFactory);

    auto execinfo = new GtProcessExecutionInfo;
    executor.setSource(source);
    executor.runTask(task.get(), execinfo);

    auto diff = execinfo->dataDiffToMerge();
    ASSERT_EQ(execinfo->processState(), GtProcessComponent::FAILED);
    ASSERT_TRUE(diff.isNull());
    ASSERT_EQ(diff.numberOfDiffSteps(), 0);
}

TEST_F(TestGtProcessExecutionInfo, failingTask2)
{
    TestObjectDouble obj;
    obj.setObjectName("FOO");
    obj.setX(1.1);
    source->appendChild(&obj);

    auto task = std::make_unique<TestFailingTask2>();
    task->setFactory(gtObjectFactory);
    task->setInput(2.2, &obj);

    auto execinfo = new GtProcessExecutionInfo;
    executor.setSource(source);
    executor.runTask(task.get(), execinfo);

    auto diff = execinfo->dataDiffToMerge();
    ASSERT_EQ(execinfo->processState(), GtProcessComponent::FAILED);
    ASSERT_TRUE(diff.isNull());
    ASSERT_EQ(diff.numberOfDiffSteps(), 0);
}

