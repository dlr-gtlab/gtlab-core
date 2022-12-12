/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2012 by DLR
 *
 *  Created on: 08.12.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_objectmodel.h"
#include "gt_processcomponentmodel.h"
#include "gt_processfiltermodel.h"

#include "test_gt_testcalculator.h"

#include "gt_loop.h"



/// This is a test fixture that does a init for each test
class TestGtProcessFilterModel : public ::testing::Test
{
};

TEST_F(TestGtProcessFilterModel, entryCheck)
{
    /// Setup
    GtLoop task;
    auto* calc = new TestCalculator;
    task.appendChild(calc);

    auto* calc2 = new TestCalculator;
    task.appendChild(calc2);

    GtObjectModel calcModel;
    calcModel.setRootObject(&task);

    GtProcessComponentModel styledModel;
    styledModel.setSourceModel(&calcModel);

    GtProcessFilterModel filterModel;
    filterModel.setSourceModel(&styledModel);

    /// check if there is one item in the top level
    ASSERT_EQ(filterModel.rowCount(), 1);

    QModelIndex taskIndex = filterModel.index(0, 0);

    /// check if there are the two child elements on the second level
    ASSERT_EQ(filterModel.rowCount(taskIndex), 2);
}
