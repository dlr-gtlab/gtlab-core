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

#include "test_gt_propertyconversionregistry.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"
#include "gt_stringproperty.h"
#include "gt_boolproperty.h"
#include "gt_propertyconversionregistry.h"

// TEST(TestGtPropertyConversionRegistry, singletonInstance)
// {
//     EXPECT_EQ(&gtPropConversion(),
//               &GtPropertyConversionRegistry::getInstance());
// }

// TEST(TestGtPropertyConversionRegistry, canConnectFunctionUnregistered)
// {
//     // no canConnect function is registered for this property type pair
//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject));
// }

// TEST(TestGtPropertyConversionRegistry, builtInConversionRegistration)
// {
//     GtIntProperty intProp("int", "intName");
//     GtDoubleProperty doubleProp("double", "doubleName", "doubleBrief",
//                                 GtUnit::Category::None, 4.0);

//     // the registry is initialized with a built in converter for both
//     // double <-> int directions
//     EXPECT_TRUE(gtPropConversion().canConnectFunction(
//         GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject));
//     EXPECT_TRUE(gtPropConversion().canConnectFunction(
//         GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject));

//     EXPECT_TRUE(doubleProp.canConnect(intProp));
//     EXPECT_TRUE(intProp.canConnect(doubleProp));
// }

// TEST(TestGtPropertyConversionRegistry, registerCanConnect)
// {
//     GtBoolProperty boolProp("bool", "boolFrom");
//     GtIntProperty intProp("int", "intTo");

//     gtPropConversion().registerConnectionCompatibility(
//         GtBoolProperty::staticMetaObject, GtIntProperty::staticMetaObject,
//         [](GtAbstractProperty const& a, GtAbstractProperty const& b) -> bool {
//             return a.objectName() == "boolFrom" && b.objectName() == "intTo";
//         });

//     const auto function = gtPropConversion().canConnectFunction(
//         GtBoolProperty::staticMetaObject, GtIntProperty::staticMetaObject);

//     // the registered function is returned and behaves as registered
//     ASSERT_TRUE(function);
//     EXPECT_TRUE(function(boolProp, intProp));
//     EXPECT_FALSE(function(boolProp, GtIntProperty("int", "otherInt")));

//     // the lookup is directional
//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject));
// }

// TEST(TestGtPropertyConversionRegistry, canConnectWithoutRegisteredFunction)
// {
//     GtIntProperty intProp1("int1", "intProp1");
//     GtIntProperty intProp2("int2", "intProp2");
//     GtStringProperty stringProp("string", "stringProp");

//     // for identical types a connection is possible (int properties are
//     // connected via the registered boundary check, the identical type
//     // fallback would accept them as well)
//     EXPECT_TRUE(intProp1.canConnect(intProp2));
//     EXPECT_TRUE(intProp2.canConnect(intProp1));

//     // for a property type pair without a registered canConnect function
//     // and different types the connection is rejected
//     EXPECT_FALSE(stringProp.canConnect(intProp1));
// }

// TEST(TestGtPropertyConversionRegistry, canConnectWithRegisteredFunction)
// {
//     GtBoolProperty boolProp("bool", "boolProp");
//     GtBoolProperty otherBoolProp("bool2", "otherBoolProp");
//     GtStringProperty stringProp("string", "stringProp");
//     GtIntProperty intProp("int", "intProp");

//     // a registered canConnect function overrides the fallback to
//     // identical property types
//     gtPropConversion().registerConnectionCompatibility(
//         GtBoolProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return false;
//         });

//     EXPECT_FALSE(boolProp.canConnect(otherBoolProp));

//     // a registered function allows a connection for a property type
//     // pair that would be rejected by the fallback
//     gtPropConversion().registerConnectionCompatibility(
//         GtIntProperty::staticMetaObject, GtStringProperty::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return true;
//         });

//     EXPECT_TRUE(intProp.canConnect(stringProp));

//     // the connection direction used at registration is respected
//     EXPECT_FALSE(stringProp.canConnect(intProp));

//     // if multiple functions are registered for the same property type
//     // pair the first registered function decides the result
//     gtPropConversion().registerConnectionCompatibility(
//         GtIntProperty::staticMetaObject, GtStringProperty::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return false;
//         });

//     EXPECT_TRUE(intProp.canConnect(stringProp));

//     // a registration for the reversed direction does not influence
//     // the connection check of the other direction
//     gtPropConversion().registerConnectionCompatibility(
//         GtStringProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return false;
//         });

//     EXPECT_FALSE(boolProp.canConnect(stringProp));
// }

// TEST(TestGtPropertyConversionRegistry, inheritanceLookup)
// {
//     GtTestRegistryPropA propA;
//     propA.setObjectName("testRegistryA");
//     GtTestRegistryPropB propB;
//     propB.setObjectName("testRegistryB");

//     // a registration for the derived from type and the base to type
//     // is found via the inheritance chains
//     gtPropConversion().registerConnectionCompatibility(
//         GtTestRegistryPropA::staticMetaObject,
//         GtAbstractProperty::staticMetaObject,
//         [](GtAbstractProperty const& a, GtAbstractProperty const&) -> bool {
//             return a.objectName() == "testRegistryA";
//         });

//     const auto function = gtPropConversion().canConnectFunction(
//         GtTestRegistryPropA::staticMetaObject,
//         GtTestRegistryPropB::staticMetaObject);

//     ASSERT_TRUE(function);
//     EXPECT_TRUE(function(propA, propB));

