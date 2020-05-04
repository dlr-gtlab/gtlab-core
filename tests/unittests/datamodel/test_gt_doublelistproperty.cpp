/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_doublelistproperty.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 01.02.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"

#include "test_gt_doublelistproperty.h"

/// This is a test fixture that does a init for each test
class TestGtDoubleListProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_obj.setObjectName("MyObject");
        m_obj.setFactory(gtObjectFactory);
    }

    virtual void TearDown()
    {

    }

    TestDoubleListObject m_obj;
};

TEST_F(TestGtDoubleListProperty, initialization)
{
    ASSERT_TRUE(m_obj.getDoubleList().isEmpty());
}

TEST_F(TestGtDoubleListProperty, setterGetter)
{
    QVector<double> temp;
    temp << 0.1 << 0.2 << 0.3;

    m_obj.setDoubleList(temp);

    QVector<double> temp2 = m_obj.getDoubleList();

    ASSERT_FALSE(temp2.isEmpty());
    ASSERT_TRUE(temp.size() == temp2.size());

    for (int i = 0; i < temp.size(); ++i)
    {
        ASSERT_DOUBLE_EQ(temp[i], temp2[i]);
    }
}

TEST_F(TestGtDoubleListProperty, memento)
{
    gtObjectFactory->registerClass(TestDoubleListObject::staticMetaObject);

    QVector<double> temp;
    temp << 0.1 << 0.2 << 0.3;

    m_obj.setDoubleList(temp);

    GtObjectMemento mem = m_obj.toMemento();
    ASSERT_FALSE(mem.isNull());

    TestDoubleListObject obj2;
    obj2.setFactory(gtObjectFactory);
    obj2.fromMemento(mem);

    QVector<double> temp2 = obj2.getDoubleList();

    ASSERT_FALSE(temp2.isEmpty());
    ASSERT_TRUE(temp.size() == temp2.size());

    for (int i = 0; i < temp.size(); ++i)
    {
        ASSERT_DOUBLE_EQ(temp[i], temp2[i]);
    }
}

TEST_F(TestGtDoubleListProperty, diff)
{
    gtObjectFactory->registerClass(TestDoubleListObject::staticMetaObject);

    GtObjectMemento mem1 = m_obj.toMemento();
    ASSERT_FALSE(mem1.isNull());

    QVector<double> temp;
    temp << 0.1 << 0.2 << 0.3;

    m_obj.setDoubleList(temp);

    GtObjectMemento mem2 = m_obj.toMemento();
    ASSERT_FALSE(mem2.isNull());

    GtObjectMementoDiff diff(mem1, mem2);

    qDebug() << "#### DOUBLELISTDIFF";
    qDebug() << diff.toByteArray();

    ASSERT_FALSE(diff.isNull());

    QVector<double> temp2 = m_obj.getDoubleList();

    ASSERT_FALSE(temp2.isEmpty());
    ASSERT_TRUE(temp.size() == temp2.size());

    for (int i = 0; i < temp.size(); ++i)
    {
        ASSERT_DOUBLE_EQ(temp[i], temp2[i]);
    }

    qDebug() << "-------------------";
    qDebug() << m_obj.getDoubleList();

    ASSERT_TRUE(m_obj.revertDiff(diff));

    qDebug() << m_obj.getDoubleList();

    qDebug() << "###################";

    ASSERT_TRUE(m_obj.getDoubleList().isEmpty());
}
