/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_dynamicinterface.cpp
 * copyright 2022 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#include "gtest/gtest.h"

#include "gt_functional_interface.h"
#include "gt_dynamicinterfacehandler.h"

#include <QString>

class DynamicInterface : public testing::Test
{};

int my_test_sum (int a, int b)
{
    return a + b;
}

TEST_F(DynamicInterface, wrapFunction)
{
    auto itf_fun = gtlab::interface::make_interface_function("my_test_sum",
                                                             my_test_sum);
    auto result = itf_fun({1, 2});

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(3, result[0].toInt());

    // too few arguments
    EXPECT_THROW(itf_fun({1}), std::runtime_error);

    // arg 0 cannot be converted into int
    EXPECT_THROW(itf_fun({"bla", 2}), std::runtime_error);
}

TEST_F(DynamicInterface, wrapQString)
{
    auto mystringfunction = [](QString str) {
        return str;
    };

    auto itf_fun = gtlab::interface::make_interface_function("mystringfunction",
                                                             mystringfunction);

    auto result = itf_fun({"Hallo Welt"});

    ASSERT_EQ(1, result.size());
    EXPECT_STREQ("Hallo Welt", result[0].toString().toStdString().c_str());
}

TEST_F(DynamicInterface, wrapStdString)
{
    auto mystringfunction = [](std::string str) {
        return str;
    };

    auto itf_fun = gtlab::interface::make_interface_function("mystringfunction2",
                                                             mystringfunction);

    auto result = itf_fun({"Hallo Welt"});

    ASSERT_EQ(1, result.size());
    EXPECT_STREQ("Hallo Welt", result[0].toString().toStdString().c_str());
}

TEST_F(DynamicInterface, wrapLambda)
{
    auto lambda = [](double a, double b) {
        return a*b;
    };

    auto itf_fun = gtlab::interface::make_interface_function(
        "my_lambda_mult", lambda);

    auto result = itf_fun({3., 4.});

    EXPECT_EQ(1, result.size());
    EXPECT_EQ(12., result[0].toDouble());
}

TEST_F(DynamicInterface, wrapFunctionObject)
{
    struct MyFunctionObj
    {
        explicit MyFunctionObj(double base) : base(base) {}

        double operator()(double head) const
        {
            return base + head;
        }

        double base{0.};
    };

    auto f = MyFunctionObj(10.);

    // sanity check
    ASSERT_EQ(15, f(5.));

    auto itf_fun = gtlab::interface::make_interface_function(
        "my_func_obj", f);

    auto result = itf_fun({12.});
    EXPECT_EQ(1, result.size());
    EXPECT_EQ(22., result[0].toDouble());
}

TEST_F(DynamicInterface, returnTuple)
{
    auto itf_fun = gtlab::interface::make_interface_function(
        "my_lambda_tuple",
        [](double a, double b) {
            return std::make_tuple(a+b, a-b);
        }
    );

    auto result = itf_fun({10., 4.});
    ASSERT_EQ(2, result.size());
    EXPECT_EQ(14., result[0].toDouble());
    EXPECT_EQ(6., result[1].toDouble());
}


TEST_F(DynamicInterface, returnVoid)
{
    auto itf_fun = gtlab::interface::make_interface_function(
        "my_return_void_func",
        [](double) {
            // nothing
        }
    );

    auto result = itf_fun({10.});
    ASSERT_EQ(0, result.size());
}

TEST_F(DynamicInterface, getFunctionFailure)
{
    auto func = gtlab::interface::get_function("this_funcion_does_not_exist");
    EXPECT_FALSE(func);
}


TEST_F(DynamicInterface, registerFunctionNoHelp)
{
    ASSERT_TRUE(gtlab::interface::register_function("my_test_sum", my_test_sum));

    auto func = gtlab::interface::get_function("my_test_sum");
    ASSERT_TRUE(func);

    EXPECT_FALSE(func.help().isEmpty());
}

TEST_F(DynamicInterface, registerFunctionWithHelp)
{
    auto help = "this is the help of my_test_sum2";

    ASSERT_TRUE(gtlab::interface::register_function("my_test_sum2", my_test_sum, help));

    auto func = gtlab::interface::get_function("my_test_sum2");
    ASSERT_TRUE(func);

    EXPECT_STREQ(help, func.help().toStdString().c_str());
}

TEST_F(DynamicInterface, checkName)
{
    ASSERT_TRUE(gtlab::interface::register_function("my_test_sum3", my_test_sum));

    auto func = gtlab::interface::get_function("my_test_sum3");
    EXPECT_STREQ("my_test_sum3", func.name().toStdString().c_str());
}
