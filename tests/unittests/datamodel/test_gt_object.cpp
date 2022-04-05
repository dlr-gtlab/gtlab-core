#include <QDebug>

#include "gtest/gtest.h"

#include "test_gt_object.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"

#include "gt_datazone.h"
#include "gt_datazone0d.h"

/// This is a test fixture that does a init for each test
class TestGtObject : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        obj.setObjectName("MyObject");
        obj.setFactory(gtObjectFactory);
    }

    virtual void TearDown()
    {
    }

    GtObjectGroup obj;
};

TEST_F(TestGtObject, toMemento)
{
    qDebug() << "#### TEST_F(TestGtObject, toMemento)";

    // without properties
    GtObjectMemento memento = obj.toMemento();
    ASSERT_FALSE(memento.isNull());

//    GtObjectMemento mementoNew = obj.toMementoNew();
//    ASSERT_FALSE(mementoNew.isNull());

//    GtObjectMementoDiff diff(memento, mementoNew);
//    ASSERT_TRUE(diff.isNull());

    // with properties
//    TestSpecialGtObject op1;
//    TestSpecialGtObject op2;
//    op2.setUuid(op1.uuid());

//    GtObjectMemento mp1 = op1.toMemento();
//    ASSERT_FALSE(mp1.isNull());

//    qDebug() << mp1.toString();

//    qDebug() << "";

//    GtObjectMemento mp2 = op2.toMementoNew();
//    ASSERT_FALSE(mp2.isNull());

//    qDebug() << mp2.toString();

//    GtObjectMementoDiff diffp(mp1, mp2);
//    ASSERT_TRUE(diffp.isNull());

//    qDebug() << "####";
}

TEST_F(TestGtObject, fromMemento)
{
    qDebug() << "#### TEST_F(TestGtObject, fromMemento)";

    gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);

    // with properties
    TestSpecialGtObject op1;

    op1.setBool(true);
    op1.setDouble(1.0);
    op1.setFile("bla");
    op1.setInt(20);
    op1.setLabel("lab");
    op1.setLink("link");
    op1.setString("str");
    QVector<double> doubleVec;
    doubleVec << 0.1 << 0.2 << 0.3;
    op1.setDoubleVec(doubleVec);

    GtObjectMemento mp1 = op1.toMemento();
    ASSERT_FALSE(mp1.isNull());

//    qDebug() << mp1.toString();

//    qDebug() << "";

//    TestSpecialGtObject op2;

//    ASSERT_FALSE(op2.getBool());
//    ASSERT_DOUBLE_EQ(op2.getDouble(), 0.0);
//    ASSERT_TRUE(op2.getFile().isEmpty());
//    ASSERT_EQ(op2.getInt(), 0);
//    ASSERT_TRUE(op2.getLabel().isEmpty());
//    ASSERT_TRUE(op2.getLink().isEmpty());
//    ASSERT_STREQ(op2.getString().toStdString().c_str(), "Test");

//    op2.setUuid(op1.uuid());
//    op2.setFactory(gtObjectFactory);

//    op2.fromMementoNew(mp1);

//    ASSERT_TRUE(op2.getBool());
//    ASSERT_DOUBLE_EQ(op2.getDouble(), 1.0);
//    ASSERT_STREQ(op2.getFile().toStdString().c_str(), "bla");
//    ASSERT_EQ(op2.getInt(), 20);
//    ASSERT_STREQ(op2.getLabel().toStdString().c_str(), "lab");
//    ASSERT_STREQ(op2.getLink().toStdString().c_str(), "link");
//    ASSERT_STREQ(op2.getString().toStdString().c_str(), "str");

//    GtObjectMemento mp2 = op2.toMementoNew();
//    ASSERT_FALSE(mp2.isNull());

//    qDebug() << mp2.toString();
}

TEST_F(TestGtObject, copy)
{
    GtObject* myCopy = obj.copy();

    ASSERT_TRUE(myCopy != nullptr);
    ASSERT_TRUE(myCopy != &obj);

    // check that uuid are not equal
    ASSERT_STRNE(myCopy->uuid().toStdString().c_str(), obj.uuid().toStdString().c_str());

    delete myCopy;
}

