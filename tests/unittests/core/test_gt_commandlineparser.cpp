/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_commandlineparser.cpp
 *
 *  Created on: 05.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_commandlineparser.h"

/// This is a test fixture that does a init for each test
class TestGtCommandLineParser : public ::testing::Test
{
protected:
    QStringList list1 {"-e", "command", "--gtlab"};
    QStringList list2 {};
    QStringList list3 {"-e=Value"};
    QStringList list4 {"awesome", "--test", "-w", "--help"};
    QStringList list5 {"awesome", "--test", "-w", "-h"};
    QStringList list6 {"awesome", "--test", "-w", "-?"};
    QStringList list7 {"-e=Value", "run", "--flag", "positional"};
};

TEST_F(TestGtCommandLineParser, parse)
{
    GtCommandLineParser parser;

    ASSERT_FALSE(parser.parse(list2));

    ASSERT_TRUE(parser.parse(list1));

    EXPECT_EQ(list1, parser.arguments());

    EXPECT_EQ(parser.firstPositionalArgument(), "command");
}

TEST_F(TestGtCommandLineParser, addOption)
{
    GtCommandLineParser parser;

    parser.addOption("evaluate", {"eval", "e"}, "Do some evaulation");

    parser.addHelpOption();

    ASSERT_TRUE(parser.parse(list1));

    EXPECT_FALSE(parser.option("gtlab"));

    EXPECT_TRUE(parser.option("evaluate"));

    ASSERT_TRUE(parser.parse(list3));

    EXPECT_EQ(parser.optionValue("evaluate"), "Value");
}

TEST_F(TestGtCommandLineParser, help)
{
    GtCommandLineParser parser;
    parser.addHelpOption();

    parser.parse(list3);
    EXPECT_FALSE(parser.helpOption());
    parser.parse(list4);
    EXPECT_TRUE(parser.helpOption());
    parser.parse(list5);
    EXPECT_TRUE(parser.helpOption());
    parser.parse(list6);
    EXPECT_TRUE(parser.helpOption());
}

TEST_F(TestGtCommandLineParser, optionValueFallsBackToDefaultAndUnknownIsInvalid)
{
    GtCommandLineParser parser;
    parser.addOption("flag", {"flag", "f"}, "flag option", QString{"default"});

    ASSERT_TRUE(parser.parse({"command", "--flag"}));

    EXPECT_EQ(parser.optionValue("flag").toString(), QString{"default"});
    EXPECT_FALSE(parser.optionValue("missing").isValid());
}

TEST_F(TestGtCommandLineParser, argumentRemovalAndSizeChecksWorkOnParsedArguments)
{
    GtCommandLineParser parser;

    ASSERT_TRUE(parser.parse(list7));
    EXPECT_TRUE(parser.argument("run"));
    EXPECT_TRUE(parser.checkSize(4));

    parser.removeArg("--flag");

    EXPECT_FALSE(parser.argument("--flag"));
    EXPECT_TRUE(parser.checkSize(3));
    EXPECT_EQ(parser.firstPositionalArgument(), QString{"run"});
}

TEST_F(TestGtCommandLineParser, positionalArgumentsFilterOutOptions)
{
    GtCommandLineParser parser;

    ASSERT_TRUE(parser.parse(list7));

    EXPECT_EQ(parser.positionalArguments(), QStringList({"run", "positional"}));
}

TEST_F(TestGtCommandLineParser, printHelpListsRegisteredOptions)
{
    GtCommandLineParser parser;
    parser.addOption("evaluate", {"eval", "e"}, "Do some evaulation");
    parser.addHelpOption();

    testing::internal::CaptureStdout();
    parser.printHelp("unit-command");
    QString output = QString::fromStdString(testing::internal::GetCapturedStdout());

    EXPECT_TRUE(output.contains("Usage: GTlabConsole.exe"));
    EXPECT_TRUE(output.contains("Help for current command unit-command"));
    EXPECT_TRUE(output.contains("--eval -e"));
    EXPECT_TRUE(output.contains("--help -h -?"));
}

TEST_F(TestGtCommandLineParser, detailHelpersHandleShortLongAndAssignedOptions)
{
    GtCommandLineOption option{{"execute", "e"}, "Execute", QString{"fallback"}};
    QStringList args{"program", "--execute=value", "-x", "-e=other"};

    EXPECT_TRUE(gt::commandline::detail::hasOption(args, option));
    EXPECT_EQ(gt::commandline::detail::indexOfOption(args, option), 1);
    EXPECT_EQ(gt::commandline::detail::valueOfOption(args, option).toString(),
              QString{"value"});
}

TEST_F(TestGtCommandLineParser, detailHelpersReturnMissingStateForAbsentOptions)
{
    GtCommandLineOption option{{"execute", "e"}, "Execute", true};
    QStringList args{"program", "--other"};

    EXPECT_FALSE(gt::commandline::detail::hasOption(args, option));
    EXPECT_EQ(gt::commandline::detail::indexOfOption(args, option), -1);
    EXPECT_FALSE(gt::commandline::detail::valueOfOption(args, option).isValid());
}
