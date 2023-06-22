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

#include "gt_stringmonitoringproperty.h"

/// This is a test fixture that does a init for each test
class TestGtStringMonitoringProperty : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }
    void TearDown() override
    {

    }
};

TEST_F(TestGtStringMonitoringProperty, initialization)
{
    GtStringMonitoringProperty prop("testIdent", "testName");

    ASSERT_STREQ(prop.objectName().toStdString().c_str(), "testName");

    ASSERT_STREQ(prop.ident().toStdString().c_str(), "testIdent");

    ASSERT_STREQ(prop.brief().toStdString().c_str(), "");

    GtStringMonitoringProperty prop2("testIdent2", "testName2", "testBrief2");

    ASSERT_STREQ(prop2.objectName().toStdString().c_str(), "testName2");

    ASSERT_STREQ(prop2.ident().toStdString().c_str(), "testIdent2");

    ASSERT_STREQ(prop2.brief().toStdString().c_str(), "testBrief2");
}

TEST_F(TestGtStringMonitoringProperty, isReadOnly)
{
    GtStringMonitoringProperty prop("testIdent", "testName");

    ASSERT_TRUE(prop.isReadOnly());
}

TEST_F(TestGtStringMonitoringProperty, setVal)
{
    GtStringMonitoringProperty prop("testIdent", "testName");

    prop.setVal("11.3");

    ASSERT_STREQ(prop.getVal().toStdString().c_str(), "11.3");

    prop = "0.75";

    ASSERT_STREQ(prop.getVal().toStdString().c_str(), "0.75");
}
