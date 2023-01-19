/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_doubleproperty
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_doubleproperty.h"


/// This is a test fixture that does a init for each test
class TestGtDoubleProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtDoubleProperty("prop", "test double", "test brief",
                                      GtUnit::Category::Area);

        m_propBounds = new GtDoubleProperty("propBounds", "test double",
                                            "test brief",
                                            GtUnit::Category::Area, -1.33,
                                            142.43);

        m_propBoundsLow = new GtDoubleProperty("propBoundsLow", "test double",
                                               "test brief",
                                               GtUnit::Category::Area,
                                               GtDoubleProperty::BoundLow,
                                               2.43, 142.43);

        m_propBoundsHigh = new GtDoubleProperty("propBoundsHigh", "test double",
                                                "test brief",
                                                GtUnit::Category::Area,
                                                GtDoubleProperty::BoundHigh,
                                                200.43, 340.2);
    }

    virtual void TearDown()
    {
        delete m_prop;

        delete m_propBounds;

        delete m_propBoundsLow;

        delete m_propBoundsHigh;
    }

    GtDoubleProperty* m_prop;

    GtDoubleProperty* m_propBounds;

    GtDoubleProperty* m_propBoundsLow;

    GtDoubleProperty* m_propBoundsHigh;

};

TEST_F(TestGtDoubleProperty, initialization)
{
    // default constructor
    ASSERT_DOUBLE_EQ(m_prop->get(), 0.0);
    ASSERT_DOUBLE_EQ(m_prop->lowSideBoundary(), 0.0);
    ASSERT_DOUBLE_EQ(m_prop->highSideBoundary(), 0.0);
    ASSERT_DOUBLE_EQ(m_prop->unitCategory(), GtUnit::Category::Area);
    ASSERT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test double");
    ASSERT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_prop->isReadOnly());

    // bounds
    ASSERT_DOUBLE_EQ(m_propBounds->get(), 0.0);
    ASSERT_DOUBLE_EQ(m_propBounds->lowSideBoundary(), -1.33);
    ASSERT_DOUBLE_EQ(m_propBounds->highSideBoundary(), 142.43);
    ASSERT_DOUBLE_EQ(m_propBounds->unitCategory(), GtUnit::Category::Area);
    ASSERT_STREQ(m_propBounds->objectName().toStdString().c_str(),
                 "test double");
    ASSERT_STREQ(m_propBounds->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBounds->isReadOnly());

    // bounds low
    ASSERT_DOUBLE_EQ(m_propBoundsLow->get(), 142.43);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->lowSideBoundary(), 2.43);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->highSideBoundary(), 0.0);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->unitCategory(), GtUnit::Category::Area);
    ASSERT_STREQ(m_propBoundsLow->objectName().toStdString().c_str(),
                 "test double");
    ASSERT_STREQ(m_propBoundsLow->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBoundsLow->isReadOnly());

    // bounds high
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->get(), 0.0);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->lowSideBoundary(), 0.0);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->highSideBoundary(), 200.43);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->unitCategory(), GtUnit::Category::Area);
    ASSERT_STREQ(m_propBoundsHigh->objectName().toStdString().c_str(),
                 "test double");
    ASSERT_STREQ(m_propBoundsHigh->brief().toStdString().c_str(),
                 "test brief");
    ASSERT_FALSE(m_propBoundsHigh->isReadOnly());
}

TEST_F(TestGtDoubleProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    ASSERT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtDoubleProperty, getter)
{
    m_prop->setVal(11.3);
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.3);

    m_prop->setVal(4.3);
    ASSERT_DOUBLE_EQ(*m_prop, 4.3);
}

TEST_F(TestGtDoubleProperty, setter)
{
    m_prop->setVal(11.3);
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.3);

    *m_prop = 230.2;
    ASSERT_DOUBLE_EQ(m_prop->get(), 230.2);

//    m_prop->setReadOnly(true);
//    m_prop->setVal(12.3);
//    ASSERT_NE(m_prop->get(), 12.3);
//    ASSERT_DOUBLE_EQ(m_prop->get(), 230.2);
}

TEST_F(TestGtDoubleProperty, setFromVariant)
{
    QVariant var(11.2);

    bool success = false;

    success = m_prop->setValueFromVariant(var, m_prop->siUnit());
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.2);
    ASSERT_TRUE(success);

    success = false;
    var = 14.2;

    success = m_prop->setValueFromVariant(var, "mm2");
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.2);
    ASSERT_FALSE(success);

    success = false;
    var = 15.2;

//    m_prop->setReadOnly(true);
//    m_prop->setValueFromVariant(var, m_prop->siUnit(), &success);
//    ASSERT_DOUBLE_EQ(m_prop->get(), 11.2);
//    ASSERT_NE(m_prop->get(), 15.2);
//    ASSERT_FALSE(success);
}

