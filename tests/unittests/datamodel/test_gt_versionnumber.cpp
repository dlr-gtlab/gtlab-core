/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_versionnumber.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 05.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_versionnumber.h"

/// This is a test fixture that does a init for each test
class TestGtVersionNumber : public ::testing::Test
{
protected:
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {

    }

};

TEST_F(TestGtVersionNumber, toString)
{
    ASSERT_STREQ(GtVersionNumber(1, 0, 0).toString().toStdString().c_str(),
                 "1.0.0");
    ASSERT_STREQ(GtVersionNumber(1, 0, 0, "a").toString().toStdString().c_str(),
                 "1.0.0-a");
    ASSERT_STREQ(GtVersionNumber(1, 0).toString().toStdString().c_str(),
                 "1.0");
    ASSERT_STREQ(GtVersionNumber(1, 0, "a").toString().toStdString().c_str(),
                 "1.0-a");
    ASSERT_STREQ(GtVersionNumber(1).toString().toStdString().c_str(),
                 "1");
    ASSERT_STREQ(GtVersionNumber(1, "a").toString().toStdString().c_str(),
                 "1-a");

    ASSERT_STREQ(GtVersionNumber("1.0.0").toString().toStdString().c_str(),
                 "1.0.0");
    ASSERT_STREQ(GtVersionNumber("1.0.0-a").toString().toStdString().c_str(),
                 "1.0.0-a");
    ASSERT_STREQ(GtVersionNumber("1.0").toString().toStdString().c_str(),
                 "1.0");
    ASSERT_STREQ(GtVersionNumber("1.0-a").toString().toStdString().c_str(),
                 "1.0-a");
    ASSERT_STREQ(GtVersionNumber("1").toString().toStdString().c_str(),
                 "1");
    ASSERT_STREQ(GtVersionNumber("1-a").toString().toStdString().c_str(),
                 "1-a");
}

TEST_F(TestGtVersionNumber, isNull)
{
    GtVersionNumber v(1, 0, 0);

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber(1, 0, 0, "a");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber();

    ASSERT_TRUE(v.isNull());

    v = GtVersionNumber("a");

    ASSERT_TRUE(v.isNull());

    v = GtVersionNumber("1");

    ASSERT_FALSE(v.isNull());

    v = GtVersionNumber("1.3.12-a");

    ASSERT_FALSE(v.isNull());
}

TEST_F(TestGtVersionNumber, compare)
{
    GtVersionNumber v1(1, 0, 0);
    GtVersionNumber v2(1, 0, 0);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), 0);

    v1 = GtVersionNumber(2, 0, 1);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), 1);

    v1 = GtVersionNumber(0, 0, 1);

    ASSERT_EQ(GtVersionNumber::compare(v1, v2), -1);
}

TEST_F(TestGtVersionNumber, operators)
{
    GtVersionNumber v1(1, 0, 0);
    GtVersionNumber v2(1, 0, 0);

    ASSERT_TRUE(v1 == v2);
    ASSERT_FALSE(v1 != v2);
    ASSERT_TRUE(v1 >= v2);
    ASSERT_TRUE(v1 <= v2);
    ASSERT_FALSE(v1 > v2);
    ASSERT_FALSE(v1 < v2);

    v1 = GtVersionNumber(2, 0, 0);

    ASSERT_FALSE(v1 == v2);
    ASSERT_TRUE(v1 != v2);
    ASSERT_TRUE(v1 >= v2);
    ASSERT_FALSE(v1 <= v2);
    ASSERT_TRUE(v1 > v2);
    ASSERT_FALSE(v1 < v2);

    v1 = GtVersionNumber(0, 1, 0);

    ASSERT_FALSE(v1 == v2);
    ASSERT_TRUE(v1 != v2);
    ASSERT_FALSE(v1 >= v2);
    ASSERT_TRUE(v1 <= v2);
    ASSERT_FALSE(v1 > v2);
    ASSERT_TRUE(v1 < v2);
}
