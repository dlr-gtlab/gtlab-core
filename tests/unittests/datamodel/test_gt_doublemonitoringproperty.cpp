/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.01.2023
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_doubleproperty.h"

/// This is a test fixture that does a init for each test
class TestGtDoubleMonitoringProperty : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }
    void TearDown() override
    {

    }
};

TEST_F(TestGtDoubleMonitoringProperty, initialization)
{
    GtDoubleProperty prop("testIdent", "testName");
    prop.setMonitoring(true);

    ASSERT_STREQ(prop.objectName().toStdString().c_str(), "testName");

    ASSERT_STREQ(prop.ident().toStdString().c_str(), "testIdent");

    ASSERT_STREQ(prop.brief().toStdString().c_str(), "");

    GtDoubleProperty prop2("testIdent2", "testName2", "testBrief2");
    prop2.setMonitoring(true);

    ASSERT_STREQ(prop2.objectName().toStdString().c_str(), "testName2");

    ASSERT_STREQ(prop2.ident().toStdString().c_str(), "testIdent2");

    ASSERT_STREQ(prop2.brief().toStdString().c_str(), "testBrief2");
}

TEST_F(TestGtDoubleMonitoringProperty, isReadOnly)
{
    GtDoubleProperty prop("testIdent", "testName");
    prop.setMonitoring(true);

    ASSERT_TRUE(prop.isReadOnly());
}

TEST_F(TestGtDoubleMonitoringProperty, setVal)
{
    GtDoubleProperty prop("testIdent", "testName");
    prop.setMonitoring(true);

    prop.setVal(11.3);

    ASSERT_DOUBLE_EQ(prop.getVal(), 11.3);

    prop = 0.75;

    ASSERT_DOUBLE_EQ(prop.getVal(), 0.75);
}
