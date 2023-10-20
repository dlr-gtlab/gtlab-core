/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_task
 *
 *  Created on: 28.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include <QList>

#include "gtest/gtest.h"
#include "gt_task.h"
#include "gt_calculator.h"

class TestCalculator : public GtCalculator
{
public:
    TestCalculator()
    {

    }
};

class TestGtTask : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
//        obj.setObjectName("MyTask");

//        obj.appendChild(&calc1);
//        obj.appendChild(&calc2);
    }

    virtual void TearDown()
    {
    }

//    GtTask obj;
//    TestCalculator calc1;
//    TestCalculator calc2;
};

//TEST_F(TestGtTask, calculators)
//{
//    ASSERT_EQ(2, obj.calculators().size());
//}
