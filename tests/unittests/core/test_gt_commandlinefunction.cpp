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

#include "gt_commandlinefunction.h"

namespace CommandLineTesting
{
int myAwesomeTest(const QStringList& args)
{
    if (args.size() < 2)
    {
        return -1;
    }
    return 0;
}
}

/// This is a test fixture that does a init for each test
class TestCommandLineFunction : public ::testing::Test
{

protected:
    gt::CommandLineFunction m_func{
        "name", CommandLineTesting::myAwesomeTest, "description"
    };
};

TEST_F(TestCommandLineFunction, getter)
{
    ASSERT_EQ(m_func.brief(), "description");
    ASSERT_EQ(m_func.id(), "name");
}


TEST_F(TestCommandLineFunction, executionTest)
{
    QStringList argsSuccess = {"one", "two"};
    QStringList argsFailure = {"zero"};

    ASSERT_DOUBLE_EQ(m_func.operator()(argsSuccess), 0);
    ASSERT_DOUBLE_EQ(m_func.operator()(argsFailure), -1);
}
