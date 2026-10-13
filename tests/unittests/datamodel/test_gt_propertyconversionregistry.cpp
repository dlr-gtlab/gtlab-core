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

#include "test_gt_object.h"
#include "test_gt_propertyconversionregistry.h"
#include "gt_boolproperty.h"
#include "gt_doubleproperty.h"
#include "gt_intproperty.h"
#include "gt_propertyconnection.h"
#include "gt_propertyconversionregistry.h"
#include "gt_stringproperty.h"

TEST(TestGtPropertyConversionRegistry, singletonInstance)
{
    EXPECT_EQ(&gtPropConversion(),
              &GtPropertyConversionRegistry::getInstance());
}

TEST(TestGtPropertyConversionRegistry, builtInConversionRegistration)
{
    GtIntProperty intProp("int", "intName", QString());
    GtDoubleProperty doubleProp("double", "doubleName", "doubleBrief",
                                GtUnit::Category::None, 4.0);

    // the registry is initialized with built in converters for both
    // double <-> int directions
    EXPECT_TRUE(gtPropConversion().convertFunction(
        GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject));
    EXPECT_TRUE(gtPropConversion().convertFunction(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject));
    EXPECT_TRUE(gtPropConversion().canConnectFunction(
        GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject));
    EXPECT_TRUE(gtPropConversion().canConnectFunction(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject));

    EXPECT_TRUE(doubleProp.canConnect(intProp));
    EXPECT_TRUE(intProp.canConnect(doubleProp));

    EXPECT_TRUE(gtPropConversion()
                    .converterAvailable("GtDoubleProperty")
                    .contains("GtIntProperty"));
}

TEST(TestGtPropertyConversionRegistry, canConnectFunctionUnregistered)
{
    // no function is registered for this property type pair
    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject));
    EXPECT_FALSE(gtPropConversion().convertFunction(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject));
}

TEST(TestGtPropertyConversionRegistry, canConnectWithoutRegisteredFunction)
{
    GtStringProperty stringProp1("string1", "stringProp1");
    GtStringProperty stringProp2("string2", "stringProp2");
    GtIntProperty intProp("int", "intProp");

    // for identical types without a registered function the connection
    // is accepted by the fallback
    EXPECT_TRUE(stringProp1.canConnect(stringProp2));

    // for a property type pair without a registered function and
    // different types the connection is rejected
    EXPECT_FALSE(stringProp1.canConnect(intProp));
    EXPECT_FALSE(intProp.canConnect(stringProp1));
}

TEST(TestGtPropertyConversionRegistry, registerCanConnect)
{
    GtTestRegistryPropF propF;
    propF.setObjectName("testRegistryF");
    GtTestRegistryPropG propG;
    propG.setObjectName("testRegistryG");

    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropF::staticMetaObject,
        GtTestRegistryPropG::staticMetaObject,
        [](GtAbstractProperty const& a, GtAbstractProperty const&) -> bool {
            return a.objectName() == "testRegistryF";
        });

    const auto function = gtPropConversion().canConnectFunction(
        GtTestRegistryPropF::staticMetaObject,
        GtTestRegistryPropG::staticMetaObject);

    // the registered function is returned and behaves as registered
    ASSERT_TRUE(function);
    EXPECT_TRUE(function(propF, propG));
    EXPECT_TRUE(propF.canConnect(propG));

    GtTestRegistryPropF otherPropF;
    otherPropF.setObjectName("otherTestRegistryF");
    EXPECT_FALSE(function(otherPropF, propG));
    EXPECT_FALSE(otherPropF.canConnect(propG));

    // the lookup is directional
    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropG::staticMetaObject,
        GtTestRegistryPropF::staticMetaObject));
}

TEST(TestGtPropertyConversionRegistry, canConnectWithRegisteredFunction)
{
    GtTestRegistryPropG propG1;
    GtTestRegistryPropG propG2;

    // a registered canConnect function overrides the fallback to
    // identical property types
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropG::staticMetaObject,
        GtTestRegistryPropG::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    EXPECT_FALSE(propG1.canConnect(propG2));
}

