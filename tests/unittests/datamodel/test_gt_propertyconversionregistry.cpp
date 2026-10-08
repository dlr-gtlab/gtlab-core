/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Created on: 26.9.2023
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gt_intproperty.h"
#include "gt_stringproperty.h"
#include "gt_boolproperty.h"
#include "gt_propertyconversionregistry.h"

TEST(TestGtPropertyConversionRegistry, canConnectFunctionsUnregistered)
{
    GtIntProperty intProp("int", "intName");

    // no canConnect function is registered for this property type pair
    const auto functions = gtPropConversion().canConnectFunctions(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject);

    EXPECT_TRUE(functions.isEmpty());
}

TEST(TestGtPropertyConversionRegistry, registerCanConnect)
{
    GtBoolProperty boolProp("bool", "boolFrom");
    GtIntProperty intProp("int", "intTo");

    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtBoolProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const& a, GtAbstractProperty const& b) -> bool {
            return a.objectName() == "boolFrom" && b.objectName() == "intTo";
        });

    const auto functions = gtPropConversion().canConnectFunctions(
        GtBoolProperty::staticMetaObject, GtIntProperty::staticMetaObject);

    ASSERT_FALSE(functions.isEmpty());

    // the registered function is returned and behaves as registered
    bool foundRegisteredFunction = false;
    for (const auto& f : functions)
    {
        if (f(boolProp, intProp) && !f(intProp, boolProp))
        {
            foundRegisteredFunction = true;
        }
    }
    EXPECT_TRUE(foundRegisteredFunction);

    // the lookup is directional
    const auto reversed = gtPropConversion().canConnectFunctions(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject);
    EXPECT_TRUE(reversed.isEmpty());
}

TEST(TestGtPropertyConversionRegistry, canConnectWithoutRegisteredFunction)
{
    GtIntProperty intProp1("int1", "intProp1");
    GtIntProperty intProp2("int2", "intProp2");
    GtStringProperty stringProp("string", "stringProp");

    // without a registered canConnect function for the property type
    // pair the connection is only possible for identical types
    EXPECT_TRUE(intProp1.canConnect(intProp2));
    EXPECT_TRUE(intProp2.canConnect(intProp1));

    EXPECT_FALSE(stringProp.canConnect(intProp1));
}

TEST(TestGtPropertyConversionRegistry, canConnectWithRegisteredFunction)
{
    GtBoolProperty boolProp("bool", "boolProp");
    GtBoolProperty otherBoolProp("bool2", "otherBoolProp");
    GtStringProperty stringProp("string", "stringProp");
    GtIntProperty intProp("int", "intProp");

    // a registered canConnect function overrides the fallback to
    // identical property types
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtBoolProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_FALSE(boolProp.canConnect(otherBoolProp));

    // a registered function allows a connection for a property type
    // pair that would be rejected by the fallback
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtIntProperty::staticMetaObject, GtStringProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return true;
        });

    EXPECT_TRUE(intProp.canConnect(stringProp));

    // the connection direction used at registration is respected
    EXPECT_FALSE(stringProp.canConnect(intProp));

    // if multiple functions are registered for a property type pair a
    // single function accepting the connection is sufficient
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtStringProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_TRUE(intProp.canConnect(stringProp));

    // if all registered functions reject the connection the
    // connection is rejected
    GtPropertyConversionRegistry::registerConnectionCompatibility(
        GtStringProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_FALSE(boolProp.canConnect(stringProp));
}
