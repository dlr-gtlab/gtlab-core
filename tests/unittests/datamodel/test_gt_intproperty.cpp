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

        m_propBoundsLow = new GtIntProperty("propBoundsLow", "test int", "test brief",
                                            gt::Boundaries<int>::makeLower(2),
                                            4);

        m_propBoundsHigh = new GtIntProperty("propBoundsHigh", "test int", "test brief",
                                             gt::Boundaries<int>::makeUpper(38),
                                             54);

        m_propBounds = new GtIntProperty("propBounds", "test int", "test brief",
                                         gt::Boundaries<int>::makeNormalized(-1, 3));
    }

    virtual void TearDown()
    {
        delete m_prop;

        delete m_propBoundsLow;

        delete m_propBoundsHigh;

        delete m_propBounds;
    }

    GtIntProperty* m_prop{};

    GtIntProperty* m_propBoundsLow{};

    GtIntProperty* m_propBoundsHigh{};

    GtIntProperty* m_propBounds{};

};

TEST_F(TestGtIntProperty, initialization_deprecated)
{
    // bounded property
    GtIntProperty prop("propBounds", "test int", "test brief", -1, 3);
    EXPECT_EQ(prop.get(), 0);
    EXPECT_EQ(prop.lowSideBoundary(), -1);
    EXPECT_EQ(prop.highSideBoundary(), 3);
    EXPECT_EQ(prop.unitCategory(), GtUnit::Category::NonDimensional);
    EXPECT_STREQ(prop.objectName().toStdString().c_str(), "test int");
    EXPECT_STREQ(prop.brief().toStdString().c_str(), "test brief");
    EXPECT_FALSE(prop.isReadOnly());

    // bounds low
    GtIntProperty propLow("propBounds", "test int", "test brief", GtIntProperty::BoundLow, 2, 4);
    EXPECT_EQ(propLow.get(), 4);
    EXPECT_EQ(propLow.lowSideBoundary(), 2);
    EXPECT_EQ(propLow.highSideBoundary(), std::numeric_limits<int>::max());
    EXPECT_EQ(propLow.unitCategory(), GtUnit::Category::NonDimensional);
    EXPECT_STREQ(propLow.objectName().toStdString().c_str(), "test int");
    EXPECT_STREQ(propLow.brief().toStdString().c_str(), "test brief");
    EXPECT_FALSE(propLow.isReadOnly());

    // bounds high
    GtIntProperty propHigh("propBounds", "test int", "test brief", GtIntProperty::BoundHigh, 38, 4);
    EXPECT_EQ(propHigh.get(), 4);
    EXPECT_EQ(propHigh.lowSideBoundary(),  std::numeric_limits<int>::min());
    EXPECT_EQ(propHigh.highSideBoundary(), 38);
    EXPECT_EQ(propHigh.unitCategory(), GtUnit::Category::NonDimensional);
    EXPECT_STREQ(propHigh.objectName().toStdString().c_str(), "test int");
    EXPECT_STREQ(propHigh.brief().toStdString().c_str(), "test brief");
    EXPECT_FALSE(propHigh.isReadOnly());
}

TEST_F(TestGtIntProperty, bounds_deprecated)
{
    GtIntProperty prop("propBounds", "test int", "test brief", -1, 3);
    bool success = false;

    prop.setVal(-1, &success);
    EXPECT_EQ(prop.get(), -1);
    EXPECT_TRUE(success);

    success = false;

    prop.setVal(-2, &success);
    EXPECT_EQ(prop.get(), -1);
    EXPECT_FALSE(success);

    success = false;

    prop.setVal(3, &success);
    EXPECT_EQ(prop.get(), 3);
    EXPECT_TRUE(success);

    success = false;

    prop.setVal(4, &success);
    EXPECT_EQ(prop.get(), 3);
    EXPECT_FALSE(success);

    GtIntProperty propWrong("prop", "test", "test", 4, 3, 4);

    EXPECT_EQ(propWrong.get(), 4);
    EXPECT_EQ(propWrong.lowSideBoundary(),  std::numeric_limits<int>::min());
    EXPECT_EQ(propWrong.highSideBoundary(), std::numeric_limits<int>::max());
}

