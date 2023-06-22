/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <mpark/variant.hpp>

#include <string>


struct TestVariant : public testing::Test
{
    mpark::variant<std::string, double> var;
};

TEST_F(TestVariant, setGet)
{
    var = "hallo";

    auto pi = mpark::get_if<double>(&var);
    ASSERT_TRUE(pi == nullptr);

    auto ps = mpark::get_if<std::string>(&var);
    ASSERT_TRUE(ps != nullptr);
    EXPECT_EQ(*ps, "hallo");
}

TEST_F(TestVariant, visit)
{
    struct
    {
        void operator()(std::string )
        {
            lastType = "string";
        }

        void operator()(double )
        {
            lastType = "double";
        }

        std::string lastType;
    } visitor;

    var = "hallo";
    mpark::visit(visitor, var);
    EXPECT_EQ("string", visitor.lastType);


    var = 0.;
    mpark::visit(visitor, var);
    EXPECT_EQ("double", visitor.lastType);
}
