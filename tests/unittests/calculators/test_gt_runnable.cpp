/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_runnable
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gtest/gtest.h"
#include "gt_runnable.h"
#include "gt_calculator.h"

class TestGtRunnable : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        obj.setObjectName("MyRunnable");
    }

    virtual void TearDown()
    {
    }

    GtRunnable obj;
};

//class TestCalculator : public GtCalculator
//{
//public:
//    TestCalculator()
//    {

//    }

//};

//TEST_F(TestGtRunnable, appendCalculator)
//{
//    TestCalculator myCalc;

////    ASSERT_TRUE(obj.appendCalculator(&myCalc));
////    ASSERT_FALSE(obj.appendCalculator(nullptr));
////    ASSERT_FALSE(obj.appendCalculator(&myCalc));
//}
