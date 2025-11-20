/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_intproperty
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
        m_prop = new GtIntProperty("prop", "test int", "test brief");

        m_propBounds = new GtIntProperty("propBounds", "test int", "test brief",
                                         -1,
                                         3);

        m_propBoundsLow = new GtIntProperty("propBoundsLow", "test int", "test brief",
                                            gt::Boundaries<int>::makeLower(2),
                                            4);

        m_propBoundsHigh = new GtIntProperty("propBoundsHigh", "test int", "test brief",
                                             gt::Boundaries<int>::makeUpper(38),
                                             54);
    }

    virtual void TearDown()
    {
        delete m_prop;

        delete m_propBounds;

        delete m_propBoundsLow;

        delete m_propBoundsHigh;
    }

    GtIntProperty* m_prop{};

    GtIntProperty* m_propBounds{};

    GtIntProperty* m_propBounds2{};

    GtIntProperty* m_propBoundsLow{};

    GtIntProperty* m_propBoundsHigh{};

};

TEST_F(TestGtIntProperty, initialization)
{
    // default constructor
    EXPECT_EQ(m_prop->get(), 0);
    EXPECT_EQ(m_prop->lowSideBoundary(),  std::numeric_limits<int>::min());
    EXPECT_EQ(m_prop->highSideBoundary(), std::numeric_limits<int>::max());
    EXPECT_EQ(m_prop->unitCategory(), GtUnit::Category::NonDimensional);
    EXPECT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test int");
    EXPECT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
    EXPECT_FALSE(m_prop->isReadOnly());

    // bounds
    EXPECT_EQ(m_propBounds->get(), 0);
    EXPECT_EQ(m_propBounds->lowSideBoundary(), -1);
    EXPECT_EQ(m_propBounds->highSideBoundary(), 3);
    EXPECT_EQ(m_propBounds->unitCategory(), GtUnit::Category::NonDimensional);
    EXPECT_STREQ(m_propBounds->objectName().toStdString().c_str(),
                 "test int");
    EXPECT_STREQ(m_propBounds->brief().toStdString().c_str(),
                 "test brief");
    EXPECT_FALSE(m_propBounds->isReadOnly());

    // check what happens if default value is outside of boundary
    GtIntProperty propBounds2("bla", "bli", "blup",
                              gt::Boundaries<int>::makeNormalized(1, 3));
    EXPECT_EQ(propBounds2.get(), propBounds2.lowSideBoundary());
    EXPECT_EQ(propBounds2.lowSideBoundary(),  1);
    EXPECT_EQ(propBounds2.highSideBoundary(), 3);

    // bounds low
    EXPECT_EQ(m_propBoundsLow->get(), 4);
    EXPECT_EQ(m_propBoundsLow->lowSideBoundary(), 2);
    EXPECT_EQ(m_propBoundsLow->highSideBoundary(), std::numeric_limits<int>::max());
    EXPECT_EQ(m_propBoundsLow->unitCategory(),
              GtUnit::Category::NonDimensional);
    EXPECT_STREQ(m_propBoundsLow->objectName().toStdString().c_str(),
                 "test int");
    EXPECT_STREQ(m_propBoundsLow->brief().toStdString().c_str(),
                 "test brief");
    EXPECT_FALSE(m_propBoundsLow->isReadOnly());

    // bounds high
    EXPECT_EQ(m_propBoundsHigh->get(), m_propBoundsHigh->highSideBoundary());
    EXPECT_EQ(m_propBoundsHigh->lowSideBoundary(),  std::numeric_limits<int>::min());
    EXPECT_EQ(m_propBoundsHigh->highSideBoundary(), 38);
    EXPECT_EQ(m_propBoundsHigh->unitCategory(),
              GtUnit::Category::NonDimensional);
    EXPECT_STREQ(m_propBoundsHigh->objectName().toStdString().c_str(),
                 "test int");
    EXPECT_STREQ(m_propBoundsHigh->brief().toStdString().c_str(),
                 "test brief");
    EXPECT_FALSE(m_propBoundsHigh->isReadOnly());
}

