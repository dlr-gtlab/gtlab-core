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
    std::unique_ptr<GtAbstractProperty> intProp(factory("bla"));

    EXPECT_FALSE(intProp->isReadOnly());

    factory = gt::makeReadOnly(gt::makeIntProperty(42));
    std::unique_ptr<GtAbstractProperty> readOnlyProp(factory("bla"));

    EXPECT_TRUE(readOnlyProp->isReadOnly());
}

TEST(TestGtProperty, makeHidden)
{
    auto factory = gt::makeDoubleProperty(42.0);
    std::unique_ptr<GtAbstractProperty> doubleProp(factory("bla"));
    EXPECT_FALSE(doubleProp->isHidden());

    factory = gt::makeHidden(gt::makeDoubleProperty(42.0));
    std::unique_ptr<GtAbstractProperty> hiddenProp(factory("bla"));

    EXPECT_TRUE(hiddenProp->isHidden());
}

TEST(TestGtProperty, makeOptional)
{
    auto factory = gt::makeStringProperty("42.0");
    std::unique_ptr<GtAbstractProperty> stringProp(factory("bla"));

    EXPECT_FALSE(stringProp->isOptional());
    EXPECT_TRUE(stringProp->isActive());

    factory = gt::makeOptional(gt::makeStringProperty("42.0"));
    std::unique_ptr<GtAbstractProperty> optionalProp(factory("bla"));

    EXPECT_TRUE(optionalProp->isOptional());
    EXPECT_FALSE(optionalProp->isActive());

    factory = gt::makeOptional(gt::makeStringProperty("42.0"), true);
    std::unique_ptr<GtAbstractProperty> activeProp(factory("bla"));
    EXPECT_TRUE(activeProp->isOptional());
    EXPECT_TRUE(activeProp->isActive());
}

TEST(TestGtProperty, makeComplexProperty)
{
    auto factory = gt::makeBoolProperty(true);
    std::unique_ptr<GtAbstractProperty> boolProp(factory("bla"));

    EXPECT_FALSE(boolProp->isOptional());
    EXPECT_FALSE(boolProp->isHidden());
    EXPECT_FALSE(boolProp->isReadOnly());

    factory = gt::makeHidden(gt::makeReadOnly(gt::makeOptional(gt::makeBoolProperty(true))));
    std::unique_ptr<GtAbstractProperty> hiddenProp(factory("bla"));
    EXPECT_TRUE(hiddenProp->isOptional());
    EXPECT_TRUE(hiddenProp->isHidden());
    EXPECT_TRUE(hiddenProp->isReadOnly());
}
