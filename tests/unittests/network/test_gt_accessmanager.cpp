/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_accessmanager
 * copyright 2009-2016 by DLR
 *
 *  Created on: 18.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QUrl>

#include "gt_accessmanager.h"

/// This is a test fixture that does a init for each test
class TestGtAccessManager : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        manager = new GtAccessManager("Test Manager");

        host = "198.167.0.1";
        port = 22;
        user = "test_user";
        pw = "my_password";
    }

    virtual void TearDown()
    {
        delete manager;
    }

    GtAccessManager* manager;

    QString host;
    int port;
    QString user;
    QString pw;

};

TEST_F(TestGtAccessManager, init)
{
    ASSERT_STREQ(manager->objectName().toStdString().c_str(),
                 "Test Manager");
}

TEST_F(TestGtAccessManager, addData)
{
    GtAccessData data1(host, port, user, pw);

    ASSERT_TRUE(manager->addData(data1));
    ASSERT_FALSE(manager->addData(data1));

    GtAccessData data2("");
    ASSERT_FALSE(manager->addData(data2));

    ASSERT_FALSE(manager->addData("", port, user));
    ASSERT_FALSE(manager->addData(host, port, user));
    ASSERT_TRUE(manager->addData(host + "2", port, user));
}

TEST_F(TestGtAccessManager, isEmpty)
{
    ASSERT_TRUE(manager->isEmpty());

    GtAccessData data1(host, port, user, pw);

    manager->addData(data1);
    ASSERT_FALSE(manager->isEmpty());
}

TEST_F(TestGtAccessManager, setData)
{
    GtAccessData data1(host, port, user, pw);

    manager->addData(data1);

    ASSERT_FALSE(manager->setData(data1, 1));
    ASSERT_FALSE(manager->setData(data1, -1));
    ASSERT_TRUE(manager->setData(data1, 0));
}

TEST_F(TestGtAccessManager, accessData)
{
    QList<GtAccessData> list = manager->accessData();

    ASSERT_TRUE(list.isEmpty());

    GtAccessData data1(host, port, user, pw);

    manager->addData(data1);

    list = manager->accessData();
    ASSERT_FALSE(list.isEmpty());
}

TEST_F(TestGtAccessManager, size)
{
    ASSERT_EQ(manager->size(), 0);

    GtAccessData data1(host, port, user, pw);

    manager->addData(data1);

    ASSERT_EQ(manager->size(), 1);
}

TEST_F(TestGtAccessManager, deleteData)
{
    ASSERT_FALSE(manager->deleteData(0));

    GtAccessData data1(host, port, user, pw);

    manager->addData(data1);

    ASSERT_FALSE(manager->deleteData(1));
    ASSERT_FALSE(manager->deleteData(-1));
    ASSERT_TRUE(manager->deleteData(0));

    ASSERT_TRUE(manager->isEmpty());
}