TEST_F(TestGtIntProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    EXPECT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtIntProperty, getter)
{
    m_prop->setVal(11);
    EXPECT_EQ(m_prop->get(), 11);

    m_prop->setVal(4);
    EXPECT_EQ(*m_prop, 4);
}

TEST_F(TestGtIntProperty, setter)
{
    m_prop->setVal(11);
    EXPECT_EQ(m_prop->get(), 11);

    *m_prop = 230;
    EXPECT_EQ(m_prop->get(), 230);
}

TEST_F(TestGtIntProperty, setFromVariant)
{
    QVariant var(11);

    EXPECT_TRUE(m_prop->setValueFromVariant(var, QString()));
    EXPECT_EQ(m_prop->get(), 11);
}

TEST_F(TestGtIntProperty, getFromVariant)
{
    m_prop->setVal(11);

    bool success = false;
    int val = 0;

    QVariant var = m_prop->valueToVariant(QString(), &success);
    val = var.toDouble();
    EXPECT_EQ(val, 11);
    EXPECT_TRUE(success);
}

TEST_F(TestGtIntProperty, convertTo)
{
    m_prop->setVal(11);
    EXPECT_EQ(m_prop->get(), 11);

    bool success = false;

    EXPECT_EQ(m_prop->getVal(QString(), &success), 11);
    EXPECT_TRUE(success);

    success = false;

    EXPECT_EQ(m_prop->getVal("cm2", &success), 11);
    EXPECT_FALSE(success);

    success = false;

    EXPECT_EQ(m_prop->getVal("cm^2", &success), 11);
    EXPECT_FALSE(success);
}

TEST_F(TestGtIntProperty, convertFrom)
{
    bool success = false;

    m_prop->setVal(11, QString(), &success);
    EXPECT_EQ(m_prop->get(), 11);
    EXPECT_TRUE(success);

    success = false;

    m_prop->setVal(14, "cm2", &success);
    EXPECT_EQ(m_prop->get(), 11);
    EXPECT_FALSE(success);

    success = false;

    m_prop->setVal(123000, "cm^2", &success);
    EXPECT_EQ(m_prop->get(), 11);
    EXPECT_FALSE(success);
}

TEST_F(TestGtIntProperty, bounds)
{
    bool success = false;

    m_propBounds->setVal(-1, &success);
    EXPECT_EQ(m_propBounds->get(), -1);
    EXPECT_TRUE(success);

    success = false;

    m_propBounds->setVal(-2, &success);
    EXPECT_EQ(m_propBounds->get(), -1);
    EXPECT_FALSE(success);

    success = false;

    m_propBounds->setVal(3, &success);
    EXPECT_EQ(m_propBounds->get(), 3);
    EXPECT_TRUE(success);

    success = false;

    m_propBounds->setVal(4, &success);
    EXPECT_EQ(m_propBounds->get(), 3);
    EXPECT_FALSE(success);
}

TEST_F(TestGtIntProperty, boundsLowOnly)
{
    bool success = false;

    m_propBoundsLow->setVal(2, &success);
    EXPECT_EQ(m_propBoundsLow->get(), 2);
    EXPECT_TRUE(success);

    success = false;

    m_propBoundsLow->setVal(-1, &success);
    EXPECT_EQ(m_propBoundsLow->get(), 2);
    EXPECT_FALSE(success);
}

TEST_F(TestGtIntProperty, boundsHighOnly)
{
    bool success = false;

    m_propBoundsHigh->setVal(38, &success);
    EXPECT_EQ(m_propBoundsHigh->get(), 38);
    EXPECT_TRUE(success);

    success = false;

    m_propBoundsHigh->setVal(45, &success);
    EXPECT_EQ(m_propBoundsHigh->get(), 38);
    EXPECT_FALSE(success);
}

