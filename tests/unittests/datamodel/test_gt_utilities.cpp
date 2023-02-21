/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include <QMutex>
#include <QByteArray>
#include <QString>
#include <QObject>

#include "gtest/gtest.h"


#include "gt_object.h"
#include "gt_utilities.h"
#include "gt_qtutilities.h"

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

TEST_F(TestGtUtilities, valueSuccess)
{
    bool _ok = false;
    bool* ok = &_ok;

    EXPECT_EQ(gt::valueError(12, nullptr), 12);
    EXPECT_FALSE(_ok);
    EXPECT_EQ(gt::valueSuccess(42, ok), 42);
    EXPECT_TRUE(_ok);
}

TEST_F(TestGtUtilities, valueError)
{
    bool _ok = true;
    bool* ok = &_ok;

    EXPECT_EQ(gt::valueError(12, nullptr), 12);
    EXPECT_TRUE(_ok);

    EXPECT_EQ(gt::valueError(42, ok), 42);
    EXPECT_FALSE(_ok);
}

TEST_F(TestGtUtilities, valueSetSuccess)
{
    int _ok = 0;
    int* ok = &_ok;

    EXPECT_EQ(gt::valueSetSuccess(42, 12, ok), 42);
    EXPECT_EQ(_ok, 12);

    // check return type signature
    QString _str = "Hello World";
    QString& str = gt::valueSetSuccess(_str, 0, ok);
    EXPECT_EQ(_str, str);
}

TEST_F(TestGtUtilities, checkNumericalLimits)
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

TEST_F(TestGtUtilities, objectNames)
{
    QObject o1, o2, o3;
    o1.setObjectName("Bla");
    o2.setObjectName("Blu");
    o3.setObjectName("Blup");
    QList<QObject*> ptrs = {&o1, &o2, &o3};

    QStringList expected = {o1.objectName(), o2.objectName(), o3.objectName()};
    QStringList names = gt::objectNames(ptrs);

    EXPECT_EQ(names, expected);
}

TEST_F(TestGtUtilities, container_const_cast)
{
    QVector<GtObject*> qvector = {(GtObject*)0x02};
    QVector<GtObject const*>& const_qvector = gt::container_const_cast(qvector);
    EXPECT_EQ(qvector[0], const_qvector[0]);

    // & does not work here :)
    // QList<int const*>& qlist_tmp  = gt::container_const_cast(QList<int*>{});
    QList<int const*> qlist_tmp  = gt::container_const_cast(QList<int*>{nullptr});
    EXPECT_EQ(qlist_tmp[0], nullptr);

    std::vector<int*, std::allocator<int*>> stdvector = {(int*)0x42};
    std::vector<int const*,
                std::allocator<int const*>
                >& const_stdvector = gt::container_const_cast(stdvector);
    EXPECT_EQ(stdvector[0], const_stdvector[0]);

    int myint = 42;
    std::vector<std::reference_wrapper<int>> const stdrefwrapper = { myint };
    std::vector<std::reference_wrapper<int const>,
                std::allocator<std::reference_wrapper<int const>>
                > const& const_stdrefwrapper = gt::container_const_cast(stdrefwrapper);
    EXPECT_EQ(stdrefwrapper[0], const_stdrefwrapper[0]);
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

TEST_F(TestGtUtilities, clamp)
{
    EXPECT_EQ(gt::clamp(1, -9, 7), 1);

    EXPECT_EQ(gt::clamp(-9, 1, 7), 1);

    EXPECT_EQ(gt::clamp(7, -9, 1), 1);

    EXPECT_DOUBLE_EQ(gt::clamp(-1234.12341, 2341.4, 42411.57), 2341.4);

    EXPECT_DOUBLE_EQ(gt::clamp(250034.12341, 2341.4, 42411.57), 42411.57);

    EXPECT_DOUBLE_EQ(gt::clamp(5234.12341, 2341.4, 42411.57), 5234.12341);

    // clamp double to int
    EXPECT_EQ(gt::clamp<int>(523.1234, 2341, 42411), 2341);

    // clamp double to int
    EXPECT_DOUBLE_EQ(gt::clamp<double>(523, 12.134, 321.12345), 321.12345);

}
