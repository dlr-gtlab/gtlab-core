/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_objectmementodiff
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_objectmementodiff.h"
#include "gt_objectmemento.h"
#include "gt_object.h"
#include "test_gt_object.h"
#include "gt_objectfactory.h"
#include "gt_datazone0d.h"


/// This is a test fixture that does a init for each test
class TestGtObjectMementoDiff : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        obj1.setObjectName("Object1");
        obj1.setFactory(gtObjectFactory);

        obj2.setObjectName("Object2");
        obj2.setFactory(gtObjectFactory);


        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
        {
            gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);
        }
    }

    virtual void TearDown()
    {
    }

    TestSpecialGtObject obj1;
    TestSpecialGtObject obj2;
    TestSpecialGtObject obj3;
    TestSpecialGtObject obj4;
};

TEST_F(TestGtObjectMementoDiff, constructors)
{
    GtObjectMemento memento1;
    GtObjectMemento memento2;

    GtObjectMementoDiff diff(memento1, memento2);

    ASSERT_TRUE(memento1.isNull());
    ASSERT_TRUE(memento2.isNull());
    ASSERT_TRUE(diff.isNull());

    GtObjectMemento memento3 = obj1.toMemento();

    ASSERT_FALSE(memento3.isNull());
}

TEST_F(TestGtObjectMementoDiff, hasObjectTreeChanges)
{
    GtObjectMemento mem1 = obj1.toMemento();
    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    ASSERT_FALSE(GtObjectMementoDiff(mem1, mem2).hasObjectTreeChanges());

    obj1.appendChild(&obj2);

    mem2 = obj1.toMemento();

    ASSERT_TRUE(GtObjectMementoDiff(mem1, mem2).hasObjectTreeChanges());

    mem1 = obj1.toMemento();

    ASSERT_FALSE(GtObjectMementoDiff(mem1, mem2).hasObjectTreeChanges());

    obj2.setParent(nullptr);

    mem2 = obj1.toMemento();

    ASSERT_TRUE(GtObjectMementoDiff(mem1, mem2).hasObjectTreeChanges());
}

TEST_F(TestGtObjectMementoDiff, differentRoots)
{
    GtObjectMemento memento1  = obj1.toMemento();
    GtObjectMemento memento2 = obj2.toMemento();

    ASSERT_FALSE(memento1.isNull());
    ASSERT_FALSE(memento2.isNull());

    GtObjectMementoDiff diff(memento1, memento2);
    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();
    ASSERT_FALSE(root.isNull());

    ASSERT_STREQ(root.tagName().toStdString().c_str(),
                 "ROOT_ERROR");
}

TEST_F(TestGtObjectMementoDiff, mementosIdentical)
{
    GtObjectMemento mem1 = obj1.toMemento();
    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);
    ASSERT_TRUE(diff.isNull());
}

TEST_F(TestGtObjectMementoDiff, doublePropertyChange)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.setDouble(123.4);

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    // Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());

    // property diff check
    QDomElement propChange = root.firstChildElement("diff-property-change");
    ASSERT_FALSE(propChange.isNull());

    QString propName = propChange.attribute("name");
    QString propType = propChange.attribute("type");

    ASSERT_STREQ(propName.toStdString().c_str(), "doubleProp");
    ASSERT_STREQ(propType.toStdString().c_str(), "double");

    QDomElement oldValue = propChange.firstChildElement("oldVal");
    QDomElement newValue = propChange.firstChildElement("newVal");

    ASSERT_FALSE(oldValue.isNull());
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(newValue.text().toStdString().c_str(), "123.4");
    ASSERT_STREQ(oldValue.text().toStdString().c_str(), "0");

    // apply diff
    obj1.setDouble(0.0);

    ASSERT_DOUBLE_EQ(obj1.getDouble(), 0.0);

    ASSERT_TRUE(obj1.applyDiff(diff));
    ASSERT_FALSE(obj2.applyDiff(diff));

    ASSERT_DOUBLE_EQ(obj1.getDouble(), 123.4);
    ASSERT_DOUBLE_EQ(obj2.getDouble(), 0.0);

    // revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));
    ASSERT_FALSE(obj2.revertDiff(diff));

    ASSERT_DOUBLE_EQ(obj1.getDouble(), 0.0);
    ASSERT_DOUBLE_EQ(obj2.getDouble(), 0.0);
}

