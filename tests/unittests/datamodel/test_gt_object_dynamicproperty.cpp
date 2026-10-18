/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <QPointer>
#include <QMetaObject>
#include <QFile>
#include <QThread>

#include "gt_doubleproperty.h"
#include "gt_object.h"
#include "gt_objectio.h"
#include "gt_objectmementodiff.h"
#include "gt_dynamicpropertyfactory.h"
#include "gt_xmlexpr.h"

class TestDynamicPropertyOwner : public GtObject
{
public:
    TestDynamicPropertyOwner() : declaredProperty("declaredId", "Declared")
    {
        declaredProperty.registerSubProperty(nestedProperty);
        registerProperty(declaredProperty);
    }

private:
    GtDoubleProperty declaredProperty;
    GtDoubleProperty nestedProperty{"nestedId", "Nested"};
};

class TestCompositeDynamicProperty : public GtDoubleProperty
{
public:
    TestCompositeDynamicProperty() :
        GtDoubleProperty("runtimeRoot", "Runtime root")
    {
        registerSubProperty(nestedProperty);
    }

    GtDoubleProperty nestedProperty{"runtimeNested", "Runtime nested"};
};

class TestDuplicateIdDynamicProperty : public GtDoubleProperty
{
public:
    TestDuplicateIdDynamicProperty() : GtDoubleProperty("duplicateId", "Root")
    {
        registerSubProperty(nestedProperty);
    }

    GtDoubleProperty nestedProperty{"duplicateId", "Nested"};
};

class TestConfiguredDynamicProperty : public GtDoubleProperty
{
public:
    TestConfiguredDynamicProperty(const QString& id, const QString& name,
                                  int precision) :
        GtDoubleProperty(id, name), m_precision(precision)
    {
        setOptional(true);
    }
    int precision() const
    {
        return m_precision;
    }

private:
    int m_precision;
};

static GtDynamicPropertyFactory::Registration
registerConfiguredDynamicProperty()
{
    return GtDynamicPropertyFactory::registerType(
        QStringLiteral("org.gtlab.tests.configured-double"),
        [](const GtAbstractProperty& property) {
            return dynamic_cast<const TestConfiguredDynamicProperty*>(
                       &property) != nullptr;
        },
        [](const GtAbstractProperty& property) {
            QVariantMap definition;
            auto configured =
                dynamic_cast<const TestConfiguredDynamicProperty*>(&property);
            if (configured)
                definition.insert(QStringLiteral("precision"),
                                  configured->precision());
            return definition;
        },
        [](const QString& id, const QString& name,
           const QVariantMap& definition)
            -> std::unique_ptr<GtAbstractProperty> {
            if (!definition.contains(QStringLiteral("precision"))) return {};
            auto property = std::make_unique<TestConfiguredDynamicProperty>(
                id, name,
                definition.value(QStringLiteral("precision")).toInt());
            property->setProperty("gt.dynamicTypeId",
                                  "org.gtlab.tests.configured-double");
            return property;
        });
}

static std::unique_ptr<TestConfiguredDynamicProperty>
configuredProperty(const QString& id, const QString& name, int precision,
                   double value, bool active = true)
{
    auto property = std::make_unique<TestConfiguredDynamicProperty>(
        id, name, precision);
    property->setProperty("gt.dynamicTypeId",
                          "org.gtlab.tests.configured-double");
    property->setVal(value);
    property->setActive(active);
    return property;
}

class TestGtObjectDynamicProperty : public ::testing::Test
{
protected:
    TestDynamicPropertyOwner object;
};

TEST_F(TestGtObjectDynamicProperty, AddsAndLooksUpOwnedRuntimeProperty)
{
    auto property = std::make_unique<GtDoubleProperty>("runtimeId", "Runtime");
    GtDoubleProperty* rawProperty = property.get();
    property->setVal(42.5);

    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    EXPECT_FALSE(property);
    EXPECT_EQ(rawProperty->parent(), &object);
    EXPECT_EQ(object.findProperty("runtimeId"), rawProperty);
    EXPECT_EQ(object.findPropertyByName("Runtime"), rawProperty);
    ASSERT_EQ(object.properties().size(), 2);
    EXPECT_EQ(object.properties().last(), rawProperty);
    EXPECT_DOUBLE_EQ(rawProperty->getVal(), 42.5);
}

