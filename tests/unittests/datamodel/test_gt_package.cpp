/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_object.h"

#include "gt_objectfactory.h"
#include "gt_objectio.h"
#include "gt_objectmemento.h"
#include "gt_package.h"

#include <QDir>
#include <QDomDocument>

class TestPackage : public GtPackage
{
public:
    TestPackage() = default;
};

class TestGtPackage : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
        {
            gtObjectFactory->registerClass(
                TestSpecialGtObject::staticMetaObject);
        }
    }

    static QDomElement rootElement(QDomDocument& doc)
    {
        auto root = doc.createElement("package");
        doc.appendChild(root);
        return root;
    }

    static void appendObject(QDomElement& root, const GtObject& object)
    {
        root.appendChild(object.toMemento(false).documentElement());
    }
};

TEST_F(TestGtPackage, readDataAcceptsEmptyRoot)
{
    TestPackage package;
    QDomDocument doc;
    auto root = rootElement(doc);

    EXPECT_TRUE(package.readData(root));
    EXPECT_EQ(package.childCount<GtObject*>(), 0);
}

TEST_F(TestGtPackage, readDataAppendsRestoredChildren)
{
    TestPackage package;

    TestSpecialGtObject sourceChild;
    sourceChild.setObjectName("child");
    sourceChild.setDouble(123.);

    QDomDocument doc;
    auto root = rootElement(doc);
    appendObject(root, sourceChild);

    EXPECT_TRUE(package.readData(root));
    ASSERT_EQ(package.childCount<GtObject*>(), 1);

    auto child = package.findDirectChild<TestSpecialGtObject*>();
    ASSERT_NE(child, nullptr);
    EXPECT_DOUBLE_EQ(child->getDouble(), 123.);
    EXPECT_EQ(child->objectName(), QString("child"));
}

TEST_F(TestGtPackage, readDataRestoresDefaultChildInPlace)
{
    TestPackage package;

    auto* defaultChild = new TestSpecialGtObject;
    defaultChild->setObjectName("child");
    defaultChild->setDefault(true);
    defaultChild->setDouble(1.);
    ASSERT_TRUE(package.appendChild(defaultChild));

    TestSpecialGtObject sourceChild;
    sourceChild.setObjectName("child");
    sourceChild.setDouble(456.);

    QDomDocument doc;
    auto root = rootElement(doc);
    appendObject(root, sourceChild);

    EXPECT_TRUE(package.readData(root));
    EXPECT_EQ(package.childCount<GtObject*>(), 1);
    EXPECT_EQ(package.findDirectChild<TestSpecialGtObject*>(), defaultChild);
    EXPECT_DOUBLE_EQ(defaultChild->getDouble(), 456.);
}

TEST_F(TestGtPackage, readDataRestoresUnknownChildrenAsDummyObjects)
{
    TestPackage package;

    auto memento = GtObjectMemento{}
                       .setClassName("UnknownPackageChild")
                       .setUuid("unknown-child-uuid")
                       .setIdent("unknown-child");

    QDomDocument doc;
    auto root = rootElement(doc);
    root.appendChild(memento.documentElement());

    EXPECT_TRUE(package.readData(root));
    ASSERT_EQ(package.childCount<GtObject*>(), 1);

    auto* child = package.findDirectChild<GtObject*>();
    ASSERT_NE(child, nullptr);
    EXPECT_TRUE(child->isDummy());
    EXPECT_EQ(child->objectName(), QString("unknown-child"));
}


