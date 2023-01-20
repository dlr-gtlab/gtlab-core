/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_intproperty
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_intproperty.h"

/// This is a test fixture that does a init for each test
class TestGtIntProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtIntProperty("prop", "test int", "test brief",
                                   GtUnit::Category::NonDimensional);

        m_propBounds = new GtIntProperty("propBounds", "test int", "test brief",
                                         GtUnit::Category::NonDimensional,
                                         -1,
                                         3);

        m_propBoundsLow = new GtIntProperty("propBoundsLow", "test int", "test brief",
                                            GtUnit::Category::NonDimensional,
                                            GtIntProperty::BoundLow,
                                            2, 4);

        m_propBoundsHigh = new GtIntProperty("propBoundsHigh", "test int", "test brief",
                                             GtUnit::Category::NonDimensional,
                                             GtIntProperty::BoundHigh,
                                             38, 54);
    }

    virtual void TearDown()
    {
        delete m_prop;

        delete m_propBounds;

        delete m_propBoundsLow;

        delete m_propBoundsHigh;
    }

    GtIntProperty* m_prop;

    GtIntProperty* m_propBounds;

    GtIntProperty* m_propBoundsLow;

    GtIntProperty* m_propBoundsHigh;

};

TEST_F(TestGtIntProperty, initialization)
{
    // default constructor
    ASSERT_EQ(m_prop->get(), 0);
    ASSERT_EQ(m_prop->lowSideBoundary(), 0);
    ASSERT_EQ(m_prop->highSideBoundary(), 0);
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::Category::NonDimensional);
    ASSERT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test int");
    ASSERT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_prop->isReadOnly());

    // bounds
    ASSERT_EQ(m_propBounds->get(), 0);
    ASSERT_EQ(m_propBounds->lowSideBoundary(), -1);
    ASSERT_EQ(m_propBounds->highSideBoundary(), 3);
    ASSERT_EQ(m_propBounds->unitCategory(), GtUnit::Category::NonDimensional);
    ASSERT_STREQ(m_propBounds->objectName().toStdString().c_str(),
                 "test int");
    ASSERT_STREQ(m_propBounds->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBounds->isReadOnly());

    // bounds low
    ASSERT_EQ(m_propBoundsLow->get(), 4);
    ASSERT_EQ(m_propBoundsLow->lowSideBoundary(), 2);
    ASSERT_EQ(m_propBoundsLow->highSideBoundary(), 0.0);
    ASSERT_EQ(m_propBoundsLow->unitCategory(),
              GtUnit::Category::NonDimensional);
    ASSERT_STREQ(m_propBoundsLow->objectName().toStdString().c_str(),
                 "test int");
    ASSERT_STREQ(m_propBoundsLow->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBoundsLow->isReadOnly());

    // bounds high
    ASSERT_EQ(m_propBoundsHigh->get(), 0.0);
    ASSERT_EQ(m_propBoundsHigh->lowSideBoundary(), 0.0);
    ASSERT_EQ(m_propBoundsHigh->highSideBoundary(), 38);
    ASSERT_EQ(m_propBoundsHigh->unitCategory(),
              GtUnit::Category::NonDimensional);
    ASSERT_STREQ(m_propBoundsHigh->objectName().toStdString().c_str(),
                 "test int");
    ASSERT_STREQ(m_propBoundsHigh->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBoundsHigh->isReadOnly());
}

TEST_F(TestGtIntProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    ASSERT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtIntProperty, getter)
{
    m_prop->setVal(11);
    ASSERT_EQ(m_prop->get(), 11);

    m_prop->setVal(4);
    ASSERT_EQ(*m_prop, 4);
}

TEST_F(TestGtIntProperty, setter)
{
    m_prop->setVal(11);
    ASSERT_EQ(m_prop->get(), 11);

    *m_prop = 230;
    ASSERT_EQ(m_prop->get(), 230);

//    m_prop->setReadOnly(true);
//    m_prop->setVal(12);
//    ASSERT_NE(m_prop->get(), 12);
//    ASSERT_EQ(m_prop->get(), 230);
}

