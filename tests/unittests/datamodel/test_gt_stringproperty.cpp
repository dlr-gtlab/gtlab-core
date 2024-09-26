/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_stringproperty
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_stringproperty.h"

/// This is a test fixture that does a init for each test
class TestGtStringProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtStringProperty("prop", "test string", "test brief");
    }

    virtual void TearDown()
    {
        delete m_prop;
    }

    GtStringProperty* m_prop;

};

TEST_F(TestGtStringProperty, initialization)
{
    // default constructor
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "");
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::Category::None);
    ASSERT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test string");
    ASSERT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
}

TEST_F(TestGtStringProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    ASSERT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtStringProperty, getter)
{
    m_prop->setVal("bla");
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "bla");

    m_prop->setVal("blup");

    QString val = *m_prop;

    ASSERT_STREQ(val.toStdString().c_str(), "blup");
}

TEST_F(TestGtStringProperty, setter)
{
    m_prop->setVal("bla");
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "bla");

    *m_prop = "blup";
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "blup");

//    m_prop->setReadOnly(true);
//    m_prop->setVal("bla");
//    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "blup");
}

TEST_F(TestGtStringProperty, setFromVariant)
{
    QVariant var("bla");

    ASSERT_TRUE(m_prop->setValueFromVariant(var, QString()));
    EXPECT_STREQ(m_prop->get().toStdString().c_str(), "bla");
}

TEST_F(TestGtStringProperty, getFromVariant)
{
    bool success = false;
    QString val;

    m_prop->setVal("bla");
    QVariant var = m_prop->valueToVariant(QString(), &success);
    val = var.toString();
    ASSERT_STREQ(val.toStdString().c_str(), "bla");
    ASSERT_TRUE(success);
}

TEST_F(TestGtStringProperty, revert)
{
    GtStringProperty prop("prop", "test", "test", "hello");

    ASSERT_STREQ(prop.get().toStdString().c_str(), "hello");

    prop = "world";

    ASSERT_STREQ(prop.get().toStdString().c_str(), "world");

    prop.revert();

    ASSERT_STREQ(prop.get().toStdString().c_str(), "hello");
}

TEST_F(TestGtStringProperty, optional)
{
    GtStringProperty prop("prop", "test", "test", "hello");

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

TEST_F(TestGtStringProperty, extendenOperator)
{
	GtStringProperty prop("prop", "test", "test", "hello");
	
	// operator+=
	QString added = " is nice!";
    prop.setVal("GTlab");
	
	prop += added;
    ASSERT_STREQ(prop.get().toStdString().c_str(), "GTlab is nice!");
}