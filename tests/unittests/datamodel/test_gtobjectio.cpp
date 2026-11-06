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
#include <cmath>
#include <cstdio>

/// Switches LC_ALL for its lifetime and restores the previous locale
/// on destruction. If the requested locale is not available, the
/// current one is kept.
struct LocaleGuard
{
    explicit LocaleGuard(const char* locale)
    {
        if (const char* current = setlocale(LC_ALL, nullptr))
        {
            snprintf(prev, sizeof(prev), "%s", current);
        }
        setlocale(LC_ALL, locale);
    }

    ~LocaleGuard()
    {
        if (prev[0] != '\0')
        {
            setlocale(LC_ALL, prev);
        }
    }

    char prev[128] = {};
};

/// This is a test fixture
struct TestGtObjectIO_Read : public ::testing::Test
{
    void SetUp() override
    {
        xmlStr = R"(
        <object class="TestSpecialGtObject" name="Group" uuid="{a-uid}">
         <propertylist name="emptyStringList" type="QString"></propertylist>
         <propertylist name="stringList" type="QString">hello;world</propertylist>
         <propertylist name="threeStrings" type="QString">;;</propertylist>
         <propertylist name="doubleList" type="double">2.4;2.6;2.7</propertylist>
         <propertylist name="doubleListExp" type="double">1.5e3;-2.5E-3;+3e+2</propertylist>
         <propertylist name="doubleListOverflow" type="double">1e999;1e-999</propertylist>
         <property name="invalidVariant" type=""/>
        </object>
        )";

        memento = GtObjectMemento(xmlStr);
    }

    const char* xmlStr;
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
    // strtod based parsing is locale dependent: parse under a locale
    // with a different decimal point so the test does not depend on
    // the machine's default locale (de_DE is used when available)
    LocaleGuard guard("de_DE.UTF-8");

    GtObjectMemento parsed(xmlStr);

    auto dblList = GtObjectMemento::findPropertyByName(parsed.properties,
                                                       "doubleList");

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

TEST_F(TestGtObjectIO_Read, readDoubleListExponents)
{
    LocaleGuard guard("de_DE.UTF-8");

    GtObjectMemento parsed(xmlStr);

    auto dblList = GtObjectMemento::findPropertyByName(parsed.properties,
                                                       "doubleListExp");

    ASSERT_TRUE(dblList != nullptr);

    QVariant variant = dblList->data();
    auto list = variant.value<QVector<double> >();

    ASSERT_EQ(3, list.size());
    EXPECT_DOUBLE_EQ(1500.0, list[0]);
    EXPECT_DOUBLE_EQ(-0.0025, list[1]);
    EXPECT_DOUBLE_EQ(300.0, list[2]);
}

TEST_F(TestGtObjectIO_Read, readDoubleListOverflowFallback)
{
    LocaleGuard guard("de_DE.UTF-8");

    GtObjectMemento parsed(xmlStr);

    auto dblList = GtObjectMemento::findPropertyByName(parsed.properties,
                                                       "doubleListOverflow");

    ASSERT_TRUE(dblList != nullptr);

    QVariant variant = dblList->data();
    auto list = variant.value<QVector<double> >();

    ASSERT_EQ(2, list.size());

    // ERANGE in strtod -> Qt parser fallback decides
    EXPECT_TRUE(std::isinf(list[0]));
    EXPECT_DOUBLE_EQ(0.0, list[1]);
}

TEST_F(TestGtObjectIO_Read, doubleListRoundTripExponents)
{
    // parse under de_DE, serialize, re-parse under the C locale:
    // the intermediate XML representation must be locale neutral
    QByteArray bytes;
    {
        LocaleGuard guard("de_DE.UTF-8");
        GtObjectMemento parsed(xmlStr);
        bytes = parsed.toByteArray();
    }

    GtObjectMemento reparsed;
    {
        LocaleGuard guard("C");
        reparsed = GtObjectMemento(bytes);
    }

    auto dblList = GtObjectMemento::findPropertyByName(reparsed.properties,
                                                       "doubleListExp");

    ASSERT_TRUE(dblList != nullptr);

    QVariant variant = dblList->data();
    auto list = variant.value<QVector<double> >();

    ASSERT_EQ(3, list.size());
    EXPECT_DOUBLE_EQ(1500.0, list[0]);
    EXPECT_DOUBLE_EQ(-0.0025, list[1]);
    EXPECT_DOUBLE_EQ(300.0, list[2]);
}

