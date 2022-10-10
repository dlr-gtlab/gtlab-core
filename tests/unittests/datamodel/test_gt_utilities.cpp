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
    EXPECT_EQ("(" + qstr + ")", Gt::brackets(qstr));
    EXPECT_EQ("'" + qstr + "'", Gt::squoted(qstr));
    EXPECT_EQ('"' + qstr + '"', Gt::quoted(qstr));
    EXPECT_EQ("test_" + qstr + "_test", Gt::quoted(qstr, "test_", "_test"));

    QByteArray qba{"My Fancy String"};
    EXPECT_EQ("(" + qba + ")", Gt::brackets(qba));
    EXPECT_EQ("'" + qba + "'", Gt::squoted(qba));
    EXPECT_EQ('"' + qba + '"', Gt::quoted(qba));
    EXPECT_EQ("test_" + qba + "_test", Gt::quoted(qba, "test_", "_test"));

    std::string std{"<something_inspirational>"};
    EXPECT_EQ("(" + std + ")", Gt::brackets(std));
    EXPECT_EQ("'" + std + "'", Gt::squoted(std));
    EXPECT_EQ('"' + std + '"', Gt::quoted(std));
    EXPECT_EQ("test_" + std + "_test", Gt::quoted(std, "test_", "_test"));
}