TEST(TestGtPropertyConversionRegistry, registrationDeduplication)
{
    GtTestRegistryPropH propH;
    GtTestRegistryPropI propI;

    // if multiple functions are registered for the same property type
    // pair the first registered function decides the result
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropH::staticMetaObject,
        GtTestRegistryPropI::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return true;
        });
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropH::staticMetaObject,
        GtTestRegistryPropI::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    const auto function = gtPropConversion().canConnectFunction(
        GtTestRegistryPropH::staticMetaObject,
        GtTestRegistryPropI::staticMetaObject);

    ASSERT_TRUE(function);
    EXPECT_TRUE(function(propH, propI));
    EXPECT_TRUE(propH.canConnect(propI));

    // the type pair is listed exactly once
    const auto available =
        gtPropConversion().converterAvailable("GtTestRegistryPropH");
    EXPECT_EQ(available.count("GtTestRegistryPropI"), 1);

    // the reversed direction is not affected
    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropI::staticMetaObject,
        GtTestRegistryPropH::staticMetaObject));
    EXPECT_FALSE(propI.canConnect(propH));
}

TEST(TestGtPropertyConversionRegistry, emptyFunctionRegistrationIgnored)
{
    GtTestRegistryPropI propI1;
    GtTestRegistryPropI propI2;
    GtTestRegistryPropH propH;

    // registering empty functions is ignored
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropI::staticMetaObject,
        GtTestRegistryPropH::staticMetaObject, gt::conversion::convert{});

    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropI::staticMetaObject,
        GtTestRegistryPropH::staticMetaObject, gt::conversion::canConnect{});

    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropI::staticMetaObject,
        GtTestRegistryPropH::staticMetaObject));
    EXPECT_FALSE(gtPropConversion().convertFunction(
        GtTestRegistryPropI::staticMetaObject,
        GtTestRegistryPropH::staticMetaObject));
    EXPECT_FALSE(gtPropConversion()
                     .converterAvailable("GtTestRegistryPropI")
                     .contains("GtTestRegistryPropH"));

    // without a registration the fallback to identical types applies
    EXPECT_TRUE(propI1.canConnect(propI2));
    EXPECT_FALSE(propI1.canConnect(propH));
}

TEST(TestGtPropertyConversionRegistry, inheritanceLookup)
{
    GtTestRegistryPropA propA;
    propA.setObjectName("testRegistryA");
    GtTestRegistryPropB propB;
    propB.setObjectName("testRegistryB");

    // a registration for the derived from type and the base to type
    // is found via the inheritance chains
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropA::staticMetaObject,
        GtAbstractProperty::staticMetaObject,
        [](GtAbstractProperty const& a, GtAbstractProperty const&) -> bool {
            return a.objectName() == "testRegistryA";
        });

    const auto function = gtPropConversion().canConnectFunction(
        GtTestRegistryPropA::staticMetaObject,
        GtTestRegistryPropB::staticMetaObject);

    ASSERT_TRUE(function);
    EXPECT_TRUE(function(propA, propB));

    GtTestRegistryPropA otherPropA;
    otherPropA.setObjectName("otherTestRegistryA");
    EXPECT_FALSE(function(otherPropA, propB));

    // the property level check benefits from the same registration
    EXPECT_TRUE(propA.canConnect(propB));

    // the lookup is directional, a base to type registration does not
    // extend the from type
    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropB::staticMetaObject,
        GtTestRegistryPropA::staticMetaObject));
    EXPECT_FALSE(propB.canConnect(propA));

    EXPECT_NE(gtPropConversion().findConverterWithInheritance(
                  GtTestRegistryPropA::staticMetaObject,
                  GtTestRegistryPropB::staticMetaObject),
              nullptr);
    EXPECT_EQ(gtPropConversion().findConverterWithInheritance(
                  GtTestRegistryPropB::staticMetaObject,
                  GtTestRegistryPropA::staticMetaObject),
              nullptr);
}