TEST_F(TestGtObjectDynamicProperty, FindsNestedRuntimeProperty)
{
    auto root = std::make_unique<TestCompositeDynamicProperty>();
    GtDoubleProperty* rawNested = &root->nestedProperty;

    ASSERT_TRUE(object.addDynamicProperty(std::move(root)));
    EXPECT_EQ(object.findProperty("runtimeNested"), rawNested);
    EXPECT_EQ(object.findPropertyByName("Runtime nested"), rawNested);
    ASSERT_EQ(object.properties().size(), 2);
    EXPECT_EQ(object.properties().last()->ident(), "runtimeRoot");
}

TEST_F(TestGtObjectDynamicProperty,
       MovesStaticAndOwnedRuntimePropertiesToDestinationThread)
{
    auto dynamicRoot = std::make_unique<TestCompositeDynamicProperty>();
    GtAbstractProperty* dynamicRootPtr = dynamicRoot.get();
    GtAbstractProperty* dynamicNestedPtr = &dynamicRoot->nestedProperty;
    ASSERT_TRUE(object.addDynamicProperty(std::move(dynamicRoot)));

    GtAbstractProperty* staticProperty = object.findProperty("declaredId");
    ASSERT_NE(staticProperty, nullptr);
    TestDynamicPropertyOwner* objectPtr = &object;
    QThread* sourceThread = QThread::currentThread();
    QThread destinationThread;
    destinationThread.start();

    gt::moveToThread(object, &destinationThread);
    EXPECT_EQ(object.thread(), &destinationThread);
    EXPECT_EQ(staticProperty->thread(), &destinationThread);
    EXPECT_EQ(dynamicRootPtr->thread(), &destinationThread);
    EXPECT_EQ(dynamicNestedPtr->thread(), &destinationThread);

    const bool movedBack = QMetaObject::invokeMethod(
        &object,
        [objectPtr, sourceThread]() {
            gt::moveToThread(*objectPtr, sourceThread);
        },
        Qt::BlockingQueuedConnection);
    EXPECT_TRUE(movedBack);
    EXPECT_EQ(object.thread(), sourceThread);
    EXPECT_EQ(staticProperty->thread(), sourceThread);
    EXPECT_EQ(dynamicRootPtr->thread(), sourceThread);
    EXPECT_EQ(dynamicNestedPtr->thread(), sourceThread);

    destinationThread.quit();
    destinationThread.wait();
}

TEST_F(TestGtObjectDynamicProperty,
       RejectsInvalidAndConflictingIdsWithoutTakingOwnership)
{
    std::unique_ptr<GtAbstractProperty> empty;
    EXPECT_FALSE(object.addDynamicProperty(std::move(empty)));
    EXPECT_FALSE(empty);

    auto emptyId = std::make_unique<GtDoubleProperty>(QString(), "Empty id");
    GtDoubleProperty* rawEmptyId = emptyId.get();
    EXPECT_FALSE(object.addDynamicProperty(std::move(emptyId)));
    EXPECT_EQ(emptyId.get(), rawEmptyId);
    EXPECT_EQ(rawEmptyId->parent(), nullptr);

    auto staticCollision =
        std::make_unique<GtDoubleProperty>("declaredId", "Collision");
    GtDoubleProperty* rawStaticCollision = staticCollision.get();
    EXPECT_FALSE(object.addDynamicProperty(std::move(staticCollision)));
    EXPECT_EQ(staticCollision.get(), rawStaticCollision);
    EXPECT_EQ(rawStaticCollision->parent(), nullptr);

    auto nestedCollision =
        std::make_unique<GtDoubleProperty>("nestedId", "Nested collision");
    GtDoubleProperty* rawNestedCollision = nestedCollision.get();
    EXPECT_FALSE(object.addDynamicProperty(std::move(nestedCollision)));
    EXPECT_EQ(nestedCollision.get(), rawNestedCollision);
    EXPECT_FALSE(object.objectFlags() & GtObject::HasOwnChanges);

    auto runtime = std::make_unique<GtDoubleProperty>("runtimeId", "Runtime");
    ASSERT_TRUE(object.addDynamicProperty(std::move(runtime)));
    auto runtimeCollision =
        std::make_unique<GtDoubleProperty>("runtimeId", "Collision");
    GtDoubleProperty* rawRuntimeCollision = runtimeCollision.get();
    EXPECT_FALSE(object.addDynamicProperty(std::move(runtimeCollision)));
    EXPECT_EQ(runtimeCollision.get(), rawRuntimeCollision);
    EXPECT_EQ(rawRuntimeCollision->parent(), nullptr);

    EXPECT_EQ(object.properties().size(), 2);
    EXPECT_EQ(object.findProperty("runtimeId")->objectName(), "Runtime");
}

