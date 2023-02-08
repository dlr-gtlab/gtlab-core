/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_structproperty
 * copyright 2023 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gtest/gtest.h"

#include "gt_objectio.h"
#include "gt_objectmemento.h"

#include <iostream>

/// This is a test fixture
struct TestGtObjectIO_Read : public ::testing::Test
{
    void SetUp() override
    {
        auto xmlStr = R"(
        <object class="TestSpecialGtObject" name="Group" uuid="{a-uid}">
         <propertylist name="emptyStringList" type="QString"></propertylist>
         <propertylist name="stringList" type="QString">hello;world</propertylist>
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
