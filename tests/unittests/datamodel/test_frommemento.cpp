#include "gtest/gtest.h"


#include "test_gt_object.h"
#include "gt_objectmemento.h"
#include "gt_objectio.h"
#include "gt_objectfactory.h"

#include <QObject>

/**
 * @brief These tests assume the functionality
 * of toMemento
 *
 * All test check the conversion obj->memento->newobj
 */
class TestFromMemento : public testing::Test
{
public:

    void SetUp() override
    {
        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
        {
            gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);
        }

    }

    TestSpecialGtObject testobj;

};

std::unique_ptr<GtObject> fromMementoNew(const GtObjectMemento& m)
{
    return m.toObject(*GtObjectFactory::instance());
}

TEST_F(TestFromMemento, abstractProperties)
{
    testobj.setBool(true);
    testobj.setDouble(2.0);
    testobj.setInt(3);
    testobj.setFile("myfile.txt");

    auto srcListProp =qobject_cast<GtDoubleListProperty*>
            (testobj.findPropertyByName("Double List Property"));

    ASSERT_TRUE(srcListProp != nullptr);

    srcListProp->setVal(QVector<double>({5., 6., 7., 8}));


    GtObjectMemento memento = testobj.toMemento();

    auto newobj = fromMementoNew(memento);

    ASSERT_TRUE(newobj != nullptr);

    EXPECT_TRUE(newobj->findPropertyByName("Test Bool") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Double") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test File") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Group") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Int") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Label") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Mode") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Type") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Link") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test String") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Test Variant") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Double List Property") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("Existing Directory") != nullptr);
    EXPECT_TRUE(newobj->findPropertyByName("File Chooser") != nullptr);

    EXPECT_TRUE(newobj->findPropertyByName("Test Bool")->value<bool>() == true);
    EXPECT_TRUE(newobj->findPropertyByName("Test Double")->value<double>() == 2.0);
    EXPECT_TRUE(newobj->findPropertyByName("Test Int")->value<int>() == 3);
    EXPECT_TRUE(newobj->findPropertyByName("Test File")->value<QString>()
                == QString("myfile.txt"));

    auto group = qobject_cast<GtGroupProperty*>(
                newobj->findPropertyByName("Test Group"));
    ASSERT_TRUE(group != nullptr);

    // check lists, which need special treatment
    auto const * doubleProp = (
                testobj.findPropertyByName("Double List Property"));
    ASSERT_TRUE(doubleProp != nullptr);


    auto dblVec = doubleProp->value<QVector<double>>();
    EXPECT_EQ(4, dblVec.size());
    EXPECT_DOUBLE_EQ(5., dblVec[0]);
    EXPECT_DOUBLE_EQ(6., dblVec[1]);
    EXPECT_DOUBLE_EQ(7., dblVec[2]);
    EXPECT_DOUBLE_EQ(8., dblVec[3]);
}

TEST_F(TestFromMemento, qtProperties)
{
    testobj.setProperty("strList", QStringList({"a", "b", "c"}));

    GtObjectMemento memento = testobj.toMemento();

    auto newobj = fromMementoNew(memento);
    ASSERT_TRUE(newobj != nullptr);

    ASSERT_FALSE(newobj->property("strList").isNull());
    auto vec = newobj->property("strList").value<QStringList>();
    EXPECT_EQ(3, vec.size());
    EXPECT_TRUE("a" == vec[0]);
    EXPECT_TRUE("b" == vec[1]);
    EXPECT_TRUE("c" == vec[2]);
}

TEST_F(TestFromMemento, childObjects)
{
    auto child = new TestSpecialGtObject;
    child->setDouble(123.);

    EXPECT_TRUE(testobj.appendChild(child));
    ASSERT_EQ(1, testobj.childCount<GtObject*>());

    GtObjectMemento memento = testobj.toMemento();

    auto newobj = fromMementoNew(memento);
    ASSERT_TRUE(newobj != nullptr);

    ASSERT_EQ(1, newobj->childCount<GtObject*>());

    // make sure, the child objects are not only constructed
    // but also their properties have been set correctly
    auto newChild = newobj->findDirectChild<TestSpecialGtObject*>();
    ASSERT_TRUE(newChild != nullptr);

    EXPECT_DOUBLE_EQ(123., newChild->getDouble());

}


TEST_F(TestFromMemento, baseProps)
{
    testobj.setUuid("myuuid");
    testobj.setObjectName("myObjectName");


    GtObjectMemento memento = testobj.toMemento();
    auto newobj = fromMementoNew(memento);
    ASSERT_TRUE(newobj != nullptr);

    EXPECT_STREQ("myuuid", newobj->uuid().toStdString().c_str());
    EXPECT_STREQ("myObjectName", newobj->objectName().toStdString().c_str());

    EXPECT_STREQ("TestSpecialGtObject",
                 newobj->metaObject()->className());

}

TEST_F(TestFromMemento, mergeObject)
{
    TestSpecialGtObject obj1;
    auto child1 = new TestSpecialGtObject;
    child1->setDouble(123.);

    EXPECT_TRUE(obj1.appendChild(child1));

    TestSpecialGtObject obj2;
    auto child2 = new TestSpecialGtObject;
    child2->setUuid(child1->uuid());
    child2->setDouble(234.);

    EXPECT_TRUE(obj2.appendChild(child2));

    auto memento = obj1.toMemento();

    EXPECT_TRUE(memento.mergeTo(obj2, *GtObjectFactory::instance()));

    EXPECT_EQ(1, obj2.childCount<GtObject*>());
    EXPECT_TRUE(child2->uuid() == child1->uuid());
    EXPECT_EQ(123., child2->getDouble());

    // if we now change the uuid of child2, this child
    // will be recreated, i.e. get the old uuid
    child2->setUuid("bla");

    EXPECT_TRUE(memento.mergeTo(obj2, *GtObjectFactory::instance()));

    EXPECT_EQ(1, obj2.childCount<GtObject*>());

    child2 = obj2.findChild<TestSpecialGtObject*>();
    EXPECT_TRUE(child2->uuid() == child1->uuid());
    EXPECT_EQ(123., child2->getDouble());
}

TEST_F(TestFromMemento, mergeNonMatchingObject)
{
    GtObject o;

    auto memento = testobj.toMemento();

    EXPECT_FALSE(memento.mergeTo(o, *GtObjectFactory::instance()));
}

TEST_F(TestFromMemento, mergeDummyObject)
{
    auto memento = GtObjectMemento{}
        .setClassName("ugly")
        .setUuid("fart")
        .setIdent("inThisRoom");

    GtObjectMemento::PropertyData doubleProp;
    doubleProp.setData(2.0);
    doubleProp.name = "shitty_double";

    GtObjectMemento::PropertyData stringListProp;
    stringListProp.setData(QStringList({"move", "your", "ass"}));
    stringListProp.name = "crazy_list";

    memento.properties = {doubleProp, stringListProp};

    auto obj = memento.toObject(*GtObjectFactory::instance());

    ASSERT_TRUE(obj != nullptr);
    ASSERT_TRUE(obj->isDummy());
    EXPECT_STREQ("GtObject", obj->metaObject()->className());
    EXPECT_EQ("fart", obj->uuid().toStdString());
    EXPECT_EQ("inThisRoom", obj->objectName().toStdString());

    auto mementoCopy = obj->toMemento(false);
    EXPECT_EQ("ugly", mementoCopy.className());
}

TEST_F(TestFromMemento, loadWithDefault)
{
    class ObjectWithDefaultChild : public GtObject
    {
    public:

        ObjectWithDefaultChild()
            : child(new TestSpecialGtObject)
        {
            child->setDefault(true);
            appendChild(child);
        }

        TestSpecialGtObject * child;
    };

    ObjectWithDefaultChild o;
    o.setUuid("o_uuid_old");

    o.child->setUuid("child_uuid_old");
    o.child->setDouble(567.);

    ASSERT_EQ(1, o.childCount<GtObject*>());

    GtObjectMemento m = o.toMemento(false);

    TestSpecialGtObject * const oldChildAddr = o.child;

    ASSERT_TRUE(m.childObjects.size() == 1);
    ASSERT_TRUE(m.childObjects[0].properties[1].dataType() == "double");

    m.childObjects[0].properties[1].setData(789.);

    m.mergeTo(o, *gtObjectFactory);

    ASSERT_EQ(1, o.childCount<GtObject*>());

    // Since the uuids have been regenerated when building the memento
    // also the child uid must have been changed!
    EXPECT_NE(o.child->uuid(), QString("child_uuid_old"));

    // the double prop needs to be 789., since we entered it into the memento
    EXPECT_DOUBLE_EQ(789., o.child->getDouble());

    EXPECT_EQ(oldChildAddr, o.child);

    m.childObjects.clear();
    m.mergeTo(o, *gtObjectFactory);

    // the child must remain, since it is default
    EXPECT_EQ(1, o.childCount<GtObject*>());
    EXPECT_EQ(oldChildAddr, o.child);

    ObjectWithDefaultChild o2;
    o2.child->setDefault(false);

    m.mergeTo(o2, *gtObjectFactory);
    EXPECT_EQ(0, o2.childCount<GtObject*>());
}