TEST_F(TestGtObjectDynamicProperty,
       RejectsAlreadyParentedPropertyWithoutReparenting)
{
    QObject externalParent;
    auto property = std::make_unique<GtDoubleProperty>("runtimeId", "Runtime");
    GtDoubleProperty* rawProperty = property.get();
    rawProperty->setParent(&externalParent);

    EXPECT_FALSE(object.addDynamicProperty(std::move(property)));
    EXPECT_EQ(property.get(), rawProperty);
    EXPECT_EQ(rawProperty->parent(), &externalParent);
    ASSERT_EQ(object.properties().size(), 1);
    EXPECT_EQ(object.findProperty("runtimeId"), nullptr);
}

TEST_F(TestGtObjectDynamicProperty,
       RejectsDuplicateIdsWithinIncomingPropertyTree)
{
    auto property = std::make_unique<TestDuplicateIdDynamicProperty>();
    TestDuplicateIdDynamicProperty* rawProperty = property.get();

    EXPECT_FALSE(object.addDynamicProperty(std::move(property)));
    EXPECT_EQ(property.get(), rawProperty);
    EXPECT_EQ(rawProperty->parent(), nullptr);
    EXPECT_EQ(object.properties().size(), 1);
    EXPECT_EQ(object.findProperty("duplicateId"), nullptr);
    EXPECT_FALSE(object.objectFlags() & GtObject::HasOwnChanges);
}

TEST_F(TestGtObjectDynamicProperty, TakesRuntimePropertyAndReturnsOwnership)
{
    auto property = std::make_unique<GtDoubleProperty>("runtimeId", "Runtime");
    GtDoubleProperty* rawProperty = property.get();
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    auto taken = object.takeDynamicProperty("runtimeId");
    ASSERT_EQ(taken.get(), rawProperty);
    EXPECT_EQ(taken->parent(), nullptr);
    EXPECT_EQ(object.findProperty("runtimeId"), nullptr);
    ASSERT_EQ(object.properties().size(), 1);
    EXPECT_EQ(object.takeDynamicProperty("runtimeId"), nullptr);
}

TEST(GtObjectDynamicProperty, ObjectDestructionDeletesAttachedProperty)
{
    QPointer<GtAbstractProperty> guardedProperty;
    {
        GtObject object;
        auto property =
            std::make_unique<GtDoubleProperty>("runtimeId", "Runtime");
        guardedProperty = property.get();
        ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    }

    EXPECT_TRUE(guardedProperty.isNull());
}

