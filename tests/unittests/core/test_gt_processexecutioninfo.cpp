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
        gtObjectFactory->registerClass(TestPackage::staticMetaObject);
        gtObjectFactory->registerClass(GtPackage::staticMetaObject);
        gtObjectFactory->registerClass(TestObjectDouble::staticMetaObject);
        gtObjectFactory->registerClass(TestTask1::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorAPlusB::staticMetaObject);
        gtObjectFactory->registerClass(TestCalculatorWriteToObject::staticMetaObject);
    }

protected:
    void SetUp() override
    {

    }

    static std::unique_ptr<GtObjectGroup> makeObjectGroup()
    {
        QTemporaryDir tempDir;
        auto proj = std::make_unique<GtObjectGroup>();
        //auto pkg = new TestPackage;
        //pkg->setObjectName("TestPackage");
        //proj->appendChild(pkg);
        return proj;
    }

protected:
    GtCoreProcessExecutor executor;
    GtObject source;

};



TEST_F(TestGtProcessExecutionInfo, storeAplusBToObject)
{

    auto obj = new TestObjectDouble;
    obj->setObjectName("FOO");
    obj->setX(1.1);

    auto objgrp = makeObjectGroup();
    //TestPackage* pkg = objgrp->findDirectChild<TestPackage*>("TestPackage");

    objgrp->appendChild(obj);

/*
    qDebug() << "Source children:";
    for(auto c : source.findDirectChildren())
    {
        qDebug() <<  "|->" << c << "uuid:" << c->uuid();
    }*/

    auto task = std::make_unique<TestTask1>();
    task->setFactory(gtObjectFactory);
    task->setInput(1.2, 2.3, obj);



    auto execinfo = new GtProcessExecutionInfo;
    executor.setSource(objgrp.get());
    executor.runTask(task.get(), execinfo);

    qDebug() << "info:";
    qDebug() << "|-> queuedTime:" << execinfo->queuedTime();
    qDebug() << "|-> startTime:" << execinfo->startTime();
    qDebug() << "|-> endTime:" << execinfo->endTime();
    qDebug() << "|-> processState:" << execinfo->processState();
    qDebug() << "|-> dataDiffToMerge:" << execinfo->dataDiffToMerge().toByteArray();

}




