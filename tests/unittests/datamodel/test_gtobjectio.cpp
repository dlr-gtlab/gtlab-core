/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_structproperty
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gtest/gtest.h"

#include "gt_objectio.h"
#include "gt_objectmemento.h"

#include <iostream>

#include <clocale>
#include <cstdio>

/// This is a test fixture
struct TestGtObjectIO_Read : public ::testing::Test
{
    void SetUp() override
    {
        auto xmlStr = R"(
        <object class="TestSpecialGtObject" name="Group" uuid="{a-uid}">
         <propertylist name="emptyStringList" type="QString"></propertylist>
         <propertylist name="stringList" type="QString">hello;world</propertylist>
         <propertylist name="threeStrings" type="QString">;;</propertylist>
         <propertylist name="doubleList" type="double">2.4;2.6;2.7</propertylist>
         <property name="invalidVariant" type=""/>
        </object>
        )";

        memento = GtObjectMemento(xmlStr);
    }

    GtObjectMemento memento;
};

TEST_F(TestGtObjectIO_Read, readEmptyStringList)
{
    auto strList = GtObjectMemento::findPropertyByName(memento.properties,
                                                       "emptyStringList");

    ASSERT_TRUE(strList != nullptr);

    QVariant variant = strList->data();
    EXPECT_EQ(variant.userType(), QMetaType::QStringList);

    auto list = variant.toStringList();
    EXPECT_EQ(0, list.size());
}

TEST_F(TestGtObjectIO_Read, readStringList)
{
    auto strList = GtObjectMemento::findPropertyByName(memento.properties,
                                                       "stringList");

    ASSERT_TRUE(strList != nullptr);

    QVariant variant = strList->data();
    EXPECT_EQ(variant.userType(), QMetaType::QStringList);

    auto list = variant.toStringList();
    EXPECT_EQ(2, list.size());
    EXPECT_EQ("hello", list[0].toStdString());
    EXPECT_EQ("world", list[1].toStdString());
}

TEST_F(TestGtObjectIO_Read, read3Strings)
{
    auto strList = GtObjectMemento::findPropertyByName(memento.properties,
                                                       "threeStrings");

    ASSERT_TRUE(strList != nullptr);

    QVariant variant = strList->data();
    EXPECT_EQ(variant.userType(), QMetaType::QStringList);

    auto list = variant.toStringList();
    ASSERT_EQ(3, list.size());
    EXPECT_EQ("", list[0].toStdString());
    EXPECT_EQ("", list[1].toStdString());
    EXPECT_EQ("", list[2].toStdString());
}

TEST_F(TestGtObjectIO_Read, invalidQVariant)
{
    auto variantProp = GtObjectMemento::findPropertyByName(memento.properties,
                                                       "invalidVariant");

    ASSERT_TRUE(variantProp != nullptr);

    QVariant variant = variantProp->data();

    EXPECT_FALSE(variant.isValid());
}

TEST_F(TestGtObjectIO_Read, readDoubleList)
{
    // strtod based parsing is locale dependent: switch to a locale with
    // a different decimal point when available, restore it afterwards
    char prevLocale[128] = {};
    if (const char* current = setlocale(LC_ALL, nullptr))
    {
        snprintf(prevLocale, sizeof(prevLocale), "%s", current);
    }

    setlocale(LC_ALL, "de_DE.UTF-8");

    auto dblList = GtObjectMemento::findPropertyByName(memento.properties,
                                                       "doubleList");

    if (prevLocale[0] != '\0')
    {
        setlocale(LC_ALL, prevLocale);
    }

    ASSERT_TRUE(dblList != nullptr);

    QVariant variant = dblList->data();
    auto list = variant.value<QVector<double> >();

    ASSERT_EQ(3, list.size());
    EXPECT_DOUBLE_EQ(2.4, list[0]);
    EXPECT_DOUBLE_EQ(2.6, list[1]);
    EXPECT_DOUBLE_EQ(2.7, list[2]);
}

TEST_F(TestGtObjectIO_Read, doubleListByteRoundTrip)
{
    GtObjectMemento roundTrip(memento.toByteArray());

    auto dblList = GtObjectMemento::findPropertyByName(roundTrip.properties,
                                                       "doubleList");

    ASSERT_TRUE(dblList != nullptr);

    QVariant variant = dblList->data();
    auto list = variant.value<QVector<double> >();

    ASSERT_EQ(3, list.size());
    EXPECT_DOUBLE_EQ(2.4, list[0]);
    EXPECT_DOUBLE_EQ(2.6, list[1]);
    EXPECT_DOUBLE_EQ(2.7, list[2]);
}

