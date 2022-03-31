/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_enumproperty.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +49 551 709 2708
 */
#include "gtest/gtest.h"

#include "gt_enumproperty.h"

#include "test_gt_enumproperty.h"

class TestGtEnumProperty : public ::testing::Test
{

protected:
    virtual void SetUp()
    {
        m_prop = new GtEnumProperty<EnumContainer::TestEnum>("prop",
                                                             "test enum",
                                                             "test brief");

        m_propWithValue = new GtEnumProperty<EnumContainer::TestEnum>("propWValue",
                                                                      "test enum with value",
                                                                      "test brief with value",
                                                                      EnumContainer::TestEnum::B);
    }

    virtual void TearDown()
    {
        delete m_prop;
        delete m_propWithValue;
    }

    GtEnumProperty<EnumContainer::TestEnum>* m_prop;

    GtEnumProperty<EnumContainer::TestEnum>* m_propWithValue;
};

TEST_F(TestGtEnumProperty, initialization)
{
    ASSERT_EQ(m_prop->getVal(), EnumContainer::TestEnum::A);
    ASSERT_EQ(m_propWithValue->getVal(), EnumContainer::TestEnum::B);

    // Do not test other init things here since they are already tested in modeProperty test
}

TEST_F(TestGtEnumProperty, setter)
{
    m_prop->setVal(EnumContainer::TestEnum::C);
    ASSERT_EQ(m_prop->getVal(), EnumContainer::TestEnum::C);
}

TEST_F(TestGtEnumProperty, implicitConversions)
{
    m_prop->setVal(EnumContainer::TestEnum::C);
    ASSERT_EQ(*m_prop, EnumContainer::TestEnum::C);

    *m_prop = EnumContainer::TestEnum::C;
    ASSERT_EQ(m_prop->getVal(), EnumContainer::TestEnum::C);
}