//     GtTestRegistryPropA otherPropA;
//     otherPropA.setObjectName("otherTestRegistryA");
//     EXPECT_FALSE(function(otherPropA, propB));

//     // the property level check benefits from the same registration
//     EXPECT_TRUE(propA.canConnect(propB));

//     // the lookup is directional, a base to type registration does not
//     // extend the from type
//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtTestRegistryPropB::staticMetaObject,
//         GtTestRegistryPropA::staticMetaObject));
// }

// TEST(TestGtPropertyConversionRegistry, inheritanceLookupPriority)
// {
//     GtTestRegistryPropC propC;
//     GtTestRegistryPropD propD;
//     GtTestRegistryPropE propE;

//     // two registrations match the pair (D, E) via different
//     // inheritance levels, the registration of the more derived from
//     // type takes priority
//     gtPropConversion().registerConnectionCompatibility(
//         GtTestRegistryPropD::staticMetaObject,
//         GtAbstractProperty::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return false;
//         });

//     gtPropConversion().registerConnectionCompatibility(
//         GtAbstractProperty::staticMetaObject,
//         GtTestRegistryPropE::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return true;
//         });

//     const auto functionDE = gtPropConversion().canConnectFunction(
//         GtTestRegistryPropD::staticMetaObject,
//         GtTestRegistryPropE::staticMetaObject);

//     // the function of the more derived from type is returned
//     ASSERT_TRUE(functionDE);
//     EXPECT_FALSE(functionDE(propD, propE));

//     // for a from type without a dedicated registration the base type
//     // registration is used
//     const auto functionCE = gtPropConversion().canConnectFunction(
//         GtTestRegistryPropC::staticMetaObject,
//         GtTestRegistryPropE::staticMetaObject);

//     ASSERT_TRUE(functionCE);
//     EXPECT_TRUE(functionCE(propC, propE));

//     // no registration matches the pair (C, D)
//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtTestRegistryPropC::staticMetaObject,
//         GtTestRegistryPropD::staticMetaObject));
// }

// TEST(TestGtPropertyConversionRegistry, convertOnlyRegistration)
// {
//     GtTestRegistryPropF propF;
//     GtTestRegistryPropG propG;

//     // a converter registered without a canConnect function does not
//     // enable a connection for the property type pair
//     gtPropConversion().registerConnectionCompatibility(
//         GtTestRegistryPropF::staticMetaObject,
//         GtTestRegistryPropG::staticMetaObject,
//         [](GtAbstractProperty const&, GtAbstractProperty&)
//             -> gt::conversion::conversionSuccess {
//             return gt::conversion::conversionSuccess::Success;
//         });

//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtTestRegistryPropF::staticMetaObject,
//         GtTestRegistryPropG::staticMetaObject));

//     EXPECT_FALSE(propF.canConnect(propG));
//     EXPECT_FALSE(propG.canConnect(propF));
// }

// TEST(TestGtPropertyConversionRegistry, emptyFunctionRegistrationIgnored)
// {
//     GtTestRegistryPropH propH1;
//     GtTestRegistryPropH propH2;
//     GtTestRegistryPropI propI;

//     // registering empty functions is ignored
//     gtPropConversion().registerConnectionCompatibility(
//         GtTestRegistryPropH::staticMetaObject,
//         GtTestRegistryPropI::staticMetaObject,
//         gt::conversion::convert{});

//     gtPropConversion().registerConnectionCompatibility(
//         GtTestRegistryPropH::staticMetaObject,
//         GtTestRegistryPropI::staticMetaObject,
//         gt::conversion::canConnect{});

//     EXPECT_FALSE(gtPropConversion().canConnectFunction(
//         GtTestRegistryPropH::staticMetaObject,
//         GtTestRegistryPropI::staticMetaObject));

//     // without a registration the fallback to identical types applies
//     EXPECT_TRUE(propH1.canConnect(propH2));
//     EXPECT_FALSE(propH1.canConnect(propI));
// }

// TEST(TestGtPropertyConversionRegistry, converterAccessors)
// {
//     const auto convert =
//         [](GtAbstractProperty const&, GtAbstractProperty&)
//             -> gt::conversion::conversionSuccess {
//             return gt::conversion::conversionSuccess::Success;
//         };

//     const auto canConnect =
//         [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
//             return true;
//         };

//     GtPropertyConverter conversionOnly(
//         GtIntProperty::staticMetaObject,
//         GtDoubleProperty::staticMetaObject, convert);

//     EXPECT_EQ(conversionOnly.fromClassName(),
//               QStringLiteral("GtIntProperty"));
//     EXPECT_EQ(conversionOnly.toClassName(),
//               QStringLiteral("GtDoubleProperty"));
//     EXPECT_TRUE(conversionOnly.canConversionDefined());
//     EXPECT_FALSE(conversionOnly.canConnectDefined());

//     const auto noFunction = conversionOnly.canConnectFunc();
//     EXPECT_FALSE(noFunction);

//     GtPropertyConverter conversionAndCanConnect(
//         GtIntProperty::staticMetaObject,
//         GtDoubleProperty::staticMetaObject, convert, canConnect);

//     EXPECT_TRUE(conversionAndCanConnect.canConversionDefined());
//     EXPECT_TRUE(conversionAndCanConnect.canConnectDefined());

//     const auto function = conversionAndCanConnect.canConnectFunc();
//     ASSERT_TRUE(function);
//     EXPECT_TRUE(function(GtIntProperty("int", "intName"),
//                         GtDoubleProperty("double", "doubleName", "doubleBrief",
//                                          GtUnit::Category::None, 4.0)));
// }
