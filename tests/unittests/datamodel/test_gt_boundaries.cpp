/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * Source File: test_gt_boundaries
 *
 *  Created on: 06.11.2025
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef TEST_GT_BOUNDARIES_H
#define TEST_GT_BOUNDARIES_H

#include "gtest/gtest.h"

#include "gt_boundaries.h"

using namespace gt;

class TestGtBoundaries : public ::testing::Test
{
protected:
    using IntBound = Boundaries<int>;
    using DoubleBound = Boundaries<double>;

    virtual void SetUp()
    {

    }
    virtual void TearDown()
    {

    }
};

TEST_F(TestGtBoundaries, MakeLower)
{
    auto b = IntBound::makeLower(5);
    EXPECT_TRUE(b.has_low());
    EXPECT_FALSE(b.has_high());
    EXPECT_EQ(b.low(), 5);
}

TEST_F(TestGtBoundaries, MakeUpper)
{
    auto b = IntBound::makeUpper(10);
    EXPECT_FALSE(b.has_low());
    EXPECT_TRUE(b.has_high());
    EXPECT_EQ(b.high(), 10);
}

TEST_F(TestGtBoundaries, MakeUnbounded)
{
    auto b = IntBound::makeUnbounded();
    EXPECT_FALSE(b.has_low());
    EXPECT_FALSE(b.has_high());
}

TEST_F(TestGtBoundaries, MakeNormalized)
{
    auto b = IntBound::makeNormalized(10, 5);
    EXPECT_EQ(*b.low(), 5);
    EXPECT_EQ(*b.high(), 10);

    auto b2 = IntBound::makeNormalized(3, 7);
    EXPECT_EQ(*b2.low(), 3);
    EXPECT_EQ(*b2.high(), 7);
}

// ---------- Clamp Function ----------

TEST_F(TestGtBoundaries, ClampValue)
{
    auto b = IntBound::makeNormalized(5, 10);
    EXPECT_EQ(b.clamp(3), 5);   // unterhalb
    EXPECT_EQ(b.clamp(7), 7);   // innerhalb
    EXPECT_EQ(b.clamp(12), 10); // oberhalb
}

TEST_F(TestGtBoundaries, ClampUnbounded)
{
    auto b = IntBound::makeLower(5);
    EXPECT_EQ(b.clamp(3), 5);
    EXPECT_EQ(b.clamp(10), 10);

    auto b2 = IntBound::makeUpper(10);
    EXPECT_EQ(b2.clamp(15), 10);
    EXPECT_EQ(b2.clamp(5), 5);
}

// ---------- Contains Function ----------

TEST_F(TestGtBoundaries, Contains)
{
    auto b = IntBound::makeNormalized(5, 10);
    EXPECT_FALSE(b.contains(3));
    EXPECT_TRUE(b.contains(5));
    EXPECT_TRUE(b.contains(7));
    EXPECT_TRUE(b.contains(10));
    EXPECT_FALSE(b.contains(12));
}

// ---------- Valid Pair Checks ----------

TEST_F(TestGtBoundaries, ValidPair)
{
    gt::optional<int> lo = 5;
    gt::optional<int> hi = 10;
    EXPECT_TRUE(IntBound::valid_pair(lo, hi));
    EXPECT_FALSE(IntBound::valid_pair(10, 5));

    gt::optional<double> nanVal = std::nan("");
    EXPECT_FALSE(DoubleBound::valid_pair(nanVal, 10.0));
    EXPECT_FALSE(DoubleBound::valid_pair(5.0, nanVal));
}

// ---------- Floating Point Clamp and Contains ----------

TEST_F(TestGtBoundaries, FloatingPointClampAndContains)
{
    auto b = DoubleBound::makeNormalized(1.5, 3.5);
    EXPECT_EQ(b.clamp(1.0), 1.5);
    EXPECT_EQ(b.clamp(4.0), 3.5);
    EXPECT_TRUE(b.contains(2.0));
    EXPECT_FALSE(b.contains(4.0));
}
#endif // TEST_GT_BOUNDARIES_H
