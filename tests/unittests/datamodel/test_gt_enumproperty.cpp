/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_dummyobject.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +49 551 709 2708
 */
#include "gtest/gtest.h"

#include "gt_enumproperty.h"

enum TestEnum {
    A = 0,
    B = 3,
    C
};

class TestGtEnumProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtEnumProperty<TestEnum>("prop",
                                              "test enum",
                                              "test brief");

        m_propWithValue = new GtEnumProperty<TestEnum>("prop",
                                                       "test enum",
                                                       "test brief",
                                                       TestEnum::B);
    }

    virtual void TearDown()
    {
        delete m_prop;
        delete m_propWithValue;
    }

    GtEnumProperty<TestEnum>* m_prop;

    GtEnumProperty<TestEnum>* m_propWithValue;
};

TEST_F(TestGtEnumProperty, initialization)
{
    ASSERT_EQ(m_prop->getVal(), TestEnum::A);
    ASSERT_EQ(m_propWithValue->getVal(), TestEnum::B);

    // Do not test other init things here since they are already tested in modeProperty test
}

TEST_F(TestGtEnumProperty, setter)
{
    m_prop->setVal(TestEnum::C);
    ASSERT_EQ(m_prop->getVal(), TestEnum::C);
}

TEST_F(TestGtEnumProperty, implicitConversions)
{
    m_prop->setVal(TestEnum::C);
    ASSERT_EQ(*m_prop, TestEnum::C);

    m_prop = TestEnum::C;
    ASSERT_EQ(m_prop->getVal(), TestEnum::C);
}