TEST_F(TestGtObject, objectPath)
{
    GtObjectGroup* child1 = new GtObjectGroup(&obj);
    child1->setObjectName("Child1");

    GtObjectGroup* child2 = new GtObjectGroup(child1);
    child2->setObjectName("Child2");

    QString path = child2->objectPath();
    QString pathCompare = obj.objectName() + ";" + child1->objectName()
                          + ";" + child2->objectName();

    // check that uuid are not equal
    ASSERT_STREQ(path.toStdString().c_str(), pathCompare.toStdString().c_str());
}

TEST_F(TestGtObject, getObjectByPath)
{
    GtObjectGroup* child1 = new GtObjectGroup(&obj);
    child1->setObjectName("Child1");

    GtObjectGroup* child2 = new GtObjectGroup(&obj);
    child2->setObjectName("Child2");

    GtObjectGroup* child3 = new GtObjectGroup(child1);
    child3->setObjectName("Child3");

    QString path = obj.objectName() + ";" + child1->objectName()
                   + ";" + child3->objectName();

    qDebug() << path;

    GtObject* fo = obj.getObjectByPath(path);

    ASSERT_TRUE(fo != nullptr);
    ASSERT_TRUE(fo == child3);
}

TEST_F(TestGtObject, clone)
{
    GtObject* cloned = obj.clone();

    ASSERT_TRUE(cloned != nullptr);

    // check that uuid are equal
    ASSERT_STREQ(cloned->uuid().toStdString().c_str(), obj.uuid().toStdString().c_str());

    delete cloned;
}

TEST_F(TestGtObject, appendChild)
{
    GtObject* child = new GtObject;
    TestSpecialGtObject* parent = new TestSpecialGtObject;

    ASSERT_FALSE(obj.appendChild(nullptr));

    ASSERT_TRUE(obj.appendChild(child));

    ASSERT_TRUE(parent->appendChild(&obj));

    ASSERT_FALSE(parent->appendChild(&obj));

    ASSERT_FALSE(parent->appendChild(child));
}

TEST_F(TestGtObject, flags)
{
    GtObject::ObjectFlags f = obj.objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);
}

TEST_F(TestGtObject, acceptChanges)
{
    GtObject::ObjectFlags f = obj.objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);

    obj.acceptChanges();

    f = obj.objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);
}

TEST_F(TestGtObject, insertChild)
{
    GtObject* parentObject = new GtObject;

    GtObject* child1 = new GtObject;
    GtObject* child2 = new GtObject;
    GtObject* child3 = new GtObject;
    GtObject* child4 = new GtObject;
    GtObject* child5 = new GtObject;

    parentObject->appendChild(child1);
    parentObject->appendChild(child2);
    parentObject->appendChild(child3);
    parentObject->appendChild(child4);
    parentObject->appendChild(child5);

    parentObject->acceptChangesRecursively();

    GtObject* objectToInsert = new GtObject;

    parentObject->insertChild(2, objectToInsert);
    QList<GtObject*> children = parentObject->findDirectChildren<GtObject*>();

    ASSERT_EQ(children.size(), 6);

    ASSERT_FALSE(children.at(2) == child3);
    ASSERT_FALSE(children.at(3) == child4);
    ASSERT_FALSE(children.at(4) == child5);

    ASSERT_TRUE(children.at(0) == child1);
    ASSERT_TRUE(children.at(1) == child2);
    ASSERT_TRUE(children.at(2) == objectToInsert);
    ASSERT_TRUE(children.at(3) == child3);
    ASSERT_TRUE(children.at(4) == child4);
    ASSERT_TRUE(children.at(5) == child5);


    GtObject::ObjectFlags f = parentObject->objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);

    f = objectToInsert->objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);

    f = child3->objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);

    delete parentObject;
}

TEST_F(TestGtObject, isDerivedFromClass)
{
    GtDataZone* dz = nullptr;
    /// Test with nullptr
    ASSERT_FALSE(isDerivedFromClass(dz, GT_CLASSNAME(GtAbstractDataZone)));
    dz = new GtDataZone;
    /// test with empty classname
    ASSERT_FALSE(isDerivedFromClass(dz, ""));
    /// check valid result
    ASSERT_TRUE(isDerivedFromClass(dz, GT_CLASSNAME(GtAbstractDataZone)));
    /// check for wrong superclass
    ASSERT_FALSE(isDerivedFromClass(dz, "GtCalculator"));
    /// check if object is derived from GtObject
    ASSERT_TRUE(isDerivedFromClass(dz, "GtObject"));
    /// check if function stops before QObject
    ASSERT_FALSE(isDerivedFromClass(dz, "QObject"));
    ///tidy up
    delete dz;
}
