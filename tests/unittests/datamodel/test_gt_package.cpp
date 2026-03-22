/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_object.h"

#include "gt_objectfactory.h"
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
