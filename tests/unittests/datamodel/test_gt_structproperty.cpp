/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_structproperty
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QVariant>

#include "gt_dynamicpropertycontainer.h"

/// This is a test fixture that does a init for each test
class TestGtStructProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtDynamicPropertyContainer("test struct");
    }

    virtual void TearDown()
    {
        delete m_prop;
    }

    GtDynamicPropertyContainer* m_prop;

};

TEST_F(TestGtStructProperty, initialization)
{
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::Category::None);
    ASSERT_TRUE(m_prop->brief().isEmpty());
}

TEST_F(TestGtStructProperty, setFromVariant)
{
    bool success = false;
    ASSERT_FALSE(m_prop->setValueFromVariant(QVariant(), QString(), &success));
    ASSERT_FALSE(success);
}

TEST_F(TestGtStructProperty, getFromVariant)
{
    bool success = false;
    QVariant var = m_prop->valueToVariant(QString(), &success);
    ASSERT_TRUE(var.isNull());
    ASSERT_FALSE(success);
}
