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
#include "gt_objectfactory.h"
#include "gt_objectio.h"
#include "gt_objectmementodiff.h"
#include "gt_dynamicpropertyfactory.h"
#include "gt_propertymodel.h"
#include "gt_structproperty.h"
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
        GtDoubleProperty(id, name),
        nestedProperty(id + QStringLiteral("_nested"),
                       QStringLiteral("Nested")),
        m_precision(precision)
    {
        setOptional(true);
        nestedProperty.setOptional(true);
        registerSubProperty(nestedProperty);
    }
    int precision() const
    {
        return m_precision;
    }

public:
    GtDoubleProperty nestedProperty;

private:
    int m_precision;
};

static gt::PolyVector<GtAbstractProperty>
makePointMembers()
{
    gt::PolyVector<GtAbstractProperty> members;
    auto x = std::make_unique<GtDoubleProperty>("x", "X");
    x->setOptional(true);
    members.push_back(std::move(x));
    auto y = std::make_unique<GtDoubleProperty>("y", "Y");
    y->setOptional(true);
    members.push_back(std::move(y));
    return members;
}

class TestStructuredDynamicProperty : public GtDoubleProperty
{
public:
    TestStructuredDynamicProperty(const QString& id, const QString& name) :
        GtDoubleProperty(id, name),
        position("position", "Point2d", makePointMembers())
    {
        registerSubProperty(position);
    }

    GtPropertyStructInstance position;
};

static GtDynamicPropertyFactory::Registration
registerStructuredDynamicProperty()
{
    return GtDynamicPropertyFactory::registerType(
        QStringLiteral("org.gtlab.tests.structured-double"),
        [](const GtAbstractProperty& property) {
            return dynamic_cast<const TestStructuredDynamicProperty*>(
                       &property) != nullptr;
        },
        [](const GtAbstractProperty&) { return QVariantMap{}; },
        [](const QString& id, const QString& name, const QVariantMap&)
            -> std::unique_ptr<GtAbstractProperty> {
            auto property = std::make_unique<TestStructuredDynamicProperty>(
                id, name);
            property->setProperty("gt.dynamicTypeId",
                                  "org.gtlab.tests.structured-double");
            return property;
        });
}

static bool
modelContainsText(const QAbstractItemModel& model,
                  const QModelIndex& parent,
                  const QString& text)
{
    for (int row = 0; row < model.rowCount(parent); ++row)
    {
        const QModelIndex index = model.index(row, 0, parent);
        if (model.data(index, Qt::DisplayRole).toString() == text ||
            modelContainsText(model, index, text)) return true;
    }
    return false;
}

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
    property->nestedProperty.setVal(54.5);
    property->nestedProperty.setActive(false);
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
    ASSERT_EQ(data.nestedProperties.size(), 1);
    EXPECT_EQ(data.nestedProperties.front().name, "configured_nested");
    EXPECT_DOUBLE_EQ(data.nestedProperties.front().data().toDouble(), 54.5);
    EXPECT_FALSE(data.nestedProperties.front().isActive);

    TestDynamicPropertyOwner restored;
    ASSERT_TRUE(loaded.mergeTo(restored, *GtObjectFactory::instance()));
    auto* restoredProperty = dynamic_cast<TestConfiguredDynamicProperty*>(
        restored.findProperty("configured"));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_DOUBLE_EQ(restoredProperty->getVal(), 12.75);
    EXPECT_FALSE(restoredProperty->isActive());
    EXPECT_DOUBLE_EQ(restoredProperty->nestedProperty.getVal(), 54.5);
    EXPECT_FALSE(restoredProperty->nestedProperty.isActive());

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
    auto* initialAlpha = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(initialAlpha, nullptr);
    initialAlpha->nestedProperty.setVal(2.25);
    initialAlpha->nestedProperty.setActive(false);
    const GtObjectMemento before = GtObjectIO().toMemento(&object);

    auto oldAlpha = object.takeDynamicProperty("alpha");
    auto oldRemoved = object.takeDynamicProperty("removed");
    ASSERT_NE(oldAlpha, nullptr);
    ASSERT_NE(oldRemoved, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Renamed alpha", 4, 19.25, false)));
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("added", "Added", 1, 3.0)));
    auto* changedAlpha = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(changedAlpha, nullptr);
    changedAlpha->nestedProperty.setVal(19.75);
    changedAlpha->nestedProperty.setActive(true);
    auto* addedProperty = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("added"));
    ASSERT_NE(addedProperty, nullptr);
    addedProperty->nestedProperty.setVal(3.25);
    addedProperty->nestedProperty.setActive(false);
    const GtObjectMemento after = GtObjectIO().toMemento(&object);
    GtObjectMementoDiff diff(before, after);

    // Restore the before state, then exercise the reversible diff in place.
    auto newAlpha = object.takeDynamicProperty("alpha");
    auto newAdded = object.takeDynamicProperty("added");
    ASSERT_NE(newAlpha, nullptr);
    ASSERT_NE(newAdded, nullptr);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("alpha", "Alpha", 2, 1.5)));
    auto* restoredAlpha = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(restoredAlpha, nullptr);
    restoredAlpha->nestedProperty.setVal(2.25);
    restoredAlpha->nestedProperty.setActive(false);
    ASSERT_TRUE(object.addDynamicProperty(
        configuredProperty("removed", "Removed", 5, 8.0)));

    GtPropertyModel model(&object);
    model.setObject(&object);
    EXPECT_TRUE(modelContainsText(model, QModelIndex(), QStringLiteral("Alpha")));
    EXPECT_TRUE(modelContainsText(model, QModelIndex(),
                                 QStringLiteral("Removed")));

    ASSERT_TRUE(GtObjectIO::applyDiff(diff, &object));
    ASSERT_EQ(object.dynamicProperties().size(), 2);
    auto* changed = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(changed, nullptr);
    EXPECT_EQ(changed->objectName(), "Renamed alpha");
    EXPECT_EQ(changed->precision(), 4);
    EXPECT_DOUBLE_EQ(changed->getVal(), 19.25);
    EXPECT_FALSE(changed->isActive());
    EXPECT_DOUBLE_EQ(changed->nestedProperty.getVal(), 19.75);
    EXPECT_TRUE(changed->nestedProperty.isActive());
    auto* added = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("added"));
    ASSERT_NE(added, nullptr);
    EXPECT_DOUBLE_EQ(added->getVal(), 3.0);
    EXPECT_DOUBLE_EQ(added->nestedProperty.getVal(), 3.25);
    EXPECT_FALSE(added->nestedProperty.isActive());
    EXPECT_EQ(object.findProperty("removed"), nullptr);
    EXPECT_TRUE(modelContainsText(model, QModelIndex(),
                                 QStringLiteral("Renamed alpha")));
    EXPECT_TRUE(modelContainsText(model, QModelIndex(), QStringLiteral("Added")));
    EXPECT_FALSE(modelContainsText(model, QModelIndex(), QStringLiteral("Alpha")));
    EXPECT_FALSE(modelContainsText(model, QModelIndex(),
                                  QStringLiteral("Removed")));

    ASSERT_TRUE(GtObjectIO::revertDiff(diff, &object));
    ASSERT_EQ(object.dynamicProperties().size(), 2);
    auto* reverted = dynamic_cast<TestConfiguredDynamicProperty*>(
        object.findProperty("alpha"));
    ASSERT_NE(reverted, nullptr);
    EXPECT_EQ(reverted->objectName(), "Alpha");
    EXPECT_EQ(reverted->precision(), 2);
    EXPECT_DOUBLE_EQ(reverted->getVal(), 1.5);
    EXPECT_TRUE(reverted->isActive());
    EXPECT_DOUBLE_EQ(reverted->nestedProperty.getVal(), 2.25);
    EXPECT_FALSE(reverted->nestedProperty.isActive());
    EXPECT_NE(object.findProperty("removed"), nullptr);
    EXPECT_EQ(object.findProperty("added"), nullptr);
    EXPECT_TRUE(modelContainsText(model, QModelIndex(), QStringLiteral("Alpha")));
    EXPECT_TRUE(modelContainsText(model, QModelIndex(),
                                 QStringLiteral("Removed")));
    EXPECT_FALSE(modelContainsText(model, QModelIndex(),
                                  QStringLiteral("Renamed alpha")));
    EXPECT_FALSE(modelContainsText(model, QModelIndex(), QStringLiteral("Added")));
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