TEST_F(TestGtObjectMementoDiff, intPropertyChange)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.setInt(2);

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());


    //property diff check
    QDomElement propChange = root.firstChildElement("diff-property-change");
    ASSERT_FALSE(propChange.isNull());

    QString propName = propChange.attribute("name");
    QString propType = propChange.attribute("type");

    ASSERT_STREQ(propName.toStdString().c_str(), "intProp");
    ASSERT_STREQ(propType.toStdString().c_str(), "int");

    QDomElement oldValue = propChange.firstChildElement("oldVal");
    QDomElement newValue = propChange.firstChildElement("newVal");

    ASSERT_FALSE(oldValue.isNull());
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(newValue.text().toStdString().c_str(), "2");
    ASSERT_STREQ(oldValue.text().toStdString().c_str(), "0");

    // apply diff
    obj1.setInt(0);

    ASSERT_EQ(obj1.getInt(), 0);

    ASSERT_TRUE(obj1.applyDiff(diff));
    ASSERT_FALSE(obj2.applyDiff(diff));

    ASSERT_EQ(obj1.getInt(), 2);
    ASSERT_EQ(obj2.getInt(), 0);

    // revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));
    ASSERT_FALSE(obj2.revertDiff(diff));

    ASSERT_EQ(obj1.getInt(), 0);
    ASSERT_EQ(obj2.getInt(), 0);
}

TEST_F(TestGtObjectMementoDiff, boolPropertyChange)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.setBool(true);

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());


    //property diff check
    QDomElement propChange = root.firstChildElement("diff-property-change");
    ASSERT_FALSE(propChange.isNull());

    QString propName = propChange.attribute("name");
    QString propType = propChange.attribute("type");

    ASSERT_STREQ(propName.toStdString().c_str(), "boolProp");
    ASSERT_STREQ(propType.toStdString().c_str(), "bool");

    QDomElement oldValue = propChange.firstChildElement("oldVal");
    QDomElement newValue = propChange.firstChildElement("newVal");

    ASSERT_FALSE(oldValue.isNull());
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(newValue.text().toStdString().c_str(), "true");
    ASSERT_STREQ(oldValue.text().toStdString().c_str(), "false");

    // apply diff
    obj1.setBool(false);

    ASSERT_FALSE(obj1.getBool());

    ASSERT_TRUE(obj1.applyDiff(diff));
    ASSERT_FALSE(obj2.applyDiff(diff));

    ASSERT_TRUE(obj1.getBool());
    ASSERT_FALSE(obj2.getBool());

    // revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));
    ASSERT_FALSE(obj2.revertDiff(diff));

    ASSERT_FALSE(obj1.getBool());
    ASSERT_FALSE(obj2.getBool());
}

TEST_F(TestGtObjectMementoDiff, stringPropertyChange)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.setString("testString");

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());


    //property diff check
    QDomElement propChange = root.firstChildElement("diff-property-change");
    ASSERT_FALSE(propChange.isNull());

    QString propName = propChange.attribute("name");
    QString propType = propChange.attribute("type");

    ASSERT_STREQ(propName.toStdString().c_str(), "strProp");
    ASSERT_STREQ(propType.toStdString().c_str(), "QString");

    QDomElement oldValue = propChange.firstChildElement("oldVal");
    QDomElement newValue = propChange.firstChildElement("newVal");

    ASSERT_FALSE(oldValue.isNull());
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(newValue.text().toStdString().c_str(), "testString");
    ASSERT_STREQ(oldValue.text().toStdString().c_str(), "Test");

    // apply diff
    obj1.setString("Test");

    ASSERT_STREQ(obj1.getString().toStdString().c_str(), "Test");

    ASSERT_TRUE(obj1.applyDiff(diff));
    ASSERT_FALSE(obj2.applyDiff(diff));

    ASSERT_STREQ(obj1.getString().toStdString().c_str(), "testString");
    ASSERT_STREQ(obj2.getString().toStdString().c_str(), "Test");

    // revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));
    ASSERT_FALSE(obj2.revertDiff(diff));

    ASSERT_STREQ(obj1.getString().toStdString().c_str(), "Test");
    ASSERT_STREQ(obj2.getString().toStdString().c_str(), "Test");
}

