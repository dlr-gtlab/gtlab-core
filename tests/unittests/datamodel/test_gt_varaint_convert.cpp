/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 26.07.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include "gt_variantconvert.h"
#include "gt_object.h"


TEST(GtVariantConvert, can_convert_string)
{
    QString string{"my_string"};
    auto v = gt::toVariant(string);

    EXPECT_TRUE(gt::canConvert<QString>(v));
    EXPECT_TRUE(gt::canConvert<QByteArray>(v));
    EXPECT_TRUE(gt::canConvert<std::string>(v));

    // restrictions
    EXPECT_FALSE(gt::canConvert<bool>(v));
    EXPECT_FALSE(gt::canConvert<int>(v));
    EXPECT_FALSE(gt::canConvert<double>(v));
    EXPECT_FALSE(gt::canConvert<float>(v));
}

TEST(GtVariantConvert, convert_string)
{
    QString string{"my_string"};
    auto v = gt::toVariant(string);

    EXPECT_EQ(gt::fromVariant<QString>(v), string);
    EXPECT_EQ(gt::fromVariant<QByteArray>(v), string.toUtf8());
    EXPECT_EQ(gt::fromVariant<std::string>(v), string.toStdString());
}

TEST(GtVariantConvert, can_convert_pod)
{
    auto vBool = gt::toVariant(true);
    EXPECT_TRUE(gt::canConvert<bool>(vBool));
    EXPECT_TRUE(gt::fromVariant<bool>(vBool));
    EXPECT_TRUE(gt::canConvert<int>(vBool));

    // restrictions
    EXPECT_FALSE(gt::canConvert<QString>(vBool));

    auto vInt = gt::toVariant(42);
    EXPECT_TRUE(gt::canConvert<int>(vInt));
    EXPECT_TRUE(gt::canConvert<double>(vInt));
    EXPECT_TRUE(gt::canConvert<float>(vInt));

    // restrictions
    EXPECT_FALSE(gt::canConvert<bool>(vInt));
    EXPECT_FALSE(gt::canConvert<bool>(gt::toVariant(1)));
    EXPECT_FALSE(gt::canConvert<QString>(vInt));
}

TEST(GtVariantConvert, to_from_variant_list_simple)
{
    auto vlist = gt::toVariantList(true);
    ASSERT_EQ(vlist.size(), 1);
    EXPECT_TRUE(gt::canConvert<bool>(vlist.first()));
    EXPECT_TRUE(gt::fromVariant<bool>(vlist.first()));

    // restrictions
    EXPECT_FALSE(gt::canConvert<QString>(vlist.first()));

    EXPECT_NO_THROW(gt::fromVariant<std::tuple<bool>>(vlist));
    EXPECT_TRUE(std::get<0>(gt::fromVariant<std::tuple<bool>>(vlist)));

    EXPECT_THROW(gt::fromVariant<std::tuple<QString>>(vlist),
                 std::runtime_error);
}

TEST(GtVariantConvert, to_from_variant_list)
{
    using Tuple = std::tuple<QString, int, bool, GtObject*>;
    Tuple tuple{"my_string", 42, true, nullptr};
    auto vlist = gt::toVariantList(tuple);

    ASSERT_EQ(vlist.size(), 4);
    EXPECT_TRUE(gt::canConvert<QString>(vlist.at(0)));
    EXPECT_TRUE(gt::canConvert<int>(vlist.at(1)));
    EXPECT_TRUE(gt::canConvert<bool>(vlist.at(2)));
    EXPECT_TRUE(gt::canConvert<QObject*>(vlist.at(3)));

    EXPECT_EQ(gt::fromVariant<QString>(vlist.at(0)), std::get<0>(tuple));
    EXPECT_EQ(gt::fromVariant<int>(vlist.at(1)), std::get<1>(tuple));
    EXPECT_EQ(gt::fromVariant<bool>(vlist.at(2)), std::get<2>(tuple));
    EXPECT_EQ(gt::fromVariant<QObject*>(vlist.at(3)), std::get<3>(tuple));

    static_assert (!gt::trait::is_generic_pointer<GtObject*>::value,
                   "Non QObject-Pointers cannot be converted to QVariant!");
}