TEST(GtObjectDynamicProperty,
     RetainsDynamicRecordsWhenTheirTypeCannotBeReconstructed)
{
    GtObjectMemento saved;
    {
        auto registration = registerConfiguredDynamicProperty();
        ASSERT_TRUE(registration);
        TestDynamicPropertyOwner source;
        auto property = configuredProperty("unavailable", "Unavailable",
                                            6, 42.5, false);
        property->nestedProperty.setVal(13.75);
        property->nestedProperty.setActive(false);
        ASSERT_TRUE(source.addDynamicProperty(std::move(property)));
        auto child = std::make_unique<GtObject>(&source);
        child->setObjectName(QStringLiteral("saved-child"));
        saved = GtObjectIO().toMemento(&source);
    }

    TestDynamicPropertyOwner restored;
    EXPECT_FALSE(saved.mergeTo(restored, *GtObjectFactory::instance()));
    EXPECT_TRUE(restored.isDummy());
    const GtObjectMemento resaved = GtObjectIO().toMemento(&restored);
    ASSERT_EQ(resaved.dynamicProperties.size(), 1);
    EXPECT_EQ(resaved.dynamicProperties.front().ident, "unavailable");
    EXPECT_DOUBLE_EQ(resaved.dynamicProperties.front().value.toDouble(), 42.5);
    ASSERT_EQ(resaved.dynamicProperties.front().nestedProperties.size(), 1);
    EXPECT_DOUBLE_EQ(resaved.dynamicProperties.front()
                         .nestedProperties.front().data().toDouble(), 13.75);
    ASSERT_EQ(resaved.childObjects.size(), 1);
    EXPECT_EQ(resaved.childObjects.front().ident(), "saved-child");
    auto* restoredChild = restored.findDirectChild<GtObject*>("saved-child");
    ASSERT_NE(restoredChild, nullptr);
    EXPECT_TRUE(restoredChild->isDummy());
}

