/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include <QDebug>

#include "gtest/gtest.h"

#include "gt_utilities.h"

class TestGtUtilities : public testing::Test
{

};

TEST_F(TestGtUtilities, quoted)
{
    QString qstr{"Hello World"};
    EXPECT_EQ("(" + qstr + ")", gt::brackets(qstr));
    EXPECT_EQ("'" + qstr + "'", gt::squoted(qstr));
    EXPECT_EQ('"' + qstr + '"', gt::quoted(qstr));
    EXPECT_EQ("test_" + qstr + "_test", gt::quoted(qstr, "test_", "_test"));

    QByteArray qba{"My Fancy String"};
    EXPECT_EQ("(" + qba + ")", gt::brackets(qba));
    EXPECT_EQ("'" + qba + "'", gt::squoted(qba));
    EXPECT_EQ('"' + qba + '"', gt::quoted(qba));
    EXPECT_EQ("test_" + qba + "_test", gt::quoted(qba, "test_", "_test"));

    std::string std{"<something_inspirational>"};
    EXPECT_EQ("(" + std + ")", gt::brackets(std));
    EXPECT_EQ("'" + std + "'", gt::squoted(std));
    EXPECT_EQ('"' + std + '"', gt::quoted(std));
    EXPECT_EQ("test_" + std + "_test", gt::quoted(std, "test_", "_test"));
}
