#include <QDebug>

#include "gtest/gtest.h"

#include "test_gt_object.h"
#include "test_propertycontainerobject.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"

#include "gt_labeldata.h"

#include "slotadaptor.h"
#include <QtConcurrent/QtConcurrent>

/// This is a test fixture that does a init for each test
class TestGtObject : public ::testing::Test
{
protected:

    void SetUp() override
    {
        obj.setObjectName("MyObject");
        obj.setFactory(gtObjectFactory);
    }

    GtObjectGroup obj;
};

TEST_F(TestGtObject, toMemento)
{
    qDebug() << "#### TEST_F(TestGtObject, toMemento)";

    // without properties
    GtObjectMemento memento = obj.toMemento();
    ASSERT_FALSE(memento.isNull());
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
}

TEST_F(TestGtObject, copy)
{
    GtObject* myCopy = obj.copy();

    ASSERT_TRUE(myCopy != nullptr);
    ASSERT_TRUE(myCopy != &obj);

    // check that uuid are not equal
    ASSERT_STRNE(myCopy->uuid().toStdString().c_str(), obj.uuid().toStdString().c_str());

    delete myCopy;

    GtObject origWithoutFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);

    GtObject* copiedWithDefaultFactory = origWithoutFactory.clone();

    ASSERT_TRUE(copiedWithDefaultFactory != nullptr);

    delete copiedWithDefaultFactory ;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);
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

    GtObject origWithoutFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);

    GtObject* clonedWithDefaultFactory = origWithoutFactory.clone();

    ASSERT_TRUE(clonedWithDefaultFactory != nullptr);

    delete clonedWithDefaultFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);
}

TEST_F(TestGtObject, appendChild)
{
    GtObject* child = new GtObject;
    TestSpecialGtObject parent;

    ASSERT_FALSE(obj.appendChild(nullptr));

    ASSERT_TRUE(obj.appendChild(child));
    ASSERT_FALSE(parent.appendChild(child));
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
    QPointer<GtObject> parentObject = new GtObject;

    QPointer<GtObject> child1 = new GtObject;
    QPointer<GtObject> child2 = new GtObject;
    QPointer<GtObject> child3 = new GtObject;
    QPointer<GtObject> child4 = new GtObject;
    QPointer<GtObject> child5 = new GtObject;

    parentObject->appendChild(child1);
    parentObject->appendChild(child2);
    parentObject->appendChild(child3);
    parentObject->appendChild(child4);
    parentObject->appendChild(child5);

    parentObject->acceptChangesRecursively();

    QPointer<GtObject> objectToInsert = new GtObject;

    parentObject->insertChild(2, objectToInsert.data());
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
    GtLabelData label;
    auto* p = &label;
    auto* m = p->metaObject();
    ASSERT_TRUE(m);

    /// test with empty classname
    ASSERT_FALSE(gt::isDerivedFromClass(p, ""));
    ASSERT_FALSE(gt::isDerivedFromClass(m, ""));
    /// check valid result
    ASSERT_TRUE(gt::isDerivedFromClass(p, GT_CLASSNAME(GtObjectGroup)));
    ASSERT_TRUE(gt::isDerivedFromClass(m, GT_CLASSNAME(GtObjectGroup)));
    /// check for wrong superclass
    ASSERT_FALSE(gt::isDerivedFromClass(p, "GtCalculator"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "GtCalculator"));
    /// check for its own class
    ASSERT_FALSE(gt::isDerivedFromClass(p, "GtLabelData"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "GtLabelData"));
    /// check if object is derived from GtObject
    ASSERT_TRUE(gt::isDerivedFromClass(p, "GtObject"));
    ASSERT_TRUE(gt::isDerivedFromClass(m, "GtObject"));
    /// check if function stops before QObject
    ASSERT_FALSE(gt::isDerivedFromClass(p, "QObject"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "QObject"));
}

TEST_F(TestGtObject, findParent)
{
    TestObject obj;
    obj.setObjectName("Bla");
    EXPECT_EQ(obj.findParent(), nullptr);

    auto* child = new GtObject;
    obj.appendChild(child);

    EXPECT_EQ(obj.findParent(), nullptr);
    EXPECT_EQ(child->findParent<TestObject*>(), &obj);
    EXPECT_EQ(child->findParent<GtObjectGroup*>(), nullptr);
    EXPECT_EQ(child->findParent<QObject*>("Bla"), &obj);
}

TEST_F(TestGtObject, findRoot)
{
    TestObject obj;
    EXPECT_EQ(obj.findParent(), nullptr);

    auto* child = new GtObject;
    obj.appendChild(child);

    auto* childchild = new GtObject;
    child->appendChild(childchild);

    EXPECT_EQ(obj.findRoot(), nullptr);
    EXPECT_EQ(obj.findRoot(&obj), &obj);
    EXPECT_EQ(childchild->findRoot<TestObject*>(), &obj);
    EXPECT_EQ(childchild->findRoot<GtObjectGroup*>(), nullptr);
    EXPECT_EQ(childchild->findRoot<QObject*>(), &obj);
}