TEST_F(TestGtObjectMementoDiff, objectAdd)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.appendChild(&obj2);

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());

    //property diff check
    QDomElement objAdd = root.firstChildElement("diff-object-add");
    ASSERT_FALSE(objAdd.isNull());

    QDomElement indChanged = root.firstChildElement("diff-index-changed");
    ASSERT_TRUE(indChanged.isNull());

    QDomElement addedObject = objAdd.firstChildElement("object");
    ASSERT_FALSE(addedObject.isNull());

    ASSERT_STREQ(addedObject.attribute("uuid").toStdString().c_str(),
                 obj2.uuid().toStdString().c_str());
    ASSERT_STREQ(addedObject.attribute("name").toStdString().c_str(),
                 obj2.objectName().toStdString().c_str());
    ASSERT_STREQ(addedObject.attribute("class").toStdString().c_str(),
                 obj2.metaObject()->className());

    GtObjectMemento addMemento(addedObject);

    ASSERT_STREQ(addMemento.uuid().toStdString().c_str(),
                 obj2.toMemento().uuid().toStdString().c_str());

    ASSERT_STREQ(addMemento.className().toStdString().c_str(),
                 obj2.toMemento().className().toStdString().c_str());

    // apply diff
    obj2.setParent(nullptr);

    ASSERT_TRUE(obj1.applyDiff(diff));

    GtObjectMemento mem3 = obj1.toMemento();

    ASSERT_TRUE(GtObjectMementoDiff(mem2, mem3).isNull());

    // revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));

    mem2 = obj1.toMemento();

    diff = GtObjectMementoDiff(mem1, mem2);

    ASSERT_TRUE(diff.isNull());
}

TEST_F(TestGtObjectMementoDiff, objectRemove)
{
    GtObjectMemento mem0 = obj1.toMemento();

    ASSERT_TRUE(obj1.findDirectChildren<TestSpecialGtObject*>().size() == 0);

    TestSpecialGtObject* newTestObj = new TestSpecialGtObject();

    obj1.appendChild(newTestObj);

    ASSERT_TRUE(obj1.findDirectChildren<TestSpecialGtObject*>().size() == 1);

    GtObjectMemento mem1 = obj1.toMemento();
    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem0.isNull());
    ASSERT_FALSE(mem1.isNull());

    GtObjectMementoDiff diff(mem1, mem0);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 obj1.objectName().toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());

    // Removed Object Node Check
    QDomElement objRem = root.firstChildElement("diff-object-remove");
    ASSERT_FALSE(objRem.isNull());

    QDomElement indChanged = root.firstChildElement("diff-index-changed");
    ASSERT_TRUE(indChanged.isNull());

    QDomElement removedObject = objRem.firstChildElement("object");
    ASSERT_FALSE(removedObject.isNull());

    ASSERT_STREQ(removedObject.attribute("uuid").toStdString().c_str(),
                 newTestObj->uuid().toStdString().c_str());
    ASSERT_STREQ(removedObject.attribute("name").toStdString().c_str(),
                 newTestObj->objectName().toStdString().c_str());
    ASSERT_STREQ(removedObject.attribute("class").toStdString().c_str(),
                 newTestObj->metaObject()->className());

    GtObjectMemento remMemento(removedObject);

    ASSERT_STREQ(remMemento.uuid().toStdString().c_str(),
                 newTestObj->toMemento().uuid().toStdString().c_str());

    ASSERT_STREQ(remMemento.className().toStdString().c_str(),
                 newTestObj->toMemento().className().toStdString().c_str());

//     apply diff
    ASSERT_TRUE(obj1.applyDiff(diff));

    ASSERT_TRUE(obj1.findDirectChildren<TestSpecialGtObject*>().size() == 0);

    mem1 = obj1.toMemento();

    ASSERT_TRUE(GtObjectMementoDiff(mem0, mem1).isNull());

//     revert diff
    ASSERT_TRUE(obj1.revertDiff(diff));

    ASSERT_TRUE(obj1.findDirectChildren<TestSpecialGtObject*>().size() == 1);

    mem1 = obj1.toMemento();

    ASSERT_TRUE(GtObjectMementoDiff(mem1, mem2).isNull());

}

