/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_commandlinefunction
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
class TestGtCommandLineFunction : public ::testing::Test
{
protected:
    GtCommandLineFunction m_func{
        "name", CommandLineTesting::myAwesomeTest, "description"
    };
};

TEST_F(TestGtCommandLineFunction, getter)
{
    EXPECT_EQ(m_func.brief(), "description");
    EXPECT_EQ(m_func.id(), "name");
}

TEST_F(TestGtCommandLineFunction, executionTest)
{
    QStringList argsSuccess = {"one", "two"};
    QStringList argsFailure = {"zero"};

    EXPECT_DOUBLE_EQ(m_func.operator()(argsSuccess), 0);
    EXPECT_DOUBLE_EQ(m_func.operator()(argsFailure), -1);
}

TEST_F(TestGtCommandLineFunction, defaultHelpFlagCanBeToggled)
{
    EXPECT_TRUE(m_func.useDefaultHelp());
    EXPECT_EQ(&m_func.setUseDefaultHelp(false), &m_func);
    EXPECT_FALSE(m_func.useDefaultHelp());
    EXPECT_EQ(&m_func.setUseDefaultHelp(true), &m_func);
    EXPECT_TRUE(m_func.useDefaultHelp());
}

TEST_F(TestGtCommandLineFunction, optionsAndArgumentsCanBeConfigured)
{
    QList<GtCommandLineOption> options{{{"verbose", "v"}, "Verbose mode"}};
    QList<GtCommandLineArgument> arguments{{{"file", "Input file"}}};

    EXPECT_EQ(&m_func.setOptions(options), &m_func);
    EXPECT_EQ(&m_func.setArgs(arguments), &m_func);
    EXPECT_EQ(m_func.options().size(), 1);
    EXPECT_EQ(m_func.options().front().names, QStringList({"verbose", "v"}));
    EXPECT_EQ(m_func.arguments().size(), 1);
    EXPECT_EQ(m_func.arguments().front().name, QString{"file"});
}

TEST_F(TestGtCommandLineFunction, nullStateAndBoolOperatorsReflectFunctionPresence)
{
    GtCommandLineFunction nullFunc;

    EXPECT_TRUE(nullFunc.isNull());
    EXPECT_FALSE(static_cast<bool>(nullFunc));
    EXPECT_TRUE(nullFunc == nullptr);
    EXPECT_FALSE(nullFunc != nullptr);

    EXPECT_FALSE(m_func.isNull());
    EXPECT_TRUE(static_cast<bool>(m_func));
    EXPECT_FALSE(m_func == nullptr);
    EXPECT_TRUE(m_func != nullptr);
}

TEST_F(TestGtCommandLineFunction, makeCommandLineFunctionCreatesCallableObject)
{
    auto func = gt::makeCommandLineFunction("made", CommandLineTesting::myAwesomeTest,
                                            "made description");

    EXPECT_EQ(func.id(), QString{"made"});
    EXPECT_EQ(func.brief(), QString{"made description"});
    EXPECT_EQ(func(QStringList{"one", "two"}), 0);
}

TEST_F(TestGtCommandLineFunction, showDefaultHelpPrintsOptionsAndArguments)
{
    m_func.setOptions({{{"verbose", "v"}, "Verbose mode"}})
          .setArgs({{"file", "Input file"}});

    testing::internal::CaptureStdout();
    m_func.showDefaultHelp();
    QString output = QString::fromStdString(testing::internal::GetCapturedStdout());

    EXPECT_TRUE(output.contains("Usage: GTlabConsole.exe name file"));
    EXPECT_TRUE(output.contains("description"));
    EXPECT_TRUE(output.contains("Options:"));
    EXPECT_TRUE(output.contains("--verbose -v"));
    EXPECT_TRUE(output.contains("Arguments:"));
    EXPECT_TRUE(output.contains("file"));
    EXPECT_TRUE(output.contains("Input file"));
}