TEST_F(TestGtPackage, saveDataSerializesDirectChildren)
{
    TestPackage package;

    auto* child1 = new TestSpecialGtObject;
    child1->setObjectName("child1");
    child1->setDouble(12.);
    ASSERT_TRUE(package.appendChild(child1));

    auto* child2 = new TestSpecialGtObject;
    child2->setObjectName("child2");
    child2->setDouble(34.);
    ASSERT_TRUE(package.appendChild(child2));

    QDomDocument doc;
    auto root = rootElement(doc);

    EXPECT_TRUE(package.saveData(root, doc));

    auto objectElement = root.firstChildElement("object");
    ASSERT_FALSE(objectElement.isNull());
    GtObjectMemento first(objectElement);
    EXPECT_EQ(first.ident(), QString("child1"));

    objectElement = objectElement.nextSiblingElement("object");
    ASSERT_FALSE(objectElement.isNull());
    GtObjectMemento second(objectElement);
    EXPECT_EQ(second.ident(), QString("child2"));

    EXPECT_TRUE(objectElement.nextSiblingElement("object").isNull());
}

TEST_F(TestGtPackage, miscDataOperationsReturnTrue)
{
    TestPackage package;
    const QDir projectDir;

    EXPECT_TRUE(package.readMiscData(projectDir));
    EXPECT_TRUE(package.saveMiscData(projectDir));
}

TEST_F(TestGtPackage, readDataSkipsNonObjectChildElements)
{
    TestPackage package;

    QDomDocument doc;
    auto root = rootElement(doc);

    // Add some non-object elements that should be skipped (lines 29-30)
    root.appendChild(doc.createElement("metadata"));
    root.appendChild(doc.createElement("comment"));

    // Add actual object
    TestSpecialGtObject sourceChild;
    sourceChild.setObjectName("realchild");
    sourceChild.setDouble(789.);
    appendObject(root, sourceChild);

    EXPECT_TRUE(package.readData(root));
    ASSERT_EQ(package.childCount<GtObject*>(), 1);

    auto child = package.findDirectChild<TestSpecialGtObject*>();
    ASSERT_NE(child, nullptr);
    EXPECT_DOUBLE_EQ(child->getDouble(), 789.);
}

// Test gt_objectio.cpp line 292: aslink="true" set when SaveAsOwnFile=true and IsUnresolved=false
TEST_F(TestGtPackage, saveDataWithSaveAsOwnFileSetsAsLinkAttribute)
{
    TestPackage package;

    auto* child = new TestSpecialGtObject;
    child->setObjectName("linkedchild");
    child->setDouble(42.);
    child->setSaveAsOwnFile(true);
    ASSERT_TRUE(package.appendChild(child));

    QDomDocument doc;
    auto root = rootElement(doc);

    EXPECT_TRUE(package.saveData(root, doc));

    auto objectElement = root.firstChildElement("object");
    ASSERT_FALSE(objectElement.isNull());
    EXPECT_EQ(objectElement.attribute(QStringLiteral("aslink")), QStringLiteral("true"));
}

// Test gt_objectio.cpp lines 356-357: toMemento skips non-object child elements
TEST_F(TestGtPackage, toMementoSkipsNonObjectChildElements)
{
    QDomDocument doc;
    auto root = doc.createElement("object");
    root.setAttribute("class", "TestSpecialGtObject");
    root.setAttribute("name", "parent");
    doc.appendChild(root);

    // Add child objects list with mixed content
    QDomElement childList = doc.createElement("objectlist");
    root.appendChild(childList);

    // Add non-object elements that should be skipped
    childList.appendChild(doc.createElement("metadata"));
    childList.appendChild(doc.createElement("comment"));

    // Add actual object child
    QDomElement childObj = doc.createElement("object");
    childObj.setAttribute("class", "TestSpecialGtObject");
    childObj.setAttribute("name", "realchild");
    childList.appendChild(childObj);

    // Add another non-object element after the real child
    childList.appendChild(doc.createElement("annotation"));

    // GtObjectMemento constructor internally calls GtObjectIO::toMemento
    GtObjectMemento memento(root);

    // Should have exactly 1 child (non-object elements skipped)
    ASSERT_EQ(memento.childObjects.size(), 1);
    EXPECT_EQ(memento.childObjects[0].ident(), QString("realchild"));
}