TEST_F(TestGtObjectMementoDiff, indexChecks)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.appendChild(&obj2);
    obj1.appendChild(&obj3);

    ASSERT_TRUE(obj1.insertChild(1, &obj4));

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Adding and inserting elements
    QDomElement objAdd1 = root.firstChildElement("diff-object-add");
    QDomElement objAdd2 = objAdd1.nextSiblingElement("diff-object-add");
    QDomElement objAdd3 = objAdd2.nextSiblingElement("diff-object-add");

    ASSERT_FALSE(objAdd1.isNull());
    ASSERT_FALSE(objAdd2.isNull());
    ASSERT_FALSE(objAdd3.isNull());

    ASSERT_STREQ(objAdd1.attribute("index").toStdString().c_str(), "0");
    ASSERT_STREQ(objAdd1.firstChildElement("object").attribute("uuid")
                 .toStdString().c_str(), obj2.uuid().toStdString().c_str());

    ASSERT_STREQ(objAdd2.attribute("index").toStdString().c_str(), "1");
    ASSERT_STREQ(objAdd2.firstChildElement("object").attribute("uuid")
                 .toStdString().c_str(), obj4.uuid().toStdString().c_str());

    ASSERT_STREQ(objAdd3.attribute("index").toStdString().c_str(), "2");
    ASSERT_STREQ(objAdd3.firstChildElement("object").attribute("uuid")
                 .toStdString().c_str(), obj3.uuid().toStdString().c_str());

    obj2.setParent(nullptr);
    obj3.setParent(nullptr);
    obj4.setParent(nullptr);

    ASSERT_TRUE(obj1.applyDiff(diff));

    QList<TestSpecialGtObject*> children =
            obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.size() == 3);

    ASSERT_STREQ(children.first()->uuid().toStdString().c_str(),
            obj2.uuid().toStdString().c_str());
    ASSERT_STREQ(children[1]->uuid().toStdString().c_str(),
            obj4.uuid().toStdString().c_str());
    ASSERT_STREQ(children.last()->uuid().toStdString().c_str(),
            obj3.uuid().toStdString().c_str());

    ASSERT_TRUE(obj1.revertDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.isEmpty());

    obj2.setParent(nullptr);
    obj3.setParent(nullptr);
    obj4.setParent(nullptr);


    //Removing elements
    obj1.appendChild(&obj2);
    obj1.appendChild(&obj3);

    TestSpecialGtObject* newTestObj = new TestSpecialGtObject();

    ASSERT_TRUE(obj1.insertChild(1, newTestObj));

    mem2 = obj1.toMemento();

    ASSERT_FALSE(mem2.isNull());

    newTestObj->setParent(nullptr);

    GtObjectMemento mem3 = obj1.toMemento();

    diff = GtObjectMementoDiff(mem2, mem3);

    ASSERT_TRUE(obj1.insertChild(1, newTestObj));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.size() == 3);

    ASSERT_STREQ(children.first()->uuid().toStdString().c_str(),
            obj2.uuid().toStdString().c_str());
    ASSERT_STREQ(children[1]->uuid().toStdString().c_str(),
            newTestObj->uuid().toStdString().c_str());
    ASSERT_STREQ(children.last()->uuid().toStdString().c_str(),
            obj3.uuid().toStdString().c_str());

    root = diff.documentElement();
    QDomElement objRem = root.firstChildElement("diff-object-remove");

    ASSERT_FALSE(objRem.isNull());
    ASSERT_STREQ(objRem.attribute("index").toStdString().c_str(), "1");
    ASSERT_STREQ(objRem.firstChildElement("object").attribute("uuid")
                 .toStdString().c_str(), newTestObj->uuid().toStdString().c_str());


    ASSERT_TRUE(obj1.applyDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.size() == 2);

    ASSERT_STREQ(children.first()->uuid().toStdString().c_str(),
            obj2.uuid().toStdString().c_str());
    ASSERT_STREQ(children.last()->uuid().toStdString().c_str(),
            obj3.uuid().toStdString().c_str());
}

