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
#include <gmock/gmock.h>

#include <QDomDocument>
#include <QStringList>
#include <QDebug>
#include "gt_moduleupgradeutilities.h"
#include "gt_xmlutilities.h"

namespace {



QString normalizeWhitespace(const QString& s)
{
    QString out = s.simplified();
    return out;
}

///
/// \brief Whitespace-Insensitive DOM-Vergleichsfunktion
/// \param a - first element to compare
/// \param b - second element to compare
/// \param diff description
/// \param path - error file path
/// \return comapare success
///
bool DomElementsEqualDetailedWSI(const QDomElement& a,
                                 const QDomElement& b,
                                 QString* diff,
                                 const QString& path = "")
{
    QString currentPath = path.isEmpty() ? a.tagName() : path + "/" + a.tagName();

    // Tag compare
    if (a.tagName() != b.tagName()) {
        *diff = QString("Different tag at %1: '%2' vs '%3'")
        .arg(currentPath, a.tagName(), b.tagName());
        return false;
    }

    // attribute compare
    QDomNamedNodeMap attrsA = a.attributes();
    QDomNamedNodeMap attrsB = b.attributes();

    if (attrsA.count() != attrsB.count()) {
        *diff = QString("Different number of attributes at %1").arg(currentPath);
        return false;
    }

    for (int i = 0; i < attrsA.count(); ++i) {
        auto attrA = attrsA.item(i).toAttr();
        auto attrB = const_cast<QDomElement*>(&b)->attributeNode(attrA.name());

        if (attrA.value() != attrB.value()) {
            *diff = QString("Different attribute '%1' at %2: '%3' vs '%4'")
            .arg(attrA.name(), currentPath, attrA.value(), attrB.value());
            return false;
        }
    }

    // Whitespace insensitive text compare
    QString textA = normalizeWhitespace(a.text());
    QString textB = normalizeWhitespace(b.text());

    if (textA != textB) {
        *diff = QString("Different text at %1: '%2' vs '%3'")
        .arg(currentPath, textA, textB);
        return false;
    }

    // Child compare (ignore whitespace-only Childnodes)
    auto nextRelevantChild = [](QDomNode n) {
        while (!n.isNull()) {
            if (n.isElement()) return n;
            if (n.isText() && !n.nodeValue().trimmed().isEmpty()) return n;
            n = n.nextSibling();
        }
        return n; // null
    };

    QDomNode childA = nextRelevantChild(a.firstChild());
    QDomNode childB = nextRelevantChild(b.firstChild());

    while (!childA.isNull() && !childB.isNull()) {

        if (childA.nodeType() != childB.nodeType()) {
            *diff = QString("Different node type at %1").arg(currentPath);
            return false;
        }

        if (childA.isElement()) {
            if (!DomElementsEqualDetailedWSI(childA.toElement(),
                                             childB.toElement(),
                                             diff,
                                             currentPath))
                return false;
        }
        else if (childA.isText()) {
            QString ca = normalizeWhitespace(childA.nodeValue());
            QString cb = normalizeWhitespace(childB.nodeValue());
            if (ca != cb) {
                *diff = QString("Different text node at %1: '%2' vs '%3'")
                .arg(currentPath, ca, cb);
                return false;
            }
        }

        childA = nextRelevantChild(childA.nextSibling());
        childB = nextRelevantChild(childB.nextSibling());
    }

    if (!childA.isNull() || !childB.isNull()) {
        *diff = QString("Different number of child nodes at %1").arg(currentPath);
        return false;
    }

    return true;
}
}
MATCHER_P(DomEqualsWSI, expected, "")
{
    QString diff;
    bool equal = ::DomElementsEqualDetailedWSI(arg, expected, &diff);

    if (!equal)
    {
        *result_listener << "DOM elements differ: " << diff.toStdString();
    }

    return equal;
}


using namespace gt::module_upgrade;

class ModuleUpgradeUtilsTest : public ::testing::Test
{
protected:
    QDomDocument doc2;
    QDomElement root;

    void SetUp() override
    {
        root = doc2.createElement("root");
        doc2.appendChild(root);

        // Beispielstruktur
        auto obj1 = doc2.createElement("object");
        obj1.setAttribute("class", "MyCalc");
        obj1.setAttribute("name", "Obj1");
        root.appendChild(obj1);

        auto obj2 = doc2.createElement("object");
        obj2.setAttribute("class", "MyTask");
        obj2.setAttribute("name", "Obj2");
        root.appendChild(obj2);

        // Property below obj1
        gt::xml::addNewPropertyElement(obj1, "propA", "int", "42");
        gt::xml::addNewPropertyElement(obj1, "propB", "double", "3.14");
    }
};

// Test: setPropertyTypeAndValue
TEST_F(ModuleUpgradeUtilsTest, setPropertyTypeAndValue)
{
    auto propA = gt::xml::findPropertyElement(root.firstChildElement("object"),
                                              "propA");
    bool ok = setPropertyTypeAndValue(propA, "double", "99.9");
    ASSERT_TRUE(ok);
    EXPECT_EQ(propA.attribute("type"), "double");
    EXPECT_EQ(propA.text(), "99.9");
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

    QList<QDomElement> foundCalcs = gt::xml::findObjectElementsByClassName(
        rootOld, "GtpyScriptCalculator");

    ASSERT_EQ(foundCalcs.size(), 1);

    QDomElement calc = foundCalcs[0];
    EXPECT_STREQ(calc.attribute("name").toStdString().c_str(), "Python Script Editor");
    EXPECT_STREQ(calc.attribute("class").toStdString().c_str(), "GtpyScriptCalculator");

    QMap<QString, QString> replaceMap;
    for (QDomElement& elem : foundCalcs)
    {
        QDomElement c = elem.firstChildElement("property-container");
        while (!c.isNull())
        {
            QString name = c.attribute("name");

            if (name == "input_args" ||
                name == "output_args")
            {
                convertPropertyContainerToMap(c, "name", replaceMap);
            }

            c = c.nextSiblingElement("property-container");
        }
    }

    EXPECT_THAT(rootOld, DomEqualsWSI(doc2.documentElement()));
}