TEST(TestGtPropertyConversionRegistry, inheritanceLookupPriority)
{
    GtTestRegistryPropC propC;
    GtTestRegistryPropD propD;
    GtTestRegistryPropE propE;

    // Source-side priority:
    // D -> AbstractProperty competes with AbstractProperty -> E.
    // The more specific source type D must win.
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropD::staticMetaObject,
        GtAbstractProperty::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return false;
        });

    gtPropConversion().registerConnectionCompatibility(
        GtAbstractProperty::staticMetaObject,
        GtTestRegistryPropE::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return true;
        });

    const auto functionDE = gtPropConversion().canConnectFunction(
        GtTestRegistryPropD::staticMetaObject,
        GtTestRegistryPropE::staticMetaObject);

    ASSERT_TRUE(functionDE);
    EXPECT_FALSE(functionDE(propD, propE));

    // Base-type fallback still works for C -> E.
    const auto functionCE = gtPropConversion().canConnectFunction(
        GtTestRegistryPropC::staticMetaObject,
        GtTestRegistryPropE::staticMetaObject);

    ASSERT_TRUE(functionCE);
    EXPECT_TRUE(functionCE(propC, propE));

    // No registration matches the pair (C, D).
    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropC::staticMetaObject,
        GtTestRegistryPropD::staticMetaObject));

    // Target-side priority:
    // Both registrations now have the same source type D.
    // The exact target type E must win over AbstractProperty,
    // even though the less specific target was registered first.
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropD::staticMetaObject,
        GtTestRegistryPropE::staticMetaObject,
        [](GtAbstractProperty const&, GtAbstractProperty const&) -> bool {
            return true;
        });

    const auto specificTargetFunction =
        gtPropConversion().canConnectFunction(
            GtTestRegistryPropD::staticMetaObject,
            GtTestRegistryPropE::staticMetaObject);

    ASSERT_TRUE(specificTargetFunction);
    EXPECT_TRUE(specificTargetFunction(propD, propE));

    // The selected converter must also be the one returned by lookup.
    const auto* converter =
        gtPropConversion().findConverterWithInheritance(
            GtTestRegistryPropD::staticMetaObject,
            GtTestRegistryPropE::staticMetaObject);

    ASSERT_NE(converter, nullptr);
    EXPECT_EQ(
        converter->fromClassName(),
        QString::fromLatin1(
            GtTestRegistryPropD::staticMetaObject.className()));
    EXPECT_EQ(
        converter->toClassName(),
        QString::fromLatin1(
            GtTestRegistryPropE::staticMetaObject.className()));
}


TEST(TestGtPropertyConversionRegistry, convertOnlyRegistration)
{
    GtTestRegistryPropC propC;
    GtTestRegistryPropD propD;

    // a converter registered without a canConnect function does not
    // enable a connection for the property type pair
    gtPropConversion().registerConnectionCompatibility(
        GtTestRegistryPropC::staticMetaObject,
        GtTestRegistryPropD::staticMetaObject,
        [](GtAbstractProperty const&,
           GtAbstractProperty&) -> gt::conversion::conversionSuccess {
            return gt::conversion::conversionSuccess::Success;
        });

    const auto conversion = gtPropConversion().convertFunction(
        GtTestRegistryPropC::staticMetaObject,
        GtTestRegistryPropD::staticMetaObject);

    ASSERT_TRUE(conversion);
    EXPECT_EQ(conversion(propC, propD),
              gt::conversion::conversionSuccess::Success);

    EXPECT_FALSE(gtPropConversion().canConnectFunction(
        GtTestRegistryPropC::staticMetaObject,
        GtTestRegistryPropD::staticMetaObject));

    EXPECT_FALSE(propC.canConnect(propD));
    EXPECT_FALSE(propD.canConnect(propC));
}

