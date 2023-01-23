/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2023 by DLR
 *
 *  Created on: 20.01.2023
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_doublemonitoringproperty.h"

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
    GtDoubleMonitoringProperty prop("testIdent", "testName");

    ASSERT_STREQ(prop.objectName().toStdString().c_str(), "testName");

    ASSERT_STREQ(prop.ident().toStdString().c_str(), "testIdent");

    ASSERT_STREQ(prop.brief().toStdString().c_str(), "");

    GtDoubleMonitoringProperty prop2("testIdent2", "testName2", "testBrief2");

    ASSERT_STREQ(prop2.objectName().toStdString().c_str(), "testName2");

    ASSERT_STREQ(prop2.ident().toStdString().c_str(), "testIdent2");

    ASSERT_STREQ(prop2.brief().toStdString().c_str(), "testBrief2");
}

TEST_F(TestGtDoubleMonitoringProperty, isReadOnly)
{
    GtDoubleMonitoringProperty prop("testIdent", "testName");

    ASSERT_TRUE(prop.isReadOnly());
}

TEST_F(TestGtDoubleMonitoringProperty, setVal)
{
    GtDoubleMonitoringProperty prop("testIdent", "testName");

    prop.setVal(11.3);

    ASSERT_DOUBLE_EQ(prop.getVal(), 11.3);

    prop = 0.75;

    ASSERT_DOUBLE_EQ(prop.getVal(), 0.75);
}