TEST(GtObjectDynamicProperty,
     ReadsAndAppliesNestedStructStateInDynamicPropertyXml)
{
    auto registration = registerStructuredDynamicProperty();
    ASSERT_TRUE(registration);
    TestDynamicPropertyOwner object;
    auto property = std::make_unique<TestStructuredDynamicProperty>(
        QStringLiteral("structured"), QStringLiteral("Structured"));
    property->setProperty("gt.dynamicTypeId",
                          "org.gtlab.tests.structured-double");
    property->position.findProperty("x")->setValueFromVariant(1.25);
    property->position.findProperty("x")->setActive(false);
    property->position.findProperty("y")->setValueFromVariant(2.5);
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    const GtObjectMemento before = GtObjectIO().toMemento(&object);
    const GtObjectMemento loaded(before.toByteArray());
    ASSERT_EQ(loaded.dynamicProperties.size(), 1);
    const auto& loadedRoot = loaded.dynamicProperties.front();
    ASSERT_EQ(loadedRoot.nestedProperties.size(), 1);
    const auto& loadedStruct = loadedRoot.nestedProperties.front();
    EXPECT_EQ(loadedStruct.type(), GtObjectMemento::PropertyData::STRUCT_T);
    ASSERT_EQ(loadedStruct.childProperties.size(), 2);
    EXPECT_DOUBLE_EQ(loadedStruct.childProperties[0].data().toDouble(), 1.25);
    EXPECT_FALSE(loadedStruct.childProperties[0].isActive);
    EXPECT_DOUBLE_EQ(loadedStruct.childProperties[1].data().toDouble(), 2.5);

    TestDynamicPropertyOwner restored;
    ASSERT_TRUE(loaded.mergeTo(restored, *GtObjectFactory::instance()));
    auto* restoredRoot = dynamic_cast<TestStructuredDynamicProperty*>(
        restored.findProperty("structured"));
    ASSERT_NE(restoredRoot, nullptr);
    EXPECT_DOUBLE_EQ(restoredRoot->position.findProperty("x")->value<double>(),
                     1.25);
    EXPECT_FALSE(restoredRoot->position.findProperty("x")->isActive());
    EXPECT_DOUBLE_EQ(restoredRoot->position.findProperty("y")->value<double>(),
                     2.5);

    auto* currentRoot = dynamic_cast<TestStructuredDynamicProperty*>(
        object.findProperty("structured"));
    ASSERT_NE(currentRoot, nullptr);
    currentRoot->position.findProperty("x")->setValueFromVariant(8.0);
    currentRoot->position.findProperty("x")->setActive(true);
    currentRoot->position.findProperty("y")->setValueFromVariant(9.0);
    currentRoot->position.findProperty("y")->setActive(false);
    const GtObjectMemento after = GtObjectIO().toMemento(&object);
    GtObjectMementoDiff diff(before, after);

    ASSERT_TRUE(before.mergeTo(object, *GtObjectFactory::instance()));
    ASSERT_TRUE(GtObjectIO::applyDiff(diff, &object));
    currentRoot = dynamic_cast<TestStructuredDynamicProperty*>(
        object.findProperty("structured"));
    ASSERT_NE(currentRoot, nullptr);
    EXPECT_DOUBLE_EQ(currentRoot->position.findProperty("x")->value<double>(),
                     8.0);
    EXPECT_TRUE(currentRoot->position.findProperty("x")->isActive());
    EXPECT_DOUBLE_EQ(currentRoot->position.findProperty("y")->value<double>(),
                     9.0);
    EXPECT_FALSE(currentRoot->position.findProperty("y")->isActive());

    ASSERT_TRUE(GtObjectIO::revertDiff(diff, &object));
    currentRoot = dynamic_cast<TestStructuredDynamicProperty*>(
        object.findProperty("structured"));
    ASSERT_NE(currentRoot, nullptr);
    EXPECT_DOUBLE_EQ(currentRoot->position.findProperty("x")->value<double>(),
                     1.25);
    EXPECT_FALSE(currentRoot->position.findProperty("x")->isActive());
    EXPECT_DOUBLE_EQ(currentRoot->position.findProperty("y")->value<double>(),
                     2.5);
}

TEST_F(TestGtObjectDynamicProperty,
       HonorsDynamicPropertyMementoStorageOptOut)
{
    auto transient = std::make_unique<GtDoubleProperty>("transient",
                                                        "Transient");
    transient->setStoreToMemento(false);
    ASSERT_TRUE(object.addDynamicProperty(std::move(transient)));
    GtObjectMemento transientMemento = GtObjectIO().toMemento(&object);
    EXPECT_TRUE(transientMemento.dynamicPropertiesValid);
    EXPECT_TRUE(transientMemento.dynamicProperties.isEmpty());

    auto registration = registerConfiguredDynamicProperty();
    ASSERT_TRUE(registration);
    auto configured = configuredProperty("configured", "Configured", 3,
                                         7.5);
    configured->nestedProperty.setVal(123.0);
    configured->nestedProperty.setStoreToMemento(false);
    ASSERT_TRUE(object.addDynamicProperty(std::move(configured)));
    const GtObjectMemento configuredMemento =
        GtObjectIO().toMemento(&object);
    ASSERT_EQ(configuredMemento.dynamicProperties.size(), 1);
    EXPECT_EQ(configuredMemento.dynamicProperties.front().ident,
              "configured");
    EXPECT_TRUE(configuredMemento.dynamicProperties.front()
                    .nestedProperties.isEmpty());
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
