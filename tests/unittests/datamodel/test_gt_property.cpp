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

TEST(TestGtProperty, categoryString)
{
    auto factory = gt::makeBoolProperty(true);
    std::unique_ptr<GtAbstractProperty> boolProp(factory("bla"));

    /// check default category
    EXPECT_STREQ(boolProp->categoryString().toStdString().c_str(), "Main");

    EXPECT_TRUE(boolProp->category() ==
                GtAbstractProperty::PropertyCategory::Main);

    boolProp->setCategory("MyFancyCategory");

    /// check identical result in deprecated function
    EXPECT_STREQ(boolProp->categoryString().toStdString().c_str(),
                 "MyFancyCategory");

    EXPECT_TRUE(boolProp->category() ==
                GtAbstractProperty::PropertyCategory::Custom);
}

TEST(TestGtProperty, propertyConnectionEnabled)
{
    GtIntProperty intProp("int", "intName");
    GtDoubleProperty doubleProp("double", "doubleName", "doubleBrief",
                                GtUnit::Category::None, 4.0);
    GtStringProperty stringProp("string", "stringName");
    GtBoolProperty boolProp("bool", "boolName");

    EXPECT_TRUE(intProp.propertyConnectionEnabled());
    EXPECT_TRUE(doubleProp.propertyConnectionEnabled());
    EXPECT_TRUE(stringProp.propertyConnectionEnabled());
    EXPECT_TRUE(boolProp.propertyConnectionEnabled());
}

TEST(TestGtProperty, setPropertyConnectionEnabled)
{
    GtIntProperty prop("int", "intName");

    ASSERT_TRUE(prop.propertyConnectionEnabled());

    prop.setPropertyConnectionEnabled(false);
    EXPECT_FALSE(prop.propertyConnectionEnabled());

    prop.setPropertyConnectionEnabled();
    EXPECT_TRUE(prop.propertyConnectionEnabled());
}

TEST(TestGtProperty, canConnectFunctionsUnregistered)
{
    GtIntProperty intProp("int", "intName");

    // no canConnect function is registered for this property type pair
    const auto functions = intProp.canConnectFunctions(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject);

    EXPECT_TRUE(functions.isEmpty());
}

TEST(TestGtProperty, registerCanConnect)
{
    GtBoolProperty boolProp("bool", "boolFrom");
    GtIntProperty intProp("int", "intTo");

    GtAbstractProperty::registerCanConnect(
        GtBoolProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const& a, GtAbstractProperty const& b) -> bool {
            return a.objectName() == "boolFrom" && b.objectName() == "intTo";
        });

    const auto functions = boolProp.canConnectFunctions(
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
    const auto reversed = boolProp.canConnectFunctions(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject);
    EXPECT_TRUE(reversed.isEmpty());
}

TEST(TestGtProperty, canConnectWithoutRegisteredFunction)
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

TEST(TestGtProperty, canConnectWithRegisteredFunction)
{
    GtBoolProperty boolProp("bool", "boolProp");
    GtBoolProperty otherBoolProp("bool2", "otherBoolProp");
    GtStringProperty stringProp("string", "stringProp");
    GtIntProperty intProp("int", "intProp");

    // a registered canConnect function overrides the fallback to
    // identical property types
    GtAbstractProperty::registerCanConnect(
        GtBoolProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_FALSE(boolProp.canConnect(otherBoolProp));

    // a registered function allows a connection for a property type
    // pair that would be rejected by the fallback
    GtAbstractProperty::registerCanConnect(
        GtIntProperty::staticMetaObject, GtStringProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return true;
        });

    EXPECT_TRUE(intProp.canConnect(stringProp));

    // the connection direction used at registration is respected
    EXPECT_FALSE(stringProp.canConnect(intProp));

    // if multiple functions are registered for a property type pair a
    // single function accepting the connection is sufficient
    GtAbstractProperty::registerCanConnect(
        GtStringProperty::staticMetaObject, GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_TRUE(intProp.canConnect(stringProp));

    // if all registered functions reject the connection the
    // connection is rejected
    GtAbstractProperty::registerCanConnect(
        GtStringProperty::staticMetaObject, GtBoolProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_FALSE(boolProp.canConnect(stringProp));
}
