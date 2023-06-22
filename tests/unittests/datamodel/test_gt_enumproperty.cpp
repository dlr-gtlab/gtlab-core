/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_enumproperty.cpp
 *
 *  Created on: 31.03.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +49 551 709 2708
 */
#include "test_gt_enumproperty.h"

#include "gt_enumproperty.h"

#include "gtest/gtest.h"

class TestGtEnumProperty : public ::testing::Test
{
protected:
    GtEnumProperty<EnumContainer::TestEnum> m_prop{
        "prop", "test enum", "test brief"
    };

    GtEnumProperty<EnumContainer::TestEnum> m_propWithValue {
        "propWValue", "test enum with value", "test brief with value",
        EnumContainer::TestEnum::B
    };
};

TEST_F(TestGtEnumProperty, initialization)
{
    ASSERT_EQ(m_prop.getVal(), EnumContainer::TestEnum::A);
    ASSERT_EQ(m_propWithValue.getVal(), EnumContainer::TestEnum::B);

    // Do not test other init things here since they are already tested in modeProperty test
}

TEST_F(TestGtEnumProperty, setter)
{
    m_prop.setVal(EnumContainer::TestEnum::C);
    ASSERT_EQ(m_prop.getVal(), EnumContainer::TestEnum::C);
}

TEST_F(TestGtEnumProperty, implicitConversions)
{
    m_prop.setVal(EnumContainer::TestEnum::C);
    ASSERT_EQ(m_prop, EnumContainer::TestEnum::C);

    m_prop = EnumContainer::TestEnum::C;
    ASSERT_EQ(m_prop.getVal(), EnumContainer::TestEnum::C);
}
