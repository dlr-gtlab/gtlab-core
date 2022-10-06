/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_commandlineparser.cpp
 * copyright 2009-2022 by DLR
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
};

TEST_F(TestGtCommandLineParser, parse)
{
    GtCommandLineParser parser;

    ASSERT_FALSE(parser.parse(list2));

    ASSERT_TRUE(parser.parse(list1));

    ASSERT_EQ(list1, parser.arguments());

    ASSERT_EQ(parser.firstPositionalArgument(), "command");
}

TEST_F(TestGtCommandLineParser, addOption)
{
    GtCommandLineParser parser;

    parser.addOption("evaluate", {"eval", "e"}, "Do some evaulation");

    parser.addHelpOption();

    ASSERT_TRUE(parser.parse(list1));

    ASSERT_FALSE(parser.option("gtlab"));

    ASSERT_TRUE(parser.option("evaluate"));

    ASSERT_TRUE(parser.parse(list3));

    ASSERT_EQ(parser.optionValue("evaluate"), "Value");
}

TEST_F(TestGtCommandLineParser, help)
{
    GtCommandLineParser parser;
    parser.addHelpOption();

    parser.parse(list3);
    ASSERT_FALSE(parser.helpOption());
    parser.parse(list4);
    ASSERT_TRUE(parser.helpOption());
    parser.parse(list5);
    ASSERT_TRUE(parser.helpOption());
    parser.parse(list6);
    ASSERT_TRUE(parser.helpOption());
}


