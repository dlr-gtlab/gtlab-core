/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_boolproperty
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_boolproperty.h"

/// This is a test fixture that does a init for each test
class TestGtBoolProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtBoolProperty("prop", "test bool", "test brief");
    }

    virtual void TearDown()
    {
        delete m_prop;
    }

    GtBoolProperty* m_prop;

};

TEST_F(TestGtBoolProperty, initialization)
{
    // default constructor
    ASSERT_FALSE(m_prop->get());
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test bool");
    ASSERT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
}

TEST_F(TestGtBoolProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    ASSERT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtBoolProperty, getter)
{
    m_prop->setVal(true);
    ASSERT_TRUE(m_prop->get());

    m_prop->setVal(false);
    ASSERT_FALSE(*m_prop);
}

TEST_F(TestGtBoolProperty, setter)
{
    m_prop->setVal(true);
    ASSERT_TRUE(m_prop->get());

    *m_prop = false;
    ASSERT_FALSE(m_prop->get());

//    m_prop->setReadOnly(true);
//    m_prop->setVal(true);
//    ASSERT_FALSE(m_prop->get());
}

TEST_F(TestGtBoolProperty, setFromVariant)
{
    QVariant var(true);

    bool success = false;

    m_prop->setValueFromVariant(var, QString(), &success);
    ASSERT_TRUE(m_prop->get());
    ASSERT_TRUE(success);
}

TEST_F(TestGtBoolProperty, getFromVariant)
{
    bool success = false;
    bool val = true;

    QVariant var = m_prop->valueToVariant(QString(), &success);
    val = var.toBool();
    ASSERT_FALSE(val);
    ASSERT_TRUE(success);

    m_prop->setVal(true);

    var = m_prop->valueToVariant(QString(), &success);
    val = var.toBool();
    ASSERT_TRUE(val);
    ASSERT_TRUE(success);
}

TEST_F(TestGtBoolProperty, revert)
{
    GtBoolProperty prop("prop", "bla", "blup", true);

    ASSERT_TRUE(prop);

    prop = false;

    ASSERT_FALSE(prop);

    prop.revert();

    ASSERT_TRUE(prop);
}

TEST_F(TestGtBoolProperty, optional)
{
    GtBoolProperty prop("prop", "bla", "blup", true);

    ASSERT_TRUE(prop.isActive());
    ASSERT_FALSE(prop.isOptional());

    prop.setActive(false);

    ASSERT_TRUE(prop.isActive());
    ASSERT_FALSE(prop.isOptional());

    prop.setActive(true);

    ASSERT_TRUE(prop.isActive());
    ASSERT_FALSE(prop.isOptional());

    prop.setOptional(true);

    ASSERT_FALSE(prop.isActive());
    ASSERT_TRUE(prop.isOptional());

    prop.setOptional(false);

    ASSERT_TRUE(prop.isActive());
    ASSERT_FALSE(prop.isOptional());
}
