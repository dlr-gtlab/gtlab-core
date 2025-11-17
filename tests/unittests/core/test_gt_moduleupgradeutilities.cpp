/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * Source File: test_gt_module_upgradeutilities.cpp
 *
 *  Created on: 17.11.2025
 *  Author: Jens Schmeink (AT-TWK)
 */

#include <gtest/gtest.h>
#include <QDomDocument>
#include <QStringList>
#include "gt_moduleupgradeutilities.h"

using namespace gt::module_upgrade_utils;

class ModuleUpgradeUtilsTest : public ::testing::Test {
protected:
    QDomDocument doc;
    QDomElement root;

    void SetUp() override {
        root = doc.createElement("root");
        doc.appendChild(root);

        // Beispielstruktur
        auto obj1 = doc.createElement("object");
        obj1.setAttribute("class", "MyCalc");
        obj1.setAttribute("name", "Obj1");
        root.appendChild(obj1);

        auto obj2 = doc.createElement("object");
        obj2.setAttribute("class", "MyTask");
        obj2.setAttribute("name", "Obj2");
        root.appendChild(obj2);

        // Property unter obj1
        properties::appendProperty(obj1, "propA", "int", "42");
        properties::appendProperty(obj1, "propB", "double", "3.14");
    }
};

// Test: findElementsByClass
TEST_F(ModuleUpgradeUtilsTest, FindElementsByClass) {
    QStringList classes{"MyCalc"};
    auto elems = findElementsByClass(root, classes, true);
    ASSERT_EQ(elems.size(), 1);
    EXPECT_EQ(elems[0].attribute("name"), "Obj1");

    QStringList classes2 = {"MyCalc", "MyTask"};
    elems = findElementsByClass(root, classes2, true);
    ASSERT_EQ(elems.size(), 2);
}

// Test: findElementsByAttribute
TEST_F(ModuleUpgradeUtilsTest, FindElementsByAttribute) {
    QStringList vals{"MyTask"};
    auto elems = findElementsByAttribute(root, "class", vals, true);
    ASSERT_EQ(elems.size(), 1);
    EXPECT_EQ(elems[0].attribute("name"), "Obj2");
}

// Test: findParentByAttribute
TEST_F(ModuleUpgradeUtilsTest, FindParentByAttribute) {
    auto propA = properties::propNode("propA", root.firstChildElement("object"));
    auto parent = findParentByAttribute(propA, "class", {"MyCalc"});
    ASSERT_FALSE(parent.isNull());
    EXPECT_EQ(parent.attribute("name"), "Obj1");
}

// Test: properties::updateTypeAndValue
TEST_F(ModuleUpgradeUtilsTest, UpdateTypeAndValue) {
    auto propA = properties::propNode("propA", root.firstChildElement("object"));
    bool ok = properties::updateTypeAndValue(propA, "double", "99.9");
    ASSERT_TRUE(ok);
    EXPECT_EQ(propA.attribute("type"), "double");
    EXPECT_EQ(propA.text(), "99.9");
}

// Test: properties::doubleValue
TEST_F(ModuleUpgradeUtilsTest, DoubleValue) {
    auto obj1 = root.firstChildElement("object");
    double val = properties::doubleValue(obj1, "propB");
    EXPECT_DOUBLE_EQ(val, 3.14);
}

// Test: appendNewGtlabObject
TEST_F(ModuleUpgradeUtilsTest, AppendNewGtlabObject) {
    auto newObj = gt::module_upgrade_utils::appendNewGtlabObject(root, "MyCalc", "NewObj");
    ASSERT_FALSE(newObj.isNull());
    EXPECT_EQ(newObj.attribute("class"), "MyCalc");
    EXPECT_EQ(newObj.attribute("name"), "NewObj");
    EXPECT_FALSE(newObj.attribute("uuid").isEmpty());
}

// Test: addObjectList
TEST_F(ModuleUpgradeUtilsTest, AddObjectList) {
    auto listElem = gt::module_upgrade_utils::addObjectList(root);
    ASSERT_FALSE(listElem.isNull());
    EXPECT_EQ(listElem.tagName(), "objectlist");
}