TEST_F(TestGtObjectDynamicProperty, SerializesDefinitionSeparatelyFromValue)
{
    auto registration = registerConfiguredDynamicProperty();
    ASSERT_TRUE(registration);
    EXPECT_FALSE(GtDynamicPropertyFactory::registerType(
        QStringLiteral("org.gtlab.tests.configured-double"),
        [](const GtAbstractProperty&) { return true; },
        [](const GtAbstractProperty&) { return QVariantMap{}; },
        [](const QString&, const QString&, const QVariantMap&) {
            return std::unique_ptr<GtAbstractProperty>{};
        }));

    auto property = std::make_unique<TestConfiguredDynamicProperty>(
        QStringLiteral("configured"), QStringLiteral("Configured"), 7);
    property->setProperty("gt.dynamicTypeId",
                          "org.gtlab.tests.configured-double");
    property->setVal(12.75);
    property->setActive(false);
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    const GtObjectMemento saved = GtObjectIO().toMemento(&object);
    const GtObjectMemento loaded(saved.toByteArray());
    ASSERT_EQ(loaded.dynamicProperties.size(), 1);
    const auto& data = loaded.dynamicProperties.front();
    EXPECT_EQ(data.typeId, "org.gtlab.tests.configured-double");
    EXPECT_EQ(data.ident, "configured");
    EXPECT_EQ(data.name, "Configured");
    EXPECT_EQ(data.definition.value(QStringLiteral("precision")).toInt(), 7);
    EXPECT_DOUBLE_EQ(data.value.toDouble(), 12.75);
    EXPECT_FALSE(data.isActive);

    auto recreated = GtDynamicPropertyFactory::create(
        data.typeId, data.ident, data.name, data.definition);
    ASSERT_NE(recreated, nullptr);
    auto configured =
        dynamic_cast<TestConfiguredDynamicProperty*>(recreated.get());
    ASSERT_NE(configured, nullptr);
    EXPECT_EQ(configured->precision(), 7);
    ASSERT_TRUE(configured->setValueFromVariant(data.value, QString()));
    configured->setActive(data.isActive);
    EXPECT_DOUBLE_EQ(configured->getVal(), 12.75);
    EXPECT_FALSE(configured->isActive());
    EXPECT_FALSE(GtDynamicPropertyFactory::create(
        QStringLiteral("org.gtlab.tests.missing"), "x", "X", {}));
}

TEST_F(TestGtObjectDynamicProperty,
       AppliesAndRevertsDynamicPropertyDefinitionValueAndStateChanges)
{
    auto registration = registerConfiguredDynamicProperty();
    ASSERT_TRUE(registration);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 2, 1.5)));
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("removed", "Removed", 5, 8.0)));
    const GtObjectMemento before = GtObjectIO().toMemento(&object);

    auto oldAlpha = object.takeDynamicProperty("alpha");
    auto oldRemoved = object.takeDynamicProperty("removed");
    ASSERT_NE(oldAlpha, nullptr);
    ASSERT_NE(oldRemoved, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Renamed alpha", 4, 19.25, false)));
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("added", "Added", 1, 3.0)));
    const GtObjectMemento after = GtObjectIO().toMemento(&object);
    GtObjectMementoDiff diff(before, after);

    // Restore the before state, then exercise the reversible diff in place.
    auto newAlpha = object.takeDynamicProperty("alpha");
    auto newAdded = object.takeDynamicProperty("added");
    ASSERT_NE(newAlpha, nullptr);
    ASSERT_NE(newAdded, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 2, 1.5)));
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("removed", "Removed", 5, 8.0)));

    ASSERT_TRUE(GtObjectIO::applyDiff(diff, &object));
    ASSERT_EQ(object.dynamicProperties().size(), 2);
    auto* changed = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(changed, nullptr);
    EXPECT_EQ(changed->objectName(), "Renamed alpha");
    EXPECT_EQ(changed->precision(), 4);
    EXPECT_DOUBLE_EQ(changed->getVal(), 19.25);
    EXPECT_FALSE(changed->isActive());
    auto* added = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("added"));
    ASSERT_NE(added, nullptr);
    EXPECT_DOUBLE_EQ(added->getVal(), 3.0);
    EXPECT_EQ(object.findProperty("removed"), nullptr);

    ASSERT_TRUE(GtObjectIO::revertDiff(diff, &object));
    ASSERT_EQ(object.dynamicProperties().size(), 2);
    auto* reverted = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(reverted, nullptr);
    EXPECT_EQ(reverted->objectName(), "Alpha");
    EXPECT_EQ(reverted->precision(), 2);
    EXPECT_DOUBLE_EQ(reverted->getVal(), 1.5);
    EXPECT_TRUE(reverted->isActive());
    EXPECT_NE(object.findProperty("removed"), nullptr);
    EXPECT_EQ(object.findProperty("added"), nullptr);
}