TEST_F(TestGtIntProperty, MakeIntPropertyCreatesCorrectProperty_deprecated)
{
    auto factory1 = gt::makeIntProperty(0, 100, 42);

    std::unique_ptr<GtIntProperty> property1(
        dynamic_cast<GtIntProperty*>(factory1("testId")));

    ASSERT_NE(property1, nullptr);
    EXPECT_EQ(property1->ident(), "testId");
    EXPECT_EQ(property1->get(), 42);
    EXPECT_EQ(property1->lowSideBoundary(), 0);
    EXPECT_EQ(property1->highSideBoundary(), 100);

    auto factory2 = gt::makeIntProperty(GtIntProperty::BoundHigh, 90, 36);

    std::unique_ptr<GtIntProperty> property2(
        dynamic_cast<GtIntProperty*>(factory2("testId2")));

    ASSERT_NE(property2, nullptr);
    EXPECT_EQ(property2->ident(), "testId2");
    EXPECT_EQ(property2->get(), 36);
    EXPECT_EQ(property2->highSideBoundary(), 90);
    EXPECT_EQ(property2->lowSideBoundary(),  std::numeric_limits<int>::min());

    auto factory3 = gt::makeIntProperty(GtIntProperty::BoundLow, 15, 35);

    std::unique_ptr<GtIntProperty> property3(
        dynamic_cast<GtIntProperty*>(factory3("testId3")));

    ASSERT_NE(property3, nullptr);
    EXPECT_EQ(property3->ident(), "testId3");
    EXPECT_EQ(property3->get(), 35);
    EXPECT_EQ(property3->highSideBoundary(), std::numeric_limits<int>::max());
    EXPECT_EQ(property3->lowSideBoundary(),  15);
}

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
    EXPECT_STREQ(m_propBounds->objectName().toStdString().c_str(), "test int");
    EXPECT_STREQ(m_propBounds->brief().toStdString().c_str(), "test brief");
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
    EXPECT_STREQ(m_propBoundsHigh->brief().toStdString().c_str(), "test brief");
    EXPECT_FALSE(m_propBoundsHigh->isReadOnly());

    GtIntProperty propBoundsStart("propBounds", "test int", "test brief",
                                  gt::Boundaries<int>::makeNormalized(-2, 4), 2);
    EXPECT_EQ(propBoundsStart.get(), 2);
    EXPECT_EQ(propBoundsStart.lowSideBoundary(), -2);
    EXPECT_EQ(propBoundsStart.highSideBoundary(), 4);

    GtIntProperty propBoundsStart2("propBounds", "test int", "test brief",
                                  gt::Boundaries<int>::makeNormalized(-2, 4), 6);
    EXPECT_EQ(propBoundsStart2.get(), 4);
    EXPECT_EQ(propBoundsStart2.lowSideBoundary(), -2);
    EXPECT_EQ(propBoundsStart2.highSideBoundary(), 4);

    GtIntProperty propBoundsStart3("propBounds", "test int", "test brief",
                                   gt::Boundaries<int>::makeNormalized(-3, 5), -6);
    EXPECT_EQ(propBoundsStart3.lowSideBoundary(), -3);
    EXPECT_EQ(propBoundsStart3.highSideBoundary(), 5);
    EXPECT_EQ(propBoundsStart3.get(), -3);
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
    GtIntProperty prop2("prop", "test", "test",
                        gt::Boundaries<int>::makeNormalized(4, 3), 4);

    EXPECT_EQ(prop2.get(), 4);
    EXPECT_EQ(prop2.lowSideBoundary(),  3);
    EXPECT_EQ(prop2.highSideBoundary(), 4);
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
    bool success = false;

    EXPECT_TRUE(l.lowSideBoundaryActive());
    EXPECT_FALSE(l.highSideBoundaryActive());
    ASSERT_DOUBLE_EQ(l.getVal(), 340);

    // set valid value
    l.setVal(fourH, &success);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(l.getVal(), fourH);

    // set invalid value
    l.setVal(hundred, &success);
    EXPECT_FALSE(success);
    EXPECT_FALSE(l.getVal() == hundred);
    EXPECT_DOUBLE_EQ(l.getVal(), fourH);

    // high bound check
    GtIntProperty h("propBoundsHigh", "test int",
                    "test brief",
                    gt::Boundaries<int>::makeUpper(200), 140);

    EXPECT_FALSE(h.lowSideBoundaryActive());
    EXPECT_TRUE(h.highSideBoundaryActive());
    ASSERT_DOUBLE_EQ(h.getVal(), 140);

    // set valid value
    h.setVal(hundred, &success);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(h.getVal(), hundred);

    // set invalid value
    h.setVal(fourH, &success);
    EXPECT_FALSE(success);
    EXPECT_FALSE(h.getVal() == fourH);
    EXPECT_DOUBLE_EQ(h.getVal(), hundred);

    // high bound check
    GtIntProperty lh("propBoundsLowAndHigh", "test int",
                      "test brief",
                      gt::Boundaries<int>::makeNormalized(80, 200),
                      140);

    EXPECT_TRUE(lh.lowSideBoundaryActive());
    EXPECT_TRUE(lh.highSideBoundaryActive());
    ASSERT_DOUBLE_EQ(lh.getVal(), 140);

    // set valid value
    lh.setVal(hundred, &success);
    EXPECT_TRUE(success);
    EXPECT_DOUBLE_EQ(lh.getVal(), hundred);

    // set invalid value
    lh.setVal(fourH, &success);
    EXPECT_FALSE(success);
    EXPECT_FALSE(lh.getVal() == fourH);
    EXPECT_DOUBLE_EQ(lh.getVal(), hundred);

    // set invalid value
    lh.setVal(40, &success);
    EXPECT_FALSE(success);
    EXPECT_FALSE(lh.getVal() == 40);
    EXPECT_DOUBLE_EQ(lh.getVal(), hundred);
}

TEST_F(TestGtIntProperty, MakeIntPropertyCreatesCorrectProperty)
{
    auto bounds = gt::Boundaries<int>::makeNormalized(0, 100);

    auto factory = gt::makeIntProperty(bounds, 42);

    std::unique_ptr<GtIntProperty> property(
        dynamic_cast<GtIntProperty*>(factory("testId")));

    ASSERT_NE(property, nullptr);

    EXPECT_EQ(property->ident(), "testId");
    EXPECT_EQ(property->get(), 42);
    EXPECT_EQ(property->lowSideBoundary(), 0);
    EXPECT_EQ(property->highSideBoundary(), 100);
}