TEST_F(TestGtIntProperty, setFromVariant)
{
    QVariant var(11);

    ASSERT_TRUE(m_prop->setValueFromVariant(var, QString()));
    EXPECT_EQ(m_prop->get(), 11);
}

TEST_F(TestGtIntProperty, getFromVariant)
{
    m_prop->setVal(11);

    bool success = false;
    int val = 0;

    QVariant var = m_prop->valueToVariant(QString(), &success);
    val = var.toDouble();
    ASSERT_EQ(val, 11);
    ASSERT_TRUE(success);
}

TEST_F(TestGtIntProperty, convertTo)
{
    m_prop->setVal(11);
    ASSERT_EQ(m_prop->get(), 11);

    bool success = false;

    ASSERT_EQ(m_prop->getVal(QString(), &success), 11);
    ASSERT_TRUE(success);

    success = false;

    ASSERT_EQ(m_prop->getVal("cm2", &success), 11);
    ASSERT_FALSE(success);

    success = false;

    ASSERT_EQ(m_prop->getVal("cm^2", &success), 11);
    ASSERT_FALSE(success);
}

TEST_F(TestGtIntProperty, convertFrom)
{
    bool success = false;

    m_prop->setVal(11, QString(), &success);
    ASSERT_EQ(m_prop->get(), 11);
    ASSERT_TRUE(success);

    success = false;

    m_prop->setVal(14, "cm2", &success);
    ASSERT_EQ(m_prop->get(), 11);
    ASSERT_FALSE(success);

    success = false;

    m_prop->setVal(123000, "cm^2", &success);
    ASSERT_EQ(m_prop->get(), 11);
    ASSERT_FALSE(success);
}

TEST_F(TestGtIntProperty, bounds)
{
    bool success = false;

    m_propBounds->setVal(-1, &success);
    ASSERT_EQ(m_propBounds->get(), -1);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(-2, &success);
    ASSERT_EQ(m_propBounds->get(), -1);
    ASSERT_FALSE(success);

    success = false;

    m_propBounds->setVal(3, &success);
    ASSERT_EQ(m_propBounds->get(), 3);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(4, &success);
    ASSERT_EQ(m_propBounds->get(), 3);
    ASSERT_FALSE(success);
}

TEST_F(TestGtIntProperty, boundsLowOnly)
{
    bool success = false;

    m_propBoundsLow->setVal(2, &success);
    ASSERT_EQ(m_propBoundsLow->get(), 2);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsLow->setVal(-1, &success);
    ASSERT_EQ(m_propBoundsLow->get(), 2);
    ASSERT_FALSE(success);
}

TEST_F(TestGtIntProperty, boundsHighOnly)
{
    bool success = false;

    m_propBoundsHigh->setVal(38, &success);
    ASSERT_EQ(m_propBoundsHigh->get(), 38);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsHigh->setVal(45, &success);
    ASSERT_EQ(m_propBoundsHigh->get(), 38);
    ASSERT_FALSE(success);
}

TEST_F(TestGtIntProperty, wrongBounds)
{
    GtIntProperty prop("prop", "test", "test", GtUnit::Category::None, 4, 3, 4);

    ASSERT_EQ(prop.get(), 4);
    ASSERT_EQ(prop.lowSideBoundary(), 0);
    ASSERT_EQ(prop.highSideBoundary(), 0);
}

TEST_F(TestGtIntProperty, revert)
{
    GtIntProperty prop("prop", "test", "test", GtUnit::Category::None, 4);

    ASSERT_EQ(prop.get(), 4);

    prop = 2;

    ASSERT_EQ(prop.get(), 2);

    prop.revert();

    ASSERT_EQ(prop.get(), 4);
}

TEST_F(TestGtIntProperty, optional)
{
    GtIntProperty prop("prop", "test", "test", GtUnit::Category::None, 4);

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
