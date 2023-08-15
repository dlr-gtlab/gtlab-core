#include "gtest/gtest.h"

#include "test_gt_object.h"
#include "test_propertycontainerobject.h"
#include "gt_propertyconnection.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"

#include <QCoreApplication>
#include <QTimer>

struct TestPropertyConnection : public testing::Test
{
    void SetUp() override
    {
        parent = std::make_unique<TestSpecialGtObject>();

        auto child1 = new TestSpecialGtObject;
        child1->setUuid("child1");
        parent->appendChild(child1);

        auto child2 = new TestSpecialGtObject;
        child2->setUuid("child2");
        parent->appendChild(child2);

        auto child3 = new TestObject();
        child3->addEnvironmentVar("PATH", "/usr/bin");
        child3->setUuid("child3");
        parent->appendChild(child3);

        if (!gtObjectFactory->knownClass("TestObject"))
        {
            gtObjectFactory->registerClass(TestObject::staticMetaObject);
        }
    }

    std::unique_ptr<GtObject> parent;
};

TEST_F(TestPropertyConnection, baseline)
{
    auto connection = new GtPropertyConnection();

    connection->setSourceUuid("child1");
    connection->setTargetUuid("child2");
    connection->setSourceProp("doubleProp");
    connection->setTargetProp("doubleProp");

    parent->appendChild(connection);

    auto c1 =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(c1 != nullptr);
    auto c2 =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child2"));
    ASSERT_TRUE(c2 != nullptr);

    c1->setDouble(2.0);
    c2->setDouble(3.0);

    EXPECT_EQ(2.0, c1->getDouble());
    EXPECT_EQ(3.0, c2->getDouble());

    connection->makeConnection();

    c1->setDouble(5.0);
    EXPECT_EQ(5.0, c1->getDouble());
    EXPECT_EQ(5.0, c2->getDouble());
}

TEST_F(TestPropertyConnection, propertyContainer)
{
    auto c1 =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(c1 != nullptr);

    c1->setString("HalloWelt");

    auto c3 = dynamic_cast<TestObject*>(parent->getObjectByUuid("child3"));
    ASSERT_TRUE(c3 != nullptr);

    const QString entryID = c3->environmentVars.at(0).ident();

    auto connection = new GtPropertyConnection();
    connection->setSourceUuid("child1");
    connection->setTargetUuid("child3");
    connection->setSourceProp("strProp");
    connection->setTargetProp(
        QString("environmentVars[%1].value").arg(entryID));

    parent->appendChild(connection);

    EXPECT_EQ("HalloWelt", c1->getString().toStdString());
    EXPECT_EQ(
        "/usr/bin",
        c3->environmentVars.at(0).getMemberVal<QString>("value").toStdString());

    connection->makeConnection();
    c1->setString("ThisIsAString");

    EXPECT_EQ("ThisIsAString", c1->getString().toStdString());
    EXPECT_EQ(
        "ThisIsAString",
        c3->environmentVars.at(0).getMemberVal<QString>("value").toStdString());
}

TEST_F(TestPropertyConnection, clearContainer)
{
    // A workaround, to make sure that the deleteLater() of the property connection
    // is in fact processed
    QEventLoop destroyLoop;

    auto c1 =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(c1 != nullptr);

    auto c3 = dynamic_cast<TestObject*>(parent->getObjectByUuid("child3"));
    ASSERT_TRUE(c3 != nullptr);

    const QString entryID = c3->environmentVars.at(0).ident();

    auto connection = new GtPropertyConnection();
    connection->setTargetUuid("child1");
    connection->setSourceUuid("child3");
    connection->setTargetProp("strProp");
    connection->setSourceProp(
        QString("environmentVars[%1].value").arg(entryID));

    auto connectionUID = connection->uuid();

    parent->appendChild(connection);
    connection->makeConnection();

    ASSERT_TRUE(parent->getObjectByUuid(connectionUID));

    // kill the event loop, after the property has been deleted
    QObject::connect(connection, &QObject::destroyed,
                     &destroyLoop, &QEventLoop::quit);


    // clear the entries, this should trigger the connection to be deleted
    c3->environmentVars.clear();

    // make sure to exit the exit the event loop, even if the source connection is not deleted
    QTimer::singleShot(10, &destroyLoop,&QEventLoop::quit);
    destroyLoop.exec();

    EXPECT_FALSE(parent->getObjectByUuid(connectionUID));
}


TEST_F(TestPropertyConnection, updateEntriesBug571)
{
    // A workaround, to make sure that the deleteLater() of the property connection
    // is in fact processed
    QEventLoop destroyLoop;

    auto c1 =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(c1 != nullptr);

    auto childWithPropContainer = dynamic_cast<TestObject*>(parent->getObjectByUuid("child3"));
    ASSERT_TRUE(childWithPropContainer != nullptr);

    const QString entryID = childWithPropContainer->environmentVars.at(0).ident();

    auto connection = new GtPropertyConnection();
    connection->setTargetUuid("child1");
    connection->setSourceUuid("child3");
    connection->setTargetProp("strProp");
    connection->setSourceProp(
        QString("environmentVars[%1].value").arg(entryID));

    auto connectionUID = connection->uuid();

    parent->appendChild(connection);
    connection->makeConnection();

    ASSERT_TRUE(parent->getObjectByUuid(connectionUID));

    auto memento = childWithPropContainer->toMemento();

    childWithPropContainer->setFactory(gtObjectFactory);
    childWithPropContainer->fromMemento(memento);

    // make sure to exit the exit the event loop, even if the source connection is not deleted
    QTimer::singleShot(10, &destroyLoop,&QEventLoop::quit);
    destroyLoop.exec();

    EXPECT_TRUE(parent->getObjectByUuid(connectionUID));
}
