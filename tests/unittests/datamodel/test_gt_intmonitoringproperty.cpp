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
    GT_SUPPRESS_DEPRECATED_BEGIN
    GtIntMonitoringProperty prop("testIdent", "testName");

    ASSERT_STREQ(prop.objectName().toStdString().c_str(), "testName");

    ASSERT_STREQ(prop.ident().toStdString().c_str(), "testIdent");

    ASSERT_STREQ(prop.brief().toStdString().c_str(), "");

    GtIntMonitoringProperty prop2("testIdent2", "testName2", "testBrief2");

    ASSERT_STREQ(prop2.objectName().toStdString().c_str(), "testName2");

    ASSERT_STREQ(prop2.ident().toStdString().c_str(), "testIdent2");

    ASSERT_STREQ(prop2.brief().toStdString().c_str(), "testBrief2");
    GT_SUPPRESS_DEPRECATED_END
}

TEST_F(TestGtIntMonitoringProperty, isReadOnly)
{
    GT_SUPPRESS_DEPRECATED_BEGIN
    GtIntMonitoringProperty prop("testIdent", "testName");

    ASSERT_TRUE(prop.isReadOnly());
    GT_SUPPRESS_DEPRECATED_END
}

TEST_F(TestGtIntMonitoringProperty, setVal)
{
    GT_SUPPRESS_DEPRECATED_BEGIN
    GtIntMonitoringProperty prop("testIdent", "testName");

    prop.setVal(11);

    ASSERT_DOUBLE_EQ(prop.getVal(), 11);

    prop = -7;

    ASSERT_DOUBLE_EQ(prop.getVal(), -7);
    GT_SUPPRESS_DEPRECATED_END
}