TEST_F(TestGtObjectMementoDiff, moreIndexChecks)
{
    qDebug() << "moreIndexChecks";

    // create some test objects
    TestSpecialGtObject* tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    QString uuid2 = tobj2->uuid();
    TestSpecialGtObject* tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    QString uuid3 = tobj3->uuid();
    TestSpecialGtObject* tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    QString uuid4 = tobj4->uuid();
    TestSpecialGtObject* tobj5 = new TestSpecialGtObject();
    tobj5->setObjectName("O5");
    QString uuid5 = tobj5->uuid();
    TestSpecialGtObject* tobj6 = new TestSpecialGtObject();
    tobj6->setObjectName("O6");
    QString uuid6 = tobj6->uuid();

    // base object is obj1
    GtObjectMemento mem1 = obj1.toMemento();
    ASSERT_FALSE(mem1.isNull());

    obj1.appendChild(tobj2);
    obj1.appendChild(tobj3);
    ASSERT_TRUE(obj1.insertChild(1, tobj4));

    QList<TestSpecialGtObject*> children =
            obj1.findDirectChildren<TestSpecialGtObject*>();
    QStringList ids;
    ids << "O2" << "O4" << "O3";
    QStringList uuids;
    uuids << uuid2 << uuid4 << uuid3;

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    GtObjectMemento mem2 = obj1.toMemento();
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);
    ASSERT_FALSE(diff.isNull());
    qDebug() << "moreIndexChecks diff:";
    qDebug() << diff.toByteArray();

    // add and remove multiple children at once
    tobj2->setParent(nullptr);
    delete tobj2;
    ASSERT_TRUE(obj1.insertChild(2, tobj5));
    obj1.appendChild(tobj6);

    children = obj1.findDirectChildren<TestSpecialGtObject*>();
    ids.clear();
    ids << "O4" << "O3" << "O5" << "O6";
    uuids.clear();
    uuids << uuid4 << uuid3 << uuid5 << uuid6;

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    GtObjectMemento mem3 = obj1.toMemento();
    ASSERT_FALSE(mem3.isNull());

    GtObjectMementoDiff diff2(mem2, mem3);
    ASSERT_FALSE(diff2.isNull());
    qDebug() << "moreIndexChecks diff2:";
    qDebug() << diff2.toByteArray();

    // revert once
    ASSERT_TRUE(obj1.revertDiff(diff2));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();
    ids.clear();
    ids << "O2" << "O4" << "O3";
    uuids.clear();
    uuids << uuid2 << uuid4 << uuid3;

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }


    // check result
    GtObjectMemento mem2_ = obj1.toMemento();
    ASSERT_FALSE(mem2_.isNull());
    GtObjectMementoDiff empty2(mem2,mem2_);
    qDebug() << "moreIndexChecks revert diff2:";
    qDebug() << empty2.toByteArray();
    ASSERT_TRUE(empty2.isNull()); // isNull is true if indices are the same, toByteArray can be empty anyway...

    // revert twice
    ASSERT_TRUE(obj1.revertDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.isEmpty());

    // check result
    GtObjectMemento mem1_ = obj1.toMemento();
    ASSERT_FALSE(mem1_.isNull());
    GtObjectMementoDiff empty1(mem1,mem1_);
    qDebug() << "moreIndexChecks revert diff:";
    qDebug() << empty1.toByteArray();
    ASSERT_TRUE(empty1.isNull()); // isNull is true if indices are the same, toByteArray can be empty anyway...

    // redo once
    ASSERT_TRUE(obj1.applyDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();
    ids.clear();
    ids << "O2" << "O4" << "O3";
    uuids.clear();
    uuids << uuid2 << uuid4 << uuid3;

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    // check result
    GtObjectMemento mem2__ = obj1.toMemento();
    ASSERT_FALSE(mem2__.isNull());
    GtObjectMementoDiff empty2_(mem2,mem2__);
    qDebug() << "moreIndexChecks reapply diff:";
    qDebug() << empty2_.toByteArray();
    ASSERT_TRUE(empty2_.isNull()); // isNull is true if indices are the same, toByteArray can be empty anyway...

    // redo twice
    ASSERT_TRUE(obj1.applyDiff(diff2));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();
    ids.clear();
    ids << "O4" << "O3" << "O5" << "O6";
    uuids.clear();
    uuids << uuid4 << uuid3 << uuid5 << uuid6;

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    // check result
    GtObjectMemento mem3_ = obj1.toMemento();
    ASSERT_FALSE(mem3_.isNull());
    GtObjectMementoDiff empty3(mem3,mem3_);
    qDebug() << "moreIndexChecks reapply diff2:";
    qDebug() << empty3.toByteArray();
    //qDebug() << "moreIndexChecks mem3:";
    //qDebug() << mem3.toByteArray();
    //qDebug() << "moreIndexChecks mem3_:";
    //qDebug() << mem3_.toByteArray();
    ASSERT_TRUE(empty3.isNull()); // isNull is true if indices are the same, toByteArray can be empty anyway...
}

