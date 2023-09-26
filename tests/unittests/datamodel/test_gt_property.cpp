/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 26.9.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gt_intproperty.h"
#include "gt_doubleproperty.h"
#include "gt_stringproperty.h"
#include "gt_boolproperty.h"

TEST(TestGtProperty, makeReadOnly)
{
    auto factory = gt::makeIntProperty(42);
    EXPECT_FALSE(factory("bla")->isReadOnly());

    factory = gt::makeReadOnly(gt::makeIntProperty(42));
    EXPECT_TRUE(factory("bla")->isReadOnly());
}

TEST(TestGtProperty, makeHidden)
{
    auto factory = gt::makeDoubleProperty(42.0);
    EXPECT_FALSE(factory("bla")->isHidden());

    factory = gt::makeHidden(gt::makeDoubleProperty(42.0));
    EXPECT_TRUE(factory("bla")->isHidden());
}

TEST(TestGtProperty, makeOptional)
{
    auto factory = gt::makeStringProperty("42.0");
    EXPECT_FALSE(factory("bla")->isOptional());
    EXPECT_TRUE(factory("bla")->isActive());

    factory = gt::makeOptional(gt::makeStringProperty("42.0"));
    EXPECT_TRUE(factory("bla")->isOptional());
    EXPECT_FALSE(factory("bla")->isActive());

    factory = gt::makeOptional(gt::makeStringProperty("42.0"), true);
    EXPECT_TRUE(factory("bla")->isOptional());
    EXPECT_TRUE(factory("bla")->isActive());
}

TEST(TestGtProperty, makeComplexProperty)
{
    auto factory = gt::makeBoolProperty(true);
    EXPECT_FALSE(factory("bla")->isOptional());
    EXPECT_FALSE(factory("bla")->isHidden());
    EXPECT_FALSE(factory("bla")->isReadOnly());

    factory = gt::makeHidden(gt::makeReadOnly(gt::makeOptional(gt::makeBoolProperty(true))));
    EXPECT_TRUE(factory("bla")->isOptional());
    EXPECT_TRUE(factory("bla")->isHidden());
    EXPECT_TRUE(factory("bla")->isReadOnly());
}