TEST_F(TestGtObject, constness)
{
    GtObject* obj{};
    GtObject const* constObj{};

    static_assert(std::is_same<GtObject*,
                  decltype(obj->findChild())>(), "Should be non-const");
    static_assert(std::is_same<GtObject const*,
                  decltype(constObj->findChild())>(), "Should be const");

    static_assert(std::is_same<GtObject*,
                  decltype(obj->findDirectChild())>(), "Should be non-const");
    static_assert(std::is_same<GtObject const*,
                  decltype(constObj->findDirectChild())>(), "Should be const");

    static_assert(std::is_same<QObject*,
                  decltype(obj->parent())>(), "Should be non-const");
    static_assert(std::is_same<QObject const*,
                  decltype(constObj->parent())>(), "Should be const");

    static_assert(std::is_same<GtObject*,
                  decltype(obj->findParent())>(), "Should be non-const");
    static_assert(std::is_same<GtObject const*,
                  decltype(constObj->findParent())>(), "Should be const");

    static_assert(std::is_same<GtObject*,
                  decltype(obj->findRoot())>(), "Should be non-const");
    static_assert(std::is_same<GtObject const*,
                  decltype(constObj->findRoot())>(), "Should be const");

    static_assert(std::is_same<GtAbstractProperty*,
                  decltype(obj->findProperty("id"))>(), "Should be non-const");
    static_assert(std::is_same<GtAbstractProperty const*,
                  decltype(constObj->findProperty("id"))>(), "Should be const");

    static_assert(std::is_same<GtAbstractProperty*,
                  decltype(obj->findPropertyByName("name"))>(), "Should be non-const");
    static_assert(std::is_same<GtAbstractProperty const*,
                  decltype(constObj->findPropertyByName("name"))>(), "Should be const");

    static_assert(std::is_same<GtPropertyStructContainer*,
                  decltype(obj->findPropertyContainer("id"))>(), "Should be non-const");
    static_assert(std::is_same<GtPropertyStructContainer const*,
                  decltype(constObj->findPropertyContainer("id"))>(), "Should be const");

    static_assert(std::is_same<QList<GtObject*>,
                  decltype(obj->findChildren())>(), "Should be non-const");
    static_assert(std::is_same<QList<GtObject const*>,
                  decltype(constObj->findChildren())>(), "Should be non-const");

    static_assert(std::is_same<QList<GtObject*>,
                  decltype(obj->findDirectChildren())>(), "Should be non-const");
    static_assert(std::is_same<QList<GtObject const*>,
                  decltype(constObj->findDirectChildren())>(), "Should be non-const");
}

TEST(GtObjectBugs, issue276)
{
    TestSpecialGtObject testobj;
    QPointer<GtObject> child1 = new TestSpecialGtObject;
    QPointer<GtObject> child2 = new TestSpecialGtObject;
    EXPECT_TRUE(testobj.appendChild(child1));
    EXPECT_TRUE(testobj.appendChild(child2));

    // uncommenting this line returns the correct no. of children but deletes
    // all children in the process
    // -> now deactivatd using SFINAE
//    EXPECT_EQ(2, testobj.childCount<GtObject>());

    EXPECT_EQ(2, testobj.childCount<GtObject*>());

    EXPECT_TRUE(child1 != nullptr);
    EXPECT_TRUE(child2 != nullptr);
}

TEST(GtObjectBugs, issue325)
{
    TestSpecialGtObject obj;
    obj.acceptChanges();

    bool propertyChanged = false;

    SlotAdaptor a;
    a.on(&obj, SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
           [&propertyChanged]() {
        propertyChanged = true;
    });

    ASSERT_FALSE(obj.hasChanges());

    EXPECT_FALSE(propertyChanged);
    obj.setDouble(23456.);
    EXPECT_TRUE(propertyChanged);
    EXPECT_TRUE(obj.hasChanges());
}

/*
 * This test checks, that a signal change from a sub
 * property is propagated correctly
 *
 * This was initially not the case, as the thread switching was not properly
 * propagated to all childs
 */
TEST(GtObjectBugs, subPropsSignalsFromThread)
{
    std::unique_ptr<TestSpecialGtObject> obj;
    QtConcurrent::run([&obj, mainThread = QThread::currentThread()](){
        obj.reset(new TestSpecialGtObject);

        // note, we may not use QObject->moveToThread here!!!
        gt::moveToThread(*obj, mainThread);
    }).waitForFinished();

    bool changed = false;
    QObject::connect(
        &(obj->m_modeProp),
        &GtAbstractProperty::subPropChanged,
        [&changed](GtAbstractProperty*)
    {
        changed = true;
    });

    ASSERT_TRUE(obj != nullptr);
    EXPECT_FALSE(changed);

    obj->m_modeTypeProp.setVal("a new value");
    EXPECT_TRUE(changed);
}


/**
 * In bug 539, the number of child objects kept increasing
 * if they were unnamed
 */
TEST_F(TestGtObject, bug539)
{
    gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);
    gtObjectFactory->registerClass(TestObject::staticMetaObject);

    // with properties
    TestSpecialGtObject parent;
    parent.setFactory(gtObjectFactory);

    auto o1 = new TestObject;
    o1->setObjectName("");
    parent.appendChild(o1);

    auto o2 = new TestSpecialGtObject;
    o2->setObjectName("");
    parent.appendChild(o2);

    ASSERT_EQ(parent.findDirectChildren<TestObject*>().size(), 1);
    ASSERT_EQ(parent.findDirectChildren<TestSpecialGtObject*>().size(), 1);

    // merge back its data into itself, it should not change its layout
    auto memento = parent.toMemento();
    parent.fromMemento(memento);

    EXPECT_EQ(parent.findDirectChildren<TestObject*>().size(), 1);
    EXPECT_EQ(parent.findDirectChildren<TestSpecialGtObject*>().size(), 1);
}