TEST_F(TestGtObjectMementoDiff, undoRedoIndexChecks)
{
    qDebug() << "undoRedoIndexChecks";
    // create some test objects
    TestSpecialGtObject* tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    TestSpecialGtObject* tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    TestSpecialGtObject* tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    TestSpecialGtObject* tobj5 = new TestSpecialGtObject();
    tobj5->setObjectName("O5");
    TestSpecialGtObject* tobj6 = new TestSpecialGtObject();
    tobj6->setObjectName("O6");

    obj1.appendChild(tobj2);
    obj1.appendChild(tobj3);
    obj1.appendChild(tobj4);
    obj1.appendChild(tobj5);
    obj1.appendChild(tobj6);

    QList<TestSpecialGtObject*> children =
            obj1.findDirectChildren<TestSpecialGtObject*>();
    QStringList ids;
    ids << "O2" << "O3" << "O4" << "O5" << "O6";
    QStringList uuids;
    uuids << tobj2->uuid() << tobj3->uuid() << tobj4->uuid() <<
             tobj5->uuid() << tobj6->uuid();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

//    // base object is obj1
    GtObjectMemento mem1 = obj1.toMemento();
    ASSERT_FALSE(mem1.isNull());

    TestSpecialGtObject* cobj =
            mem1.restore<TestSpecialGtObject*>(gtObjectFactory);

    ASSERT_TRUE(cobj != nullptr);

    children = cobj->findDirectChildren<TestSpecialGtObject*>();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    qDeleteAll(children);

    children = cobj->findDirectChildren<TestSpecialGtObject*>();

    ASSERT_TRUE(children.isEmpty());

    tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    tobj5 = new TestSpecialGtObject();
    tobj5->setObjectName("O5");
    tobj6 = new TestSpecialGtObject();
    tobj6->setObjectName("O6");

    cobj->appendChild(tobj2);
    cobj->appendChild(tobj3);
    cobj->appendChild(tobj4);
    cobj->appendChild(tobj5);
    cobj->appendChild(tobj6);

    children = cobj->findDirectChildren<TestSpecialGtObject*>();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STRNE(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    GtObjectMemento mem2 = cobj->toMemento();
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    ASSERT_TRUE(obj1.applyDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STRNE(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    ASSERT_TRUE(obj1.revertDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }
}

TEST_F(TestGtObjectMementoDiff, moreUndoRedoIndexChecks)
{
    // create some test objects
    TestSpecialGtObject* tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    TestSpecialGtObject* tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    TestSpecialGtObject* tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    TestSpecialGtObject* tobj5 = new TestSpecialGtObject();
    tobj5->setObjectName("O5");
    TestSpecialGtObject* tobj6 = new TestSpecialGtObject();
    tobj6->setObjectName("O6");

    obj1.appendChild(tobj2);
    obj1.appendChild(tobj3);
    obj1.appendChild(tobj4);
    obj1.appendChild(tobj5);
    obj1.appendChild(tobj6);

    QList<TestSpecialGtObject*> children =
            obj1.findDirectChildren<TestSpecialGtObject*>();
    QStringList ids;
    ids << "O2" << "O3" << "O4" << "O5" << "O6";
    QStringList uuids;
    uuids << tobj2->uuid() << tobj3->uuid() << tobj4->uuid() <<
             tobj5->uuid() << tobj6->uuid();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }

    // base object is obj1
    GtObjectMemento mem1 = obj1.toMemento();
    ASSERT_FALSE(mem1.isNull());

    delete tobj4;
    delete tobj3;

    GtObjectMemento mem2 = obj1.toMemento();
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    ASSERT_TRUE(obj1.revertDiff(diff));

    children = obj1.findDirectChildren<TestSpecialGtObject*>();

    ASSERT_EQ(children.size(), ids.size());

    for (int i = 0; i < children.size(); i++)
    {
        ASSERT_STREQ(children[i]->objectName().toStdString().c_str(),
                     ids[i].toStdString().c_str());
        ASSERT_STREQ(children[i]->uuid().toStdString().c_str(),
                     uuids[i].toStdString().c_str());
    }
}

TEST_F(TestGtObjectMementoDiff, attrChange)
{
    GtObjectMemento mem1 = obj1.toMemento();
    QString oldObjName = obj1.objectName();

    obj1.setObjectName("newObjName");

    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_FALSE(mem1.isNull());
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    QDomElement root = diff.documentElement();

    //Root Object check
    ASSERT_STREQ(root.tagName().toStdString().c_str(), "object");
    ASSERT_FALSE(root.attribute("uuid").isEmpty());
    ASSERT_STREQ(root.attribute("uuid").toStdString().c_str(),
                 obj1.uuid().toStdString().c_str());
    ASSERT_STREQ(root.attribute("name").toStdString().c_str(),
                 oldObjName.toStdString().c_str());
    ASSERT_STREQ(root.attribute("class").toStdString().c_str(),
                 obj1.metaObject()->className());

    //attribute diff check
    QDomElement attrChange = root.firstChildElement("diff-attribute-change");
    ASSERT_FALSE(attrChange.isNull());

    QDomElement oldValue = attrChange.firstChildElement("oldVal");
    ASSERT_FALSE(oldValue.isNull());

    QDomElement newValue = attrChange.firstChildElement("newVal");
    ASSERT_FALSE(newValue.isNull());

    ASSERT_STREQ(oldValue.text().toStdString().c_str(),
                 oldObjName.toStdString().c_str());

    ASSERT_STREQ(newValue.text().toStdString().c_str(), "newObjName");
}

TEST_F(TestGtObjectMementoDiff, appendDiff)
{
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.appendChild(&obj2);

    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);
    GtObjectMementoDiff diff0(mem1, mem2);

    ASSERT_EQ(diff.numberOfDiffSteps(), 1);
    ASSERT_EQ(diff0.numberOfDiffSteps(), 1);

    // empty diff
    GtObjectMementoDiff diff2;

    ASSERT_FALSE(diff.appendDiff(diff2));
    ASSERT_EQ(diff.numberOfDiffSteps(), 1);
    ASSERT_EQ(diff2.numberOfDiffSteps(), 0);

    diff2 << diff;

    ASSERT_EQ(diff2.numberOfDiffSteps(), 1);

    // full diff
    obj2.setParent(nullptr);
    obj1.setDouble(47);

    GtObjectMementoDiff diff3(mem1, obj1.toMemento());

    ASSERT_TRUE(diff.appendDiff(diff3));
    ASSERT_EQ(diff.numberOfDiffSteps(), 2);

    // streaming operator
    diff0 << diff3;
    ASSERT_EQ(diff0.numberOfDiffSteps(), 2);
}

TEST_F(TestGtObjectMementoDiff, numberOfDiffSteps)
{
    TestSpecialGtObject obj3;
    TestSpecialGtObject obj4;
    TestSpecialGtObject obj5;

    obj1.appendChild(&obj2);
    obj2.appendChild(&obj3);

    GtObjectMemento mem1 = obj1.toMemento();
    GtObjectMemento mem2 = obj1.toMemento();

    ASSERT_EQ(GtObjectMementoDiff(mem1, mem2).numberOfDiffSteps(), 0);


    obj2.appendChild(&obj4);

    mem2 = obj1.toMemento();

    ASSERT_EQ(GtObjectMementoDiff(mem1, mem2).numberOfDiffSteps(), 1);

    obj3.appendChild(&obj5);

    GtObjectMemento mem3 = obj1.toMemento();

    ASSERT_EQ(GtObjectMementoDiff(mem1, mem3).numberOfDiffSteps(), 2);
}

TEST_F(TestGtObjectMementoDiff, objectAddBranch)
{
    GtObjectMementoDiff diff = GtObjectMementoDiff::objectAddBranch(&obj2,
                                                                    &obj1);

    ASSERT_FALSE(diff.isNull());
}

TEST_F(TestGtObjectMementoDiff, objectRemoveBranch)
{
    GtObjectMementoDiff diff = GtObjectMementoDiff::objectRemoveBranch(&obj2,
                                                                       &obj1);

    ASSERT_FALSE(diff.isNull());
}

TEST_F(TestGtObjectMementoDiff, doublePropertyListChange)
{
    ASSERT_EQ(obj1.getDoubleVec().size(), 0);

    GtObjectMemento mem1 = obj1.toMemento();
    obj1.setDoubleVec(QVector<double>() << 0.1 << 0.2);

    ASSERT_EQ(obj1.getDoubleVec().size(), 2);

    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    obj1.setDoubleVec(QVector<double>() << 0.3 << 0.4 << 0.5);

    ASSERT_EQ(obj1.getDoubleVec().size(), 3);

    GtObjectMemento mem3 = obj1.toMemento();

    GtObjectMementoDiff diff2(mem2, mem3);

    ASSERT_FALSE(diff2.isNull());
}

TEST_F(TestGtObjectMementoDiff, dataZone0DChange)
{
    /*
    GtDataZone0D dataZone;

    dataZone.setParams(QStringList() << "a" << "b" << "b");
    dataZone.setUnits(QStringList() << "[a]" << "[b]" << "[c]");
    dataZone.setValues(QVector<double>() << 0.577965226358801 <<
                       11.8776138797173 << 0.930368302276224);

    ASSERT_EQ(dataZone.values().size(), 3);

    GtObjectMemento mem1 = dataZone.toMemento();

    dataZone.setValues(QVector<double>() << 0.577777873968311 <<
                       11.8898074392412 << 0.930454284524836);

    ASSERT_EQ(dataZone.values().size(), 3);

    GtObjectMemento mem2 = dataZone.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());

    ASSERT_TRUE(dataZone.revertDiff(diff));

    auto dataZoneValues = dataZone.values();
    ASSERT_DOUBLE_EQ(dataZoneValues[0], 0.577965226358801);
    ASSERT_DOUBLE_EQ(dataZoneValues[1], 11.8776138797173);
    ASSERT_DOUBLE_EQ(dataZoneValues[2], 0.930368302276224);
    */
}

TEST_F(TestGtObjectMementoDiff, objectNameChange)
{
    obj1.setObjectName("before");
    ASSERT_STREQ(obj1.objectName().toStdString().c_str(), "before");
    GtObjectMemento mem1 = obj1.toMemento();

    obj1.setObjectName("after");
    ASSERT_STREQ(obj1.objectName().toStdString().c_str(), "after");
    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());
    ASSERT_TRUE(obj1.revertDiff(diff));
    ASSERT_STREQ(obj1.objectName().toStdString().c_str(), "before");
    ASSERT_TRUE(obj1.applyDiff(diff));
    ASSERT_STREQ(obj1.objectName().toStdString().c_str(), "after");
}

