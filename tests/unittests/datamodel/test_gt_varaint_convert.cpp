/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 26.07.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include "gt_variant_convert.h"
#include "gt_object.h"

#include <QDebug>

TEST(GtVariantConvert, can_convert_string)
{
    QString string{"my_string"};
    auto v = gt::to_variant(string);

    EXPECT_TRUE(gt::can_convert<QString>(v));
    EXPECT_TRUE(gt::can_convert<QByteArray>(v));
    EXPECT_TRUE(gt::can_convert<std::string>(v));

    // restrictions
    EXPECT_FALSE(gt::can_convert<bool>(v));
    EXPECT_FALSE(gt::can_convert<int>(v));
    EXPECT_FALSE(gt::can_convert<double>(v));
    EXPECT_FALSE(gt::can_convert<float>(v));
}

TEST(GtVariantConvert, convert_string)
{
    QString string{"my_string"};
    auto v = gt::to_variant(string);

    EXPECT_EQ(gt::from_variant<QString>(v), string);
    EXPECT_EQ(gt::from_variant<QByteArray>(v), string.toUtf8());
    EXPECT_EQ(gt::from_variant<std::string>(v), string.toStdString());
}

TEST(GtVariantConvert, can_convert_pod)
{
    auto vBool = gt::to_variant(true);
    EXPECT_TRUE(gt::can_convert<bool>(vBool));
    EXPECT_TRUE(gt::from_variant<bool>(vBool));
    EXPECT_TRUE(gt::can_convert<int>(vBool));

    // restrictions
    EXPECT_FALSE(gt::can_convert<QString>(vBool));

    auto vInt = gt::to_variant(42);
    EXPECT_TRUE(gt::can_convert<int>(vInt));
    EXPECT_TRUE(gt::can_convert<double>(vInt));
    EXPECT_TRUE(gt::can_convert<float>(vInt));

    // restrictions
    EXPECT_FALSE(gt::can_convert<bool>(vInt));
    EXPECT_FALSE(gt::can_convert<bool>(gt::to_variant(1)));
    EXPECT_FALSE(gt::can_convert<QString>(vInt));
}

TEST(GtVariantConvert, to_from_variant_list_simple)
{
    auto vlist = gt::to_variant_list(true);
    ASSERT_EQ(vlist.size(), 1);
    EXPECT_TRUE(gt::can_convert<bool>(vlist.first()));
    EXPECT_TRUE(gt::from_variant<bool>(vlist.first()));

    // restrictions
    EXPECT_FALSE(gt::can_convert<QString>(vlist.first()));

    EXPECT_NO_THROW(gt::from_variant<std::tuple<bool>>(vlist));
    EXPECT_TRUE(std::get<0>(gt::from_variant<std::tuple<bool>>(vlist)));

    EXPECT_THROW(gt::from_variant<std::tuple<QString>>(vlist),
                 std::runtime_error);
}

TEST(GtVariantConvert, to_from_variant_list)
{
    using Tuple = std::tuple<QString, int, bool, GtObject*>;
    Tuple tuple{"my_string", 42, true, nullptr};
    auto vlist = gt::to_variant_list(tuple);

    ASSERT_EQ(vlist.size(), 4);
    EXPECT_TRUE(gt::can_convert<QString>(vlist.at(0)));
    EXPECT_TRUE(gt::can_convert<int>(vlist.at(1)));
    EXPECT_TRUE(gt::can_convert<bool>(vlist.at(2)));
    EXPECT_TRUE(gt::can_convert<QObject*>(vlist.at(3)));

    EXPECT_EQ(gt::from_variant<QString>(vlist.at(0)), std::get<0>(tuple));
    EXPECT_EQ(gt::from_variant<int>(vlist.at(1)), std::get<1>(tuple));
    EXPECT_EQ(gt::from_variant<bool>(vlist.at(2)), std::get<2>(tuple));
    EXPECT_EQ(gt::from_variant<QObject*>(vlist.at(3)), std::get<3>(tuple));

    static_assert (!gt::trait::is_generic_pointer<GtObject*>::value,
                   "Non QObject-Pointers cannot be converted to QVariant!");
}
