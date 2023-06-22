/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QDebug>

#include "gtest/gtest.h"

#include "gt_object.h"
#include "gt_objectpath.h"


/// This is a test fixture that does a init for each test
class TestGtObjectPath : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        // root
        //  |-> first_1
        //  |->  |-> second_1
        //  |->  |-> second_2
        //  |->  |->  |-> third_1
        //  |->  |-> second_3
        //  |-> first_2

        m_rootObj = new GtObject;
        m_rootObj->setObjectName("root");

        m_firstChild1 = new GtObject(m_rootObj);
        m_firstChild1->setObjectName("first_1");

        m_firstChild2 = new GtObject(m_rootObj);
        m_firstChild2->setObjectName("first_2");

        m_secondChild1 = new GtObject(m_firstChild1);
        m_secondChild1->setObjectName("second_1");

        m_secondChild2 = new GtObject(m_firstChild1);
        m_secondChild2->setObjectName("second_2");

        m_secondChild3 = new GtObject(m_firstChild1);
        m_secondChild3->setObjectName("second_3");

        m_thirdChild1 = new GtObject(m_secondChild2);
        m_thirdChild1->setObjectName("third_1");
    }

    virtual void TearDown()
    {
        delete m_rootObj;
    }

    GtObject* m_rootObj;

    GtObject* m_firstChild1;

    GtObject* m_firstChild2;

    GtObject* m_secondChild1;

    GtObject* m_secondChild2;

    GtObject* m_secondChild3;

    GtObject* m_thirdChild1;

};

TEST_F(TestGtObjectPath, constructor)
{
    // empty constructor
    GtObjectPath path1;
    ASSERT_TRUE(path1.isEmpty());

    // string constructor
    GtObjectPath path2(QStringLiteral("root") + GtObjectPath::separator() +
                       QStringLiteral("first_1"));
    ASSERT_FALSE(path2.isEmpty());

    // object constructor
    GtObjectPath path3 (m_thirdChild1);
    ASSERT_FALSE(path3.isEmpty());
}

TEST_F(TestGtObjectPath, toString)
{
    QString tmpStr = QStringLiteral("root") + GtObjectPath::separator() +
                     QStringLiteral("first_1");

    GtObjectPath path1(tmpStr);

    ASSERT_STREQ(path1.toString().toStdString().c_str(),
                 tmpStr.toStdString().c_str());

    // empty
    GtObjectPath path2;

    ASSERT_TRUE(path2.toString().isEmpty());
}

TEST_F(TestGtObjectPath, toStringList)
{
    QStringList tmpList;
    tmpList << "root" << "first_1";

    QString tmpStr = QStringLiteral("root") + GtObjectPath::separator() +
                     QStringLiteral("first_1");

    GtObjectPath path1(tmpStr);

    QStringList resList = path1.toStringList();

    ASSERT_TRUE(resList.size() == tmpList.size());

    for (int i = 0; i < resList.size(); ++i)
    {
        ASSERT_STREQ(resList[i].toStdString().c_str(),
                     tmpList[i].toStdString().c_str());
    }

    // empty
    GtObjectPath path2;

    ASSERT_TRUE(path2.toStringList().isEmpty());
}

TEST_F(TestGtObjectPath, getObject)
{
    // empty constructor
    GtObjectPath path1;
    GtObject* obj = path1.getObject(m_rootObj);
    ASSERT_TRUE(obj == nullptr);

    // string constructor
    GtObjectPath path2(QStringLiteral("root") + GtObjectPath::separator() +
                       QStringLiteral("first_1"));

    obj = path2.getObject(m_rootObj);
    ASSERT_TRUE(obj == m_firstChild1);

    obj = path2.getObject(m_secondChild1);
    ASSERT_FALSE(obj == m_firstChild1);

    // object constructor
    GtObjectPath path3 (m_thirdChild1);
    obj = path3.getObject(m_rootObj);
    ASSERT_TRUE(obj == m_thirdChild1);
}
