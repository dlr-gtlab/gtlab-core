/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <tl/optional.hpp>

#include <string>


struct TestOptional : public testing::Test
{
    tl::optional<double> var;
};

TEST_F(TestOptional, has_value)
{
    EXPECT_FALSE(var.has_value());
    EXPECT_EQ(var.value_or(12), 12);
    var = 42;


    EXPECT_TRUE(var.has_value());
    EXPECT_EQ(var.value_or(12), 42);
}