TEST_F(TestGtObjectDynamicProperty,
       RejectsInvalidDynamicPropertyBatchWithoutPartialMutation)
{
    auto registration = registerConfiguredDynamicProperty();
    ASSERT_TRUE(registration);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 2, 1.5)));
    const GtObjectMemento before = GtObjectIO().toMemento(&object);

    auto oldAlpha = object.takeDynamicProperty("alpha");
    ASSERT_NE(oldAlpha, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 4, 19.25, false)));
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("added", "Added", 1, 3.0)));
    const GtObjectMemento after = GtObjectIO().toMemento(&object);
    GtObjectMementoDiff generated(before, after);

    QDomDocument malformed = generated.doc();
    QDomElement addedRecord = malformed.documentElement()
        .firstChildElement(gt::xml::S_DIFF_DYNAMIC_PROPERTIES_TAG)
        .lastChildElement()
        .firstChildElement(gt::xml::S_DIFF_NEWVAL_TAG)
        .firstChildElement(QStringLiteral("object"))
        .firstChildElement(QStringLiteral("dynamicProperties"))
        .firstChildElement(QStringLiteral("property"));
    ASSERT_FALSE(addedRecord.isNull());
    addedRecord.setAttribute(QStringLiteral("typeId"),
                             QStringLiteral("org.gtlab.tests.missing"));
    GtObjectMementoDiff invalidDiff(malformed.toByteArray());

    // The target is in the old state; the first operation is valid, while the
    // later add refers to an unregistered type.
    auto newAlpha = object.takeDynamicProperty("alpha");
    auto newAdded = object.takeDynamicProperty("added");
    ASSERT_NE(newAlpha, nullptr);
    ASSERT_NE(newAdded, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 2, 1.5)));

    EXPECT_FALSE(GtObjectIO::applyDiff(invalidDiff, &object));
    ASSERT_EQ(object.dynamicProperties().size(), 1);
    auto* unchanged = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(unchanged, nullptr);
    EXPECT_EQ(unchanged->objectName(), "Alpha");
    EXPECT_EQ(unchanged->precision(), 2);
    EXPECT_DOUBLE_EQ(unchanged->getVal(), 1.5);
    EXPECT_TRUE(unchanged->isActive());
    EXPECT_EQ(object.findProperty("added"), nullptr);
}

TEST(GtObjectDynamicProperty, ReadsLegacyXmlWithoutDynamicProperties)
{
    QFile file(QStringLiteral(":/testdata/legacy-static-object.xml"));
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    const GtObjectMemento loaded(file.readAll());
    EXPECT_EQ(loaded.className(), "LegacyOwner");
    EXPECT_EQ(loaded.uuid(), "legacy-static-object");
    EXPECT_EQ(loaded.ident(), "Legacy");
    EXPECT_TRUE(loaded.dynamicPropertiesValid);
    EXPECT_TRUE(loaded.dynamicProperties.isEmpty());
    const auto* value = GtObjectMemento::findPropertyByName(
        loaded.properties, QStringLiteral("doubleProp"));
    ASSERT_NE(value, nullptr);
    EXPECT_DOUBLE_EQ(value->data().toDouble(), 106.2);
    const auto* text = GtObjectMemento::findPropertyByName(
        loaded.properties, QStringLiteral("stringProp"));
    ASSERT_NE(text, nullptr);
    EXPECT_EQ(text->data().toString(), "saved before runtime properties");
}

TEST(GtObjectDynamicProperty, RejectsUnsupportedDynamicXmlVersion)
{
    const QByteArray xml = R"(<object class="Owner" uuid="id" name="Owner">
        <dynamicProperties formatVersion="9"/>
        </object>)";
    const GtObjectMemento loaded(xml);
    EXPECT_FALSE(loaded.dynamicPropertiesValid);
    EXPECT_TRUE(loaded.dynamicProperties.isEmpty());
}