TEST_F(TestGtIntProperty, wrongBounds)
{
    GtIntProperty prop("prop", "test", "test", 4, 3, 4);

    EXPECT_EQ(prop.get(), 4);
    EXPECT_EQ(prop.lowSideBoundary(),  std::numeric_limits<int>::min());
    EXPECT_EQ(prop.highSideBoundary(), std::numeric_limits<int>::max());
}

TEST_F(TestGtIntProperty, revert)
{
    GtIntProperty prop("prop", "test", "test", 4);

    EXPECT_EQ(prop.get(), 4);

    prop = 2;

    EXPECT_EQ(prop.get(), 2);

    prop.revert();

    EXPECT_EQ(prop.get(), 4);
}

TEST_F(TestGtIntProperty, optional)
{
    GtIntProperty prop("prop", "test", "test", 4);

    EXPECT_TRUE(prop.isActive());
    EXPECT_FALSE(prop.isOptional());

    prop.setActive(false);

    EXPECT_TRUE(prop.isActive());
    EXPECT_FALSE(prop.isOptional());

    prop.setActive(true);

    EXPECT_TRUE(prop.isActive());
    EXPECT_FALSE(prop.isOptional());

    prop.setOptional(true);

    EXPECT_FALSE(prop.isActive());
    EXPECT_TRUE(prop.isOptional());

    prop.setOptional(false);

    EXPECT_TRUE(prop.isActive());
    EXPECT_FALSE(prop.isOptional());
}

TEST_F(TestGtIntProperty, extendenOperators)
{
    GtIntProperty prop("prop", "test", "test", 4);
    // operator+=
    int sum = 4;
    prop.setVal(13);
	
    prop += sum;
    EXPECT_EQ(prop.get(), 13 + sum);

    // operator-=
    int diff = 4;
    prop.setVal(13);
	
    prop -= diff;
    EXPECT_EQ(prop.get(), 13 - diff);
	
    // operator*=
    int factor = 4;
    prop.setVal(13);
	
    prop *= factor;
    EXPECT_EQ(prop.get(), 13 * factor);
	
    // operator/=
    int div = 4;
    prop.setVal(13);
	
    prop /= div;
    EXPECT_EQ(prop.get(), 3);
}

TEST_F(TestGtIntProperty, boundaries)
{
    // low bound check
    GtIntProperty l("propBoundsLow", "test int",
                    "test brief",
                    gt::Boundaries<int>::makeLower(200), 340);

    double hundred = 100;
    double fourH = 400;

    // set valid value
    l.setVal(fourH);
    EXPECT_DOUBLE_EQ(l.getVal(), fourH);

    // set invalid value
    l.setVal(hundred);
    EXPECT_FALSE(l.getVal() == hundred);
    EXPECT_DOUBLE_EQ(l.getVal(), fourH);

    // high bound check
    GtIntProperty h("propBoundsHigh", "test int",
                    "test brief",
                    gt::Boundaries<int>::makeUpper(200), 140);
    // set valid value
    l.setVal(hundred);
    EXPECT_DOUBLE_EQ(l.getVal(), hundred);

    // set invalid value
    l.setVal(fourH);
    EXPECT_FALSE(l.getVal() == fourH);
    EXPECT_DOUBLE_EQ(l.getVal(), hundred);

    // high bound check
    GtIntProperty lh("propBoundsLowAndHigh", "test int",
                      "test brief",
                      gt::Boundaries<int>::makeNormalized(80, 200),
                      140);

    // set valid value
    l.setVal(hundred);
    EXPECT_DOUBLE_EQ(l.getVal(), hundred);

    // set invalid value
    l.setVal(fourH);
    EXPECT_FALSE(l.getVal() == fourH);
    EXPECT_DOUBLE_EQ(l.getVal(), hundred);

    // set invalid value
    l.setVal(40);
    EXPECT_FALSE(l.getVal() == 40);
    EXPECT_DOUBLE_EQ(l.getVal(), hundred);
}

