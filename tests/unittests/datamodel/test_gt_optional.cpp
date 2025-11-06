/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * Source File: test_gt_optional
 *
 *  Created on: 06.11.2025
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef TEST_GT_OPTIONAL_H
#define TEST_GT_OPTIONAL_H

#include "gtest/gtest.h"

#include "gt_optional.h"

using namespace gt;

// Definition of testing types
typedef ::testing::Types<int, double, std::string> TestTypes;

template <typename T>
class OptionalTypedTest : public ::testing::Test {
protected:
    using OptType = optional<T>;

    OptType make_value(T val) { return OptType(val); }
    T default_val() {
        if constexpr (std::is_same_v<T, std::string>) return "test";
        else return static_cast<T>(42);
    }
    T alt_val() {
        if constexpr (std::is_same_v<T, std::string>) return "alt";
        else return static_cast<T>(100);
    }
};

TYPED_TEST_SUITE(OptionalTypedTest, TestTypes);

// ---------- Default Construction ----------
TYPED_TEST(OptionalTypedTest, DefaultConstruction)
{
    typename TestFixture::OptType a;
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(a, nullopt);
}

// ---------- Value Construction ----------
TYPED_TEST(OptionalTypedTest, ValueConstruction)
{
    auto val = this->default_val();
    typename TestFixture::OptType a(val);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), val);
}

// ---------- Copy/Move ----------
TYPED_TEST(OptionalTypedTest, CopyMoveConstruction)
{
    auto val = this->default_val();
    typename TestFixture::OptType a(val);
    typename TestFixture::OptType b(a);       // copy
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(b.value(), val);

    typename TestFixture::OptType c(std::move(a)); // move
    EXPECT_TRUE(c.has_value());
    EXPECT_EQ(c.value(), val);
}

// ---------- Assignment ----------
TYPED_TEST(OptionalTypedTest, AssignmentOperators)
{
    auto val = this->default_val();
    auto alt = this->alt_val();

    typename TestFixture::OptType a;
    a = val;
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), val);

    typename TestFixture::OptType b;
    b = a;
    EXPECT_TRUE(b.has_value());
    EXPECT_EQ(b.value(), val);

    b = alt;
    EXPECT_EQ(b.value(), alt);

    b = nullopt;
    EXPECT_FALSE(b.has_value());
}

// ---------- value_or ----------
TYPED_TEST(OptionalTypedTest, ValueOr)
{
    auto val = this->default_val();
    auto alt = this->alt_val();

    typename TestFixture::OptType a(val);
    EXPECT_EQ(a.value_or(alt), val);

    typename TestFixture::OptType b;
    EXPECT_EQ(b.value_or(alt), alt);
}

// ---------- Emplace ----------
TYPED_TEST(OptionalTypedTest, Emplace)
{
    auto val = this->default_val();
    auto alt = this->alt_val();

    typename TestFixture::OptType a;
    a.emplace(val);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), val);

    a.emplace(alt);
    EXPECT_TRUE(a.has_value());
    EXPECT_EQ(a.value(), alt);
}

// ---------- Swap ----------
TYPED_TEST(OptionalTypedTest, Swap)
{
    auto val = this->default_val();
    auto alt = this->alt_val();

    typename TestFixture::OptType a(val), b(alt);
    a.swap(b);
    EXPECT_EQ(a.value(), alt);
    EXPECT_EQ(b.value(), val);

    typename TestFixture::OptType c;
    a.swap(c);
    EXPECT_FALSE(a.has_value());
    EXPECT_EQ(c.value(), alt);
}

// ---------- Comparisons ----------
TYPED_TEST(OptionalTypedTest, Comparisons)
{
    auto val = this->default_val();
    auto alt = this->alt_val();

    typename TestFixture::OptType a(val), b(val), c(alt), d;

    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_FALSE(a == d);
    EXPECT_TRUE(d == nullopt);
    EXPECT_TRUE(a != nullopt);
    EXPECT_TRUE(a == val);
    EXPECT_TRUE(val == a);
    EXPECT_TRUE(a != alt);
}

// ---------- Ordering ----------
TYPED_TEST(OptionalTypedTest, Ordering)
{
    if constexpr (!std::is_same_v<TypeParam, std::string>)
    {
        typename TestFixture::OptType a(1), b(2), c;
        EXPECT_TRUE(a < b);
        EXPECT_TRUE(c < a);
        EXPECT_TRUE(b > a);
        EXPECT_TRUE(c <= a);
        EXPECT_TRUE(a >= c);
    }
}
#endif // TEST_GT_OPTIONAL_H