TEST(TestGtPropertyConversionRegistry, convertFunctionBuiltInValues)
{
    GtDoubleProperty doubleProp("double", "doubleName", "doubleBrief",
                                GtUnit::Category::None, 4.5);
    GtIntProperty intProp("int", "intName", QString(), 0);

    const auto conversion = gtPropConversion().convertFunction(
        GtDoubleProperty::staticMetaObject, GtIntProperty::staticMetaObject);
    ASSERT_TRUE(conversion);

    // rounding with information loss
    EXPECT_EQ(conversion(doubleProp, intProp),
              gt::conversion::conversionSuccess::Lossy);
    EXPECT_EQ(intProp.getVal(), 5);

    // exact conversion without loss
    doubleProp.setVal(4.0);
    EXPECT_EQ(conversion(doubleProp, intProp),
              gt::conversion::conversionSuccess::Success);
    EXPECT_EQ(intProp.getVal(), 4);

    GtIntProperty intSource("intSource", "intSourceName", QString(), 7);
    GtDoubleProperty doubleTarget("doubleTarget", "doubleTargetName",
                                  "doubleTargetBrief", GtUnit::Category::None,
                                  0.0);

    const auto intToDouble = gtPropConversion().convertFunction(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject);
    ASSERT_TRUE(intToDouble);
    EXPECT_EQ(intToDouble(intSource, doubleTarget),
              gt::conversion::conversionSuccess::Success);
    EXPECT_DOUBLE_EQ(doubleTarget.getVal(), 7.0);

    // no conversion function is registered in this direction
    EXPECT_FALSE(gtPropConversion().convertFunction(
        GtIntProperty::staticMetaObject, GtBoolProperty::staticMetaObject));
}

TEST(TestGtPropertyConversionRegistry, converterAccessors)
{
    const auto convert =
        [](GtAbstractProperty const&,
           GtAbstractProperty&) -> gt::conversion::conversionSuccess {
        return gt::conversion::conversionSuccess::Success;
    };

    const auto canConnect = [](GtAbstractProperty const&,
                               GtAbstractProperty const&) -> bool {
        return true;
    };

    GtPropertyConverter conversionOnly(GtIntProperty::staticMetaObject,
                                       GtDoubleProperty::staticMetaObject,
                                       convert);

    EXPECT_EQ(conversionOnly.fromClassName(), "GtIntProperty");
    EXPECT_EQ(conversionOnly.toClassName(), "GtDoubleProperty");
    EXPECT_TRUE(conversionOnly.conversionDefined());
    EXPECT_FALSE(conversionOnly.canConnectDefined());
    EXPECT_FALSE(conversionOnly.canConnectFunc());

    GtPropertyConverter convertInvalid(GtIntProperty::staticMetaObject,
                                       GtDoubleProperty::staticMetaObject, {},
                                       canConnect);

    EXPECT_FALSE(convertInvalid.conversionDefined());

    GtPropertyConverter conversionAndCanConnect(
        GtIntProperty::staticMetaObject, GtDoubleProperty::staticMetaObject,
        convert, canConnect);

    EXPECT_TRUE(conversionAndCanConnect.conversionDefined());
    EXPECT_TRUE(conversionAndCanConnect.canConnectDefined());

    const auto function = conversionAndCanConnect.canConnectFunc();
    ASSERT_TRUE(function);
    EXPECT_TRUE(function(GtIntProperty("int", "intName"),
                         GtDoubleProperty("double", "doubleName", "doubleBrief",
                                          GtUnit::Category::None, 4.0)));

    const auto convertFunc = conversionOnly.convertFunc();
    ASSERT_TRUE(convertFunc);
    GtIntProperty intProp("int", "intName");
    GtDoubleProperty doubleProp("double", "doubleName", "doubleBrief",
                                GtUnit::Category::None, 0.0);
    EXPECT_EQ(convertFunc(intProp, doubleProp),
              gt::conversion::conversionSuccess::Success);
}

