/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_commandlinefunction
 * copyright 2009-2022 by DLR
 *
 *  Created on: 16.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include <QStringList>

#include "internal/gt_commandlinefunctionhandler.h"
#include "gt_commandlinefunction.h"

namespace CommandLineTesting
{
int myAwesomeTest2(const QStringList& args)
{
    if (args.size() < 4)
    {
        return -1;
    }
    return 0;
}
}

/// This is a test fixture that does a init for each test
class TestGtCommandLineInterface : public ::testing::Test
{};

TEST_F(TestGtCommandLineInterface, registration)
{
    auto fun = GtCommandLineInterface::make_commandLineFunction(
                "test function", CommandLineTesting::myAwesomeTest2,
                "combines arguments");

    gtlab::commandline::register_function(fun);

    GtCommandLineFunction f2 =
            GtCommandLineFunctionHandler::instance().getInterfaceFunc(
                "test function");

    GtCommandLineFunction f3 =
            GtCommandLineFunctionHandler::instance().getInterfaceFunc(
                "wrongName");

    ASSERT_NE(f2, nullptr);
    ASSERT_EQ(f3, nullptr);

    QStringList success = {"1", "2", "3", "4", "5"};
    QStringList failure = {"1", "2"};

    ASSERT_DOUBLE_EQ(fun(success), 0);
    ASSERT_DOUBLE_EQ(fun(success), f2(success));

    ASSERT_DOUBLE_EQ(fun(failure), -1);
    ASSERT_DOUBLE_EQ(fun(failure), f2(failure));
}