TEST_F(TestGtObjectMementoDiff, childChildRedoUndo)
{
    obj1.setObjectName("O1");

    TestSpecialGtObject* tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    obj1.appendChild(tobj2);

    TestSpecialGtObject* tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    obj1.appendChild(tobj3);

    TestSpecialGtObject* tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    tobj3->appendChild(tobj4);

    GtObjectList o1Childs = obj1.findDirectChildren<GtObject*>();
    GtObjectList o3Childs = tobj3->findDirectChildren<GtObject*>();

    ASSERT_FALSE(o1Childs.isEmpty());
    ASSERT_FALSE(o3Childs.isEmpty());

    ASSERT_EQ(o1Childs.size(), 2);
    ASSERT_EQ(o3Childs.size(), 1);

    ASSERT_STREQ(o1Childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(o1Childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(o3Childs[0]->objectName().toStdString().c_str(), "O4");

    GtObjectMemento mem1 = obj1.toMemento();

    tobj4->setParent(nullptr);
    obj1.appendChild(tobj4);

    o1Childs = obj1.findDirectChildren<GtObject*>();
    o3Childs = tobj3->findDirectChildren<GtObject*>();

    ASSERT_FALSE(o1Childs.isEmpty());
    ASSERT_TRUE(o3Childs.isEmpty());

    ASSERT_EQ(o1Childs.size(), 3);

    ASSERT_STREQ(o1Childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(o1Childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(o1Childs[2]->objectName().toStdString().c_str(), "O4");

    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());
    ASSERT_TRUE(obj1.revertDiff(diff));

    o1Childs = obj1.findDirectChildren<GtObject*>();
    o3Childs = tobj3->findDirectChildren<GtObject*>();

    ASSERT_FALSE(o1Childs.isEmpty());
    ASSERT_FALSE(o3Childs.isEmpty());

    ASSERT_EQ(o1Childs.size(), 2);
    ASSERT_EQ(o3Childs.size(), 1);

    ASSERT_STREQ(o1Childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(o1Childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(o3Childs[0]->objectName().toStdString().c_str(), "O4");

    ASSERT_TRUE(obj1.applyDiff(diff));

    o1Childs = obj1.findDirectChildren<GtObject*>();
    o3Childs = tobj3->findDirectChildren<GtObject*>();

    ASSERT_FALSE(o1Childs.isEmpty());
    ASSERT_TRUE(o3Childs.isEmpty());

    ASSERT_EQ(o1Childs.size(), 3);

    ASSERT_STREQ(o1Childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(o1Childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(o1Childs[2]->objectName().toStdString().c_str(), "O4");
}

TEST_F(TestGtObjectMementoDiff, childIndexChanges)
{
    // create some test objects
    TestSpecialGtObject* tobj2 = new TestSpecialGtObject();
    tobj2->setObjectName("O2");
    obj1.appendChild(tobj2);
    TestSpecialGtObject* tobj3 = new TestSpecialGtObject();
    tobj3->setObjectName("O3");
    obj1.appendChild(tobj3);
    TestSpecialGtObject* tobj4 = new TestSpecialGtObject();
    tobj4->setObjectName("O4");
    obj1.appendChild(tobj4);

    GtObjectList childs = obj1.findDirectChildren<GtObject*>();

    ASSERT_EQ(childs.size(), 3);

    ASSERT_STREQ(childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(childs[2]->objectName().toStdString().c_str(), "O4");

    GtObjectMemento mem1 = obj1.toMemento();

    tobj3->setParent(nullptr);
    tobj4->setParent(nullptr);

    obj1.appendChild(tobj4);
    obj1.appendChild(tobj3);

    childs = obj1.findDirectChildren<GtObject*>();

    ASSERT_EQ(childs.size(), 3);

    ASSERT_STREQ(childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(childs[1]->objectName().toStdString().c_str(), "O4");
    ASSERT_STREQ(childs[2]->objectName().toStdString().c_str(), "O3");

    GtObjectMemento mem2 = obj1.toMemento();

    GtObjectMementoDiff diff(mem1, mem2);

    ASSERT_FALSE(diff.isNull());
    ASSERT_TRUE(obj1.revertDiff(diff));

    childs = obj1.findDirectChildren<GtObject*>();

    ASSERT_EQ(childs.size(), 3);

    ASSERT_STREQ(childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(childs[1]->objectName().toStdString().c_str(), "O3");
    ASSERT_STREQ(childs[2]->objectName().toStdString().c_str(), "O4");

    ASSERT_TRUE(obj1.applyDiff(diff));

    childs = obj1.findDirectChildren<GtObject*>();

    ASSERT_EQ(childs.size(), 3);

    ASSERT_STREQ(childs[0]->objectName().toStdString().c_str(), "O2");
    ASSERT_STREQ(childs[1]->objectName().toStdString().c_str(), "O4");
    ASSERT_STREQ(childs[2]->objectName().toStdString().c_str(), "O3");
}