TEST(TestGtPropertyConversionRegistry, setValFromConnectionEndToEnd)
{
    auto parent = std::make_unique<TestSpecialGtObject>();

    auto child1 = new TestSpecialGtObject;
    child1->setUuid("child1");
    parent->appendChild(child1);

    auto child2 = new TestSpecialGtObject;
    child2->setUuid("child2");
    parent->appendChild(child2);

    auto connection = new GtPropertyConnection();
    connection->setSourceUuid("child1");
    connection->setTargetUuid("child2");
    connection->setSourceProp("intProp");
    connection->setTargetProp("doubleProp");
    parent->appendChild(connection);

    auto source =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(source != nullptr);
    auto target =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child2"));
    ASSERT_TRUE(target != nullptr);

    source->setInt(7);

    connection->makeConnection();
    EXPECT_TRUE(connection->isConnected());

    // the initial transfer uses the built in int -> double converter
    EXPECT_DOUBLE_EQ(target->getDouble(), 7.0);

    // value changes of the source are forwarded to the target
    source->setInt(9);
    EXPECT_DOUBLE_EQ(target->getDouble(), 9.0);

    // the connection resolves the source property
    EXPECT_EQ(connection->sourcePropObject(),
              gt::getConnectedProperty(*connection));
    EXPECT_EQ(gt::getConnectedValue(*connection).toInt(), 9);
}

TEST(TestGtPropertyConversionRegistry, setValFromConnectionEndToEndFailure)
{
    auto parent = std::make_unique<TestSpecialGtObject>();

    auto child1 = new TestSpecialGtObject;
    child1->setUuid("child1");
    parent->appendChild(child1);

    auto child2 = new TestSpecialGtObject;
    child2->setUuid("child2");
    parent->appendChild(child2);

    auto connection = new GtPropertyConnection();
    connection->setSourceUuid("child1");
    connection->setTargetUuid("child2");
    connection->setSourceProp("doubleProp");
    connection->setTargetProp("intProp");
    parent->appendChild(connection);

    auto source =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child1"));
    ASSERT_TRUE(source != nullptr);
    auto target =
        qobject_cast<TestSpecialGtObject*>(parent->getObjectByUuid("child2"));
    ASSERT_TRUE(target != nullptr);

    source->setDouble(7);

    connection->makeConnection();
    EXPECT_TRUE(connection->isConnected());


    //
    QVector<QString> log;

    auto dest = gt::log::makeFunctorDestination(
        [this, &log](std::string const& msg, gt::log::Level level,
                     gt::log::Details details) {
            log.append(QStringLiteral("%1 [%2] [%3] %4")
                           .arg(level)
                           .arg(details.id.c_str(),
                                gt::log::formatTime(details.time).c_str(),
                                msg.c_str()));
        });
    bool success =
        gt::log::Logger::instance().addDestination("test", std::move(dest));
    auto removeLogDest = gt::finally(
        []() { gt::log::Logger::instance().removeDestination("test"); });

    QString expectedWarningText = QObject::tr("Conversion of doubleProp to "
                                              "intProp indicated some "
                                              "infomation loss.");

    //  Expect to see no gtWarning
    auto iter = std::find_if(log.begin(), log.end(), [=](QString const& entry) {
        return entry.contains(expectedWarningText);
    });

    // the initial transfer uses the built in int -> double converter
    EXPECT_EQ(target->getInt(), 7);

    ASSERT_EQ(iter, log.end());

    ASSERT_TRUE(success);

    // value changes of the source are forwarded to the target
    source->setDouble(7.7);

    // the connection resolves the source property
    EXPECT_EQ(connection->sourcePropObject(),
              gt::getConnectedProperty(*connection));
    EXPECT_EQ(gt::getConnectedValue(*connection).toInt(), 8);

    //  Expect to see a gtWarning
    auto iter2 =
        std::find_if(log.begin(), log.end(), [=](QString const& entry) {
            return entry.contains(expectedWarningText);
        });

    ASSERT_NE(iter, log.end());
}
