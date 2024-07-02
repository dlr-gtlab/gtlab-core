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

#include "gt_intmonitoringproperty.h"

/// This is a test fixture that does a init for each test
class TestGtIntMonitoringProperty : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }
    void TearDown() override
    {

    }
};

TEST_F(TestGtIntMonitoringProperty, initialization)
{
    GtIntMonitoringProperty prop("testIdent", "testName");

    ASSERT_STREQ(prop.objectName().toStdString().c_str(), "testName");

    ASSERT_STREQ(prop.ident().toStdString().c_str(), "testIdent");

    // ToDo: This test is failing.
    // The GtIntProperty does behave a bit different compared to GtIntMonitoringProperty
    // or even GtDoubleProperty and GtStringpropety:
    // The GtIntProperty now used does set the brief value to the same as the name value
    // if the constructor with only two arguments is used.
    // THis difference should be discussed
    ASSERT_STREQ(prop.brief().toStdString().c_str(), "");

    GtIntMonitoringProperty prop2("testIdent2", "testName2", "testBrief2");

    ASSERT_STREQ(prop2.objectName().toStdString().c_str(), "testName2");

    ASSERT_STREQ(prop2.ident().toStdString().c_str(), "testIdent2");

    ASSERT_STREQ(prop2.brief().toStdString().c_str(), "testBrief2");
}

TEST_F(TestGtIntMonitoringProperty, isReadOnly)
{
    // ToDo: This test is failing.
    // We should discuss how the monitoring propeties should be defined
    // currently properties become monitoring properties
    // by the registration as monitoring properties
    // and also becomes read only in this procedure
    GtIntMonitoringProperty prop("testIdent", "testName");

    ASSERT_TRUE(prop.isReadOnly());
}

TEST_F(TestGtIntMonitoringProperty, setVal)
{
    GtIntMonitoringProperty prop("testIdent", "testName");

    prop.setVal(11);

    ASSERT_DOUBLE_EQ(prop.getVal(), 11);

    prop = -7;

    ASSERT_DOUBLE_EQ(prop.getVal(), -7);
}