TEST_F(TestGtDoubleProperty, getFromVariant)
{
    m_prop->setVal(11.3);

    bool success = false;
    double val = 0.0;

    QVariant var = m_prop->valueToVariant(m_prop->siUnit(), &success);
    val = var.toDouble();
    ASSERT_DOUBLE_EQ(val, 11.3);
    ASSERT_TRUE(success);

    success = false;
    val = 0.0;

    var = m_prop->valueToVariant("mm2", &success);
    val = var.toDouble();
    ASSERT_DOUBLE_EQ(val, 11.3);
    ASSERT_FALSE(success);
}

TEST_F(TestGtDoubleProperty, convertTo)
{
    m_prop->setVal(11.3);
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.3);

    bool success = false;

    ASSERT_DOUBLE_EQ(m_prop->getVal(m_prop->siUnit(), &success), 11.3);
    ASSERT_TRUE(success);

    success = false;

    ASSERT_DOUBLE_EQ(m_prop->getVal("cm2", &success), 11.3);
    ASSERT_FALSE(success);

    success = false;

    ASSERT_DOUBLE_EQ(m_prop->getVal("cm^2", &success), 113000);
    ASSERT_TRUE(success);
}

TEST_F(TestGtDoubleProperty, convertFrom)
{
    bool success = false;

    m_prop->setVal(11.3, m_prop->siUnit(), &success);
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.3);
    ASSERT_TRUE(success);

    success = false;

    m_prop->setVal(14.3, "cm2", &success);
    ASSERT_DOUBLE_EQ(m_prop->get(), 11.3);
    ASSERT_FALSE(success);

    success = false;

    m_prop->setVal(123000, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_prop->get(), 12.3);
    ASSERT_TRUE(success);
}

TEST_F(TestGtDoubleProperty, bounds)
{
    bool success = false;

    m_propBounds->setVal(-1.33, &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), -1.33);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(-2.33, &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), -1.33);
    ASSERT_FALSE(success);

    success = false;

    m_propBounds->setVal(142.43, &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), 142.43);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(242.43, &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), 142.43);
    ASSERT_FALSE(success);

    success = false;

    m_propBounds->setVal(-12300, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), -1.23);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(-22300, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), -1.23);
    ASSERT_FALSE(success);

    success = false;

    m_propBounds->setVal(1424300, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), 142.43);
    ASSERT_TRUE(success);

    success = false;

    m_propBounds->setVal(2424300, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBounds->get(), 142.43);
    ASSERT_FALSE(success);
}

TEST_F(TestGtDoubleProperty, boundsLowOnly)
{
    bool success = false;

    m_propBoundsLow->setVal(2.43, &success);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->get(), 2.43);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsLow->setVal(-2.33, &success);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->get(), 2.43);
    ASSERT_FALSE(success);

    success = false;

    m_propBoundsLow->setVal(34500, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->get(), 3.45);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsLow->setVal(-22300, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBoundsLow->get(), 3.45);
    ASSERT_FALSE(success);
}

TEST_F(TestGtDoubleProperty, boundsHighOnly)
{
    bool success = false;

    m_propBoundsHigh->setVal(2.43, &success);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->get(), 2.43);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsHigh->setVal(324.2, &success);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->get(), 2.43);
    ASSERT_FALSE(success);

    success = false;

    m_propBoundsHigh->setVal(34500, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->get(), 3.45);
    ASSERT_TRUE(success);

    success = false;

    m_propBoundsHigh->setVal(4792929, "cm^2", &success);
    ASSERT_DOUBLE_EQ(m_propBoundsHigh->get(), 3.45);
    ASSERT_FALSE(success);
}

TEST_F(TestGtDoubleProperty, wrongBounds)
{
    GtDoubleProperty prop("prop", "test", "test", GtUnit::Category::None,
                          4.0, 3.0, 4.0);

    ASSERT_DOUBLE_EQ(prop.get(), 4.0);
    ASSERT_DOUBLE_EQ(prop.lowSideBoundary(), 0.0);
    ASSERT_DOUBLE_EQ(prop.highSideBoundary(), 0.0);
}

TEST_F(TestGtDoubleProperty, revert)
{
    GtDoubleProperty prop("prop", "test", "test", GtUnit::Category::None, 4.0);

    ASSERT_DOUBLE_EQ(prop.get(), 4.0);

    prop = 2.3;

    ASSERT_DOUBLE_EQ(prop.get(), 2.3);

    prop.revert();

    ASSERT_DOUBLE_EQ(prop.get(), 4.0);
}

TEST_F(TestGtDoubleProperty, optional)
{
    GtDoubleProperty prop("prop", "test", "test", GtUnit::Category::None, 4.0);

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
