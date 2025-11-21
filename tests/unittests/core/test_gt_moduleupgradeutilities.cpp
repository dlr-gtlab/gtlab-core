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
#include <QDebug>
#include "gt_moduleupgradeutilities.h"
#include "gt_xmlutilities.h"


using namespace gt::module_upgrade;

class ModuleUpgradeUtilsTest : public ::testing::Test
{
protected:
    QDomDocument doc;
    QDomElement root;

    void SetUp() override
    {
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
        gt::xml::addNewPropertyElement(obj1, "propA", "int", "42");
        gt::xml::addNewPropertyElement(obj1, "propB", "double", "3.14");
    }

    bool domElementsEqual(const QDomElement& a, const QDomElement& b) const
    {
        if (a.tagName() != b.tagName())
        {
            qWarning() << "Tag names are different:"
                       << a.tagName() << "/" << b.tagName();
            return false;
        }
        if (a.attributes().count() != b.attributes().count())
        {
            qWarning() << "Attribute count is different";
            return false;
        }
        // Attribute vergleichen
        for (int i = 0; i < a.attributes().count(); ++i)
        {
            QDomAttr attrA = a.attributes().item(i).toAttr();
            QString aName = attrA.name();
            QDomAttr attrB = const_cast<QDomElement*>(&b)->attributeNode(aName);
            if (attrB.isNull() || attrA.value() != attrB.value())
            {
                qWarning() << "Error in comparison of " << aName;
                return false;
            }
        }

        // compare text
        if (a.text().trimmed() != b.text().trimmed())
        {
            qWarning() << "Error in comparison of" <<  a.text().trimmed()
                       << "and" << b.text().trimmed();
            return false;
        }


        // Child compare
        QDomNodeList childrenA = a.childNodes();
        QDomNodeList childrenB = b.childNodes();
        if (childrenA.count() != childrenB.count())
        {
            qWarning() << "Child count is different" <<  childrenA.count()
                       << "/" << childrenB.count();
            return false;
        }
        for (int i = 0; i < childrenA.count(); ++i) {
            QDomElement childA = childrenA.at(i).toElement();
            QDomElement childB = childrenB.at(i).toElement();

            if (!childA.isNull() && !childB.isNull())
            {
                if (!domElementsEqual(childA, childB))
                {
                    qWarning() << "Child compared failed";
                    return false;
                }
            }
            else if (childA.isNull() != childB.isNull())
            {
                qWarning() << "There is an invalid child to compare";
                return false;
            }
        }

        return true;
    }
};

// Test: findElementsByClass
TEST_F(ModuleUpgradeUtilsTest, FindElementsByClass)
{
    QStringList classes{"MyCalc"};
    auto elems = findElementsByClass(root, classes);
    ASSERT_EQ(elems.size(), 1);
    EXPECT_EQ(elems[0].attribute("name"), "Obj1");

    QStringList classes2 = {"MyCalc", "MyTask"};
    elems = findElementsByClass(root, classes2);
    ASSERT_EQ(elems.size(), 2);
}

// Test: findElementsByAttribute
TEST_F(ModuleUpgradeUtilsTest, FindElementsByAttribute)
{
    QStringList vals{"MyTask"};
    auto elems = findElementsByAttribute(root, "class", vals);
    ASSERT_EQ(elems.size(), 1);
    EXPECT_EQ(elems[0].attribute("name"), "Obj2");
}

// Test: findParentByAttribute
TEST_F(ModuleUpgradeUtilsTest, FindParentByAttribute)
{
    auto propA = gt::xml::findPropertyElement(root.firstChildElement("object"),
                                              "propA");
    auto parent = gt::xml::findParentByAttribute(propA, "class", {"MyCalc"});
    ASSERT_FALSE(parent.isNull());
    EXPECT_EQ(parent.attribute("name"), "Obj1");
}

// Test: properties::updateTypeAndValue
TEST_F(ModuleUpgradeUtilsTest, UpdateTypeAndValue)
{
    auto propA = gt::xml::findPropertyElement(root.firstChildElement("object"),
                                              "propA");
    bool ok = setPropertyTypeAndValue(propA, "double", "99.9");
    ASSERT_TRUE(ok);
    EXPECT_EQ(propA.attribute("type"), "double");
    EXPECT_EQ(propA.text(), "99.9");
}

// Test: properties::doubleValue
TEST_F(ModuleUpgradeUtilsTest, DoubleValue)
{
    auto obj1 = root.firstChildElement("object");
    double val = *gt::xml::getDoublePropetyElementValue(obj1, "propB");
    EXPECT_DOUBLE_EQ(val, 3.14);
}

