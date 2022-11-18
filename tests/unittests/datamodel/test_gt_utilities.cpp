/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include <QDebug>
#include <QMutex>

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

TEST_F(TestGtUtilities, checkumericalLimits)
{
    EXPECT_TRUE(gt::checkNumericalLimits<int>(-1));
    EXPECT_TRUE(gt::checkNumericalLimits<double>(42.0));

    // an unsigned int can go below 0
    EXPECT_FALSE(gt::checkNumericalLimits<unsigned int>(-1));

    // larger number than an int16
    EXPECT_FALSE(gt::checkNumericalLimits<int16_t>(
                     std::numeric_limits<int16_t>::max() + int32_t{1}));

    // lower number than an int32
    EXPECT_FALSE(gt::checkNumericalLimits<int32_t>(
                     std::numeric_limits<int32_t>::min() - int64_t{1}));
}

TEST_F(TestGtUtilities, finally_member)
{
    QMutex m;

    m.lock();

    {
        // alredy locked
        EXPECT_FALSE(m.tryLock());

        auto finally = gt::finally(&m, &QMutex::unlock);
        ASSERT_FALSE(finally.isNull());
        finally.clear();
        EXPECT_TRUE(finally.isNull());
    }

    // still locked
    EXPECT_FALSE(m.tryLock());

    {
        auto finally = gt::finally(&m, &QMutex::unlock);
        ASSERT_FALSE(finally.isNull());

        // still locked
        EXPECT_FALSE(m.tryLock());
    }

    // no longer locked
    EXPECT_TRUE(m.tryLock());
    m.unlock();
}

TEST_F(TestGtUtilities, finally_lambda)
{
    int i = 1;

    auto lambda = [&](){ i--; };

    {
        EXPECT_EQ(i, 1);

        auto finally = gt::finally(lambda);
        ASSERT_FALSE(finally.isNull());
        finally.clear();
        EXPECT_TRUE(finally.isNull());
    }

    EXPECT_EQ(i, 1);

    {
        auto finally = gt::finally(lambda);
        ASSERT_FALSE(finally.isNull());

        EXPECT_EQ(i, 1);
    }

    EXPECT_EQ(i, 0);
}