// Test: appendNewGtlabObject
TEST_F(ModuleUpgradeUtilsTest, AppendNewGtlabObject)
{
    auto newObj = gt::xml::addObjectElement(root, "MyCalc", "NewObj");
    ASSERT_FALSE(newObj.isNull());
    EXPECT_EQ(newObj.attribute("class"), "MyCalc");
    EXPECT_EQ(newObj.attribute("name"), "NewObj");
    EXPECT_FALSE(newObj.attribute("uuid").isEmpty());
}

// Test: addObjectList
TEST_F(ModuleUpgradeUtilsTest, AddObjectList)
{
    auto listElem = gt::xml::addObjectListElement(root);
    ASSERT_FALSE(listElem.isNull());
    EXPECT_EQ(listElem.tagName(), "objectlist");
}

TEST_F(ModuleUpgradeUtilsTest, PyProcessTest1)
{
    /// This test calls some combined functions to check if the test to
    /// change container property to map style is successful.


    QString oldXML = R"(<?xml version="1.0" encoding="UTF-8"?>
<object class="GtTask" name="New Task" uuid="{e127e6ee-03c7-47e6-8e00-883a17a4f3ce}">
    <objectlist>
        <object class="GtpyScriptCalculator" name="Python Script Editor" uuid="{af82fec5-76db-4a77-bf87-03b04f49de68}">
            <property-container name="input_args">
                <property name="{asdf-dfht-qwer-1234}" type="float">
                    <property name="name" type="QString">input_1</property>
                    <property name="value" type="double">0</property>
                </property>
            </property-container>
            <property-container name="output_args">
                <property name="output_1" type="float">
                    <property name="name" type="QString">output_1</property>
                    <property name="value" type="double">0</property>
                </property>
            </property-container>
         </object>
    </objectlist>
</object>
    )";

    QString newXML = R"(<?xml version="1.0" encoding="UTF-8"?>
<object class="GtTask" name="New Task" uuid="{e127e6ee-03c7-47e6-8e00-883a17a4f3ce}">
    <objectlist>
        <object class="GtpyScriptCalculator" name="Python Script Editor" uuid="{af82fec5-76db-4a77-bf87-03b04f49de68}">
            <property-container name="input_args">
                <property name="input_1" type="float">
                    <property name="value" type="double">0</property>
                </property>
            </property-container>
            <property-container name="output_args">
                <property name="output_1" type="float">
                    <property name="value" type="double">0</property>
                </property>
            </property-container>
         </object>
    </objectlist>
</object>
    )";

    // QDomDocument erzeugen
    QDomDocument doc1;
    QDomDocument doc2;

    QString errorMsg;
    int errorLine, errorColumn;

    bool readOldCheck = doc1.setContent(oldXML, &errorMsg, &errorLine, &errorColumn);

    if (!readOldCheck)
    {
        qWarning() << "Error while parsing xml:" << errorMsg
                   << "Line:" << errorLine << "Column:" << errorColumn;
    }

    ASSERT_TRUE(readOldCheck);

    bool readNewCheck = doc2.setContent(newXML, &errorMsg, &errorLine, &errorColumn);

    if (!readNewCheck)
    {
        qWarning() << "Error while parsing xml:" << errorMsg
                   << "Line:" << errorLine << "Column:" << errorColumn;
    }

    ASSERT_TRUE(readNewCheck);

    QDomElement rootOld = doc1.documentElement();

    QVector<QDomElement> found = gt::module_upgrade::findElementsByClass(
        rootOld, {"GtpyScriptCalculator", "GtpyTask"}, SearchMode::NonRecursive);

    ASSERT_EQ(found.size(), 1);

    QDomElement calc = found[0];
    EXPECT_STREQ(calc.attribute("name").toStdString().c_str(), "Python Script Editor");
    EXPECT_STREQ(calc.attribute("class").toStdString().c_str(), "GtpyScriptCalculator");

    QMap<QString, QString> replaceMap;
    for (QDomElement& elem : found)
    {
        QDomElement c = elem.firstChildElement("property-container");
        while (!c.isNull())
        {
            QString name = c.attribute("name");

            if (name == "input_args" ||
                name == "output_args")
            {
                normalizePropertyContainerId(c, "name", replaceMap);
            }

            c = c.nextSiblingElement("property-container");
        }
    }

    bool comparisonCheck = domElementsEqual(rootOld,
                                            doc2.documentElement());

    if (!comparisonCheck)
    {
        qWarning() << "old: " << rootOld.ownerDocument().toString(4);
        qWarning() << "new: " << doc2.toString(4);
    }

    EXPECT_TRUE(comparisonCheck);
}
