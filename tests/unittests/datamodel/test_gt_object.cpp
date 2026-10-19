/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QDebug>
#include <algorithm>
#include <QCoreApplication>
#include <QEvent>
#include <QPointer>
#include <QRegularExpression>
#include <QThread>

#include "gtest/gtest.h"

#include "test_gt_object.h"
#include "test_propertycontainerobject.h"

#include "gt_objectfactory.h"
#include "gt_factorygroup.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_propertyconnection.h"
#include "gt_structproperty.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"
#include "property/gt_dummyproperty.h"

#include "gt_labeldata.h"

#include "slotadaptor.h"
#include <QtConcurrent/QtConcurrent>


using DynamicPropertyData = GtObjectMemento::DynamicPropertyData;

static QVector<DynamicPropertyData>
dynamicPropertyDefinitions(const GtObject& object)
{
    return object.toMemento().dynamicProperties;
}

static bool
restoreDynamicProperties(GtObject& object,
                         const QVector<DynamicPropertyData>& definitions)
{
    auto memento = object.toMemento();
    memento.dynamicProperties = definitions;
    return memento.mergeDynamicPropertiesTo(object, object.factory());
}

static bool
updateDynamicPropertyDefinition(GtObject& object,
                                const DynamicPropertyData& definition)
{
    auto memento = object.toMemento();
    auto current = std::find_if(memento.dynamicProperties.begin(),
                                memento.dynamicProperties.end(),
                                [&definition](const auto& value) {
                                    return value.ident == definition.ident;
                                });
    if (current == memento.dynamicProperties.end()) return false;
    *current = definition;
    return memento.mergeDynamicPropertiesTo(object, object.factory());
}

class DynamicPropertyTestObject : public GtObject
{
public:
    DynamicPropertyTestObject()
    {
        setFactory(gtObjectFactory);
    }
    bool addStaticProperty(GtAbstractProperty& property)
    {
        return registerProperty(property);
    }
};

class ConfigurableStringProperty : public GtStringProperty
{
    Q_OBJECT
public:
    explicit ConfigurableStringProperty(const QString& id) :
        GtStringProperty(id, id)
    {
    }

    gt::PropertyConfiguration configuration() const override
    {
        return m_configuration;
    }

    bool applyConfiguration(
        const gt::PropertyConfiguration& configuration) override
    {
        if (!gt::isJsonLikePropertyConfiguration(configuration)) return false;
        m_configuration = configuration;
        return true;
    }

private:
    gt::PropertyConfiguration m_configuration;
};

class DerivedStringPropertyWithoutMetaObject : public GtStringProperty
{
public:
    explicit DerivedStringPropertyWithoutMetaObject(const QString& id) :
        GtStringProperty(id, id)
    {
    }
};

class StringPropertyWithIntChild : public GtStringProperty
{
    Q_OBJECT
public:
    explicit StringPropertyWithIntChild(const QString& id) :
        GtStringProperty(id, id, QString(), QStringLiteral("initial")),
        m_child(QStringLiteral("dynamicChild"), QStringLiteral("Child"),
                QString(), 1)
    {
        m_child.setOptional(true);
        registerSubProperty(m_child);
    }

    GtIntProperty* child()
    {
        return &m_child;
    }

private:
    GtIntProperty m_child;
};

class UnregisteredStringProperty : public GtStringProperty
{
    Q_OBJECT
public:
    explicit UnregisteredStringProperty(const QString& id) :
        GtStringProperty(id, id)
    {
    }
};

class RecoverableStringProperty : public StringPropertyWithIntChild
{
public:
    RecoverableStringProperty(const QString& id, bool acceptsConfiguration) :
        StringPropertyWithIntChild(id),
        m_acceptsConfiguration(acceptsConfiguration)
    {
    }

    bool applyConfiguration(
        const gt::PropertyConfiguration& configuration) override
    {
        return m_acceptsConfiguration &&
               GtStringProperty::applyConfiguration(configuration);
    }

private:
    bool m_acceptsConfiguration;
};

/// This is a test fixture that does a init for each test
class TestGtObject : public ::testing::Test
{
protected:
    void SetUp() override
    {
        obj.setObjectName("MyObject");
        obj.setFactory(gtObjectFactory);
    }

    GtObjectGroup obj;
};

TEST_F(TestGtObject, toMemento)
{
    qDebug() << "#### TEST_F(TestGtObject, toMemento)";

    // without properties
    GtObjectMemento memento = obj.toMemento();
    ASSERT_FALSE(memento.isNull());
}

TEST_F(TestGtObject, dynamicPropertyUsesCoreRegistryWithoutObjectFactory)
{
    GtObjectGroup object;
    ASSERT_EQ(object.factory(), nullptr);

    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtimeString"), QStringLiteral("Runtime string"));
    property->setVal(QStringLiteral("saved value"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    const auto memento = object.toMemento();
    ASSERT_EQ(memento.dynamicProperties.size(), 1);
    EXPECT_EQ(memento.dynamicProperties.front().typeId,
              QStringLiteral("gtlab.core.string"));

    GtObjectGroup restored;
    ASSERT_EQ(restored.factory(), nullptr);
    ASSERT_TRUE(memento.mergeDynamicPropertiesTo(restored, nullptr));
    const auto* value = qobject_cast<GtStringProperty*>(
        restored.findProperty(QStringLiteral("runtimeString")));
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->getVal(), QStringLiteral("saved value"));
}

TEST_F(TestGtObject, dynamicPropertyFallsBackIfObjectFactoryLacksType)
{
    GtFactoryGroup localFactory;
    GtObjectGroup object;
    object.setFactory(&localFactory);

    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtimeString"), QStringLiteral("Runtime string"));
    property->setVal(QStringLiteral("saved value"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    const auto memento = object.toMemento();
    ASSERT_EQ(memento.dynamicProperties.size(), 1);
    EXPECT_EQ(memento.dynamicProperties.front().typeId,
              QStringLiteral("gtlab.core.string"));

    GtObjectGroup restored;
    restored.setFactory(&localFactory);
    ASSERT_TRUE(memento.mergeDynamicPropertiesTo(restored, &localFactory));
    const auto* value = qobject_cast<GtStringProperty*>(
        restored.findProperty(QStringLiteral("runtimeString")));
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->getVal(), QStringLiteral("saved value"));
}

TEST_F(TestGtObject, dynamicPropertyPrefersLocallyRegisteredType)
{
    GtFactoryGroup localFactory;
    PropertyFactoryRegistration registration(
        localFactory, QStringLiteral("test.local_configurable"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(registration.isValid());

    GtObjectGroup object;
    object.setFactory(&localFactory);
    auto property = std::make_unique<ConfigurableStringProperty>(
        QStringLiteral("runtimeCustom"));
    property->setVal(QStringLiteral("local value"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    const auto memento = object.toMemento();
    ASSERT_EQ(memento.dynamicProperties.size(), 1);
    EXPECT_EQ(memento.dynamicProperties.front().typeId,
              QStringLiteral("test.local_configurable"));

    GtObjectGroup restored;
    restored.setFactory(&localFactory);
    ASSERT_TRUE(memento.mergeDynamicPropertiesTo(restored, &localFactory));
    const auto* value = dynamic_cast<ConfigurableStringProperty*>(
        restored.findProperty(QStringLiteral("runtimeCustom")));
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->getVal(), QStringLiteral("local value"));
}

TEST_F(TestGtObject, dynamicPropertiesShareThePropertyApiAndOwnership)
{
    EXPECT_EQ(
        gtObjectFactory->propertyTypeId(GtStringProperty::staticMetaObject),
        QStringLiteral("gtlab.core.string"));
    DynamicPropertyTestObject object;
    GtStringProperty staticProperty("sharedId", "Static", QString(), "static");
    ASSERT_TRUE(object.addStaticProperty(staticProperty));

    auto dynamicProperty = std::make_unique<GtStringProperty>(
        "runtimeId", "Runtime", QString(), "runtime");
    auto* dynamicRaw = dynamicProperty.get();
    ASSERT_TRUE(object.addDynamicProperty(std::move(dynamicProperty)));

    EXPECT_EQ(object.findProperty("runtimeId"), dynamicRaw);
    EXPECT_EQ(object.findProperty("sharedId"), &staticProperty);
    EXPECT_TRUE(object.properties().contains(dynamicRaw));

    auto duplicateId = std::make_unique<GtStringProperty>(
        "sharedId", "Duplicate", QString(), "duplicate");
    EXPECT_FALSE(object.addDynamicProperty(std::move(duplicateId)));

    EXPECT_TRUE(object.removeDynamicProperty("runtimeId"));
    EXPECT_EQ(object.findProperty("runtimeId"), nullptr);
    EXPECT_FALSE(object.removeDynamicProperty("runtimeId"));
}

TEST_F(TestGtObject, dynamicPropertyIdentifiersIncludeNestedProperties)
{
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, QStringLiteral("test.struct"),
        GtPropertyStructInstance::staticMetaObject, [](const QString& id) {
            GtPropertyStructDefinition definition(QStringLiteral("TestStruct"));
            definition.defineMember(
                QStringLiteral("nestedId"), [](const QString& childId) {
                    return new GtStringProperty(childId, childId);
                });
            return definition.newInstance(id).release();
        });
    ASSERT_TRUE(propertyFactory.isValid());

    DynamicPropertyTestObject object;
    GtPropertyStructDefinition staticDefinition(QStringLiteral("StaticStruct"));
    staticDefinition.defineMember(
        QStringLiteral("nestedId"),
        [](const QString& id) { return new GtStringProperty(id, id); });
    auto staticRoot =
        staticDefinition.newInstance(QStringLiteral("staticRoot"));
    ASSERT_TRUE(object.addStaticProperty(*staticRoot));

    auto conflicting =
        gt::createProperty(*gtObjectFactory, QStringLiteral("test.struct"),
                           QStringLiteral("runtimeRoot"));
    ASSERT_NE(conflicting, nullptr);
    EXPECT_FALSE(object.addDynamicProperty(std::move(conflicting)));

    DynamicPropertyData rootCollision;
    rootCollision.typeId = QStringLiteral("test.struct");
    rootCollision.ident = QStringLiteral("staticRoot");
    EXPECT_FALSE(restoreDynamicProperties(object, {rootCollision}));

    // A root ID that is free still cannot introduce a nested ID that overlaps
    // the static tree.
    rootCollision.ident = QStringLiteral("runtimeRoot");
    EXPECT_FALSE(restoreDynamicProperties(object, {rootCollision}));
    EXPECT_EQ(dynamicPropertyDefinitions(object).size(), 0);
}

TEST_F(TestGtObject, dynamicPropertyRegistrationRejectsTypeAndClassConflicts)
{
    PropertyFactoryRegistration registration(
        *gtObjectFactory, QStringLiteral("test.dynamic_configurable"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(registration.isValid());
    EXPECT_TRUE(gtObjectFactory->canRegisterPropertyFactory(
        QStringLiteral("test.other_id"),
        ConfigurableStringProperty::staticMetaObject));
    EXPECT_FALSE(gtObjectFactory->canRegisterPropertyFactory(
        QStringLiteral("test.dynamic_configurable"),
        GtIntProperty::staticMetaObject));
    EXPECT_FALSE(gtObjectFactory->canRegisterPropertyFactory(
        QStringLiteral("test.second_configurable"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); }));

    PropertyFactoryRegistration derivedRegistration(
        *gtObjectFactory, QStringLiteral("test.derived_string_without_meta"),
        GtStringProperty::staticMetaObject,
        [](const QString& id) {
            return new DerivedStringPropertyWithoutMetaObject(id);
        },
        std::type_index(typeid(DerivedStringPropertyWithoutMetaObject)));
    ASSERT_TRUE(derivedRegistration.isValid());
    auto derived = std::make_unique<DerivedStringPropertyWithoutMetaObject>(
        QStringLiteral("derivedRuntime"));
    ASSERT_TRUE(obj.addDynamicProperty(std::move(derived)));
    EXPECT_EQ(dynamicPropertyDefinitions(obj).back().typeId,
              QStringLiteral("test.derived_string_without_meta"));
}

TEST_F(TestGtObject, propertyFactoryBatchRegistrationIsAtomic)
{
    const auto configurableFactory = [](const QString& id) {
        return new ConfigurableStringProperty(id);
    };
    const auto nestedFactory = [](const QString& id) {
        return new StringPropertyWithIntChild(id);
    };
    const auto derivedFactory = [](const QString& id) {
        return new DerivedStringPropertyWithoutMetaObject(id);
    };

    const QList<PropertyFactoryData> duplicateIds{
        {QStringLiteral("test.batch_duplicate"),
         &ConfigurableStringProperty::staticMetaObject, configurableFactory},
        {QStringLiteral("test.batch_duplicate"),
         &StringPropertyWithIntChild::staticMetaObject, nestedFactory}};
    EXPECT_FALSE(gtObjectFactory->canRegisterPropertyFactories(duplicateIds));
    EXPECT_FALSE(gtObjectFactory->registerPropertyFactories(duplicateIds));

    const QList<PropertyFactoryData> duplicateTypes{
        {QStringLiteral("test.batch_configurable_a"),
         &ConfigurableStringProperty::staticMetaObject, configurableFactory},
        {QStringLiteral("test.batch_configurable_b"),
         &ConfigurableStringProperty::staticMetaObject, configurableFactory}};
    EXPECT_FALSE(gtObjectFactory->canRegisterPropertyFactories(duplicateTypes));
    EXPECT_FALSE(gtObjectFactory->registerPropertyFactories(duplicateTypes));

    PropertyFactoryRegistration existing(
        *gtObjectFactory, QStringLiteral("test.batch_existing"),
        ConfigurableStringProperty::staticMetaObject, configurableFactory);
    ASSERT_TRUE(existing.isValid());

    const QList<PropertyFactoryData> existingCollision{
        {QStringLiteral("test.batch_new"), &GtStringProperty::staticMetaObject,
         derivedFactory},
        {QStringLiteral("test.batch_existing"),
         &StringPropertyWithIntChild::staticMetaObject, nestedFactory}};
    EXPECT_FALSE(
        gtObjectFactory->canRegisterPropertyFactories(existingCollision));
    EXPECT_FALSE(gtObjectFactory->registerPropertyFactories(existingCollision));
    EXPECT_FALSE(
        gtObjectFactory->hasPropertyFactory(QStringLiteral("test.batch_new")));

    const QList<PropertyFactoryData> validBatch{
        {QStringLiteral("test.batch_nested"),
         &StringPropertyWithIntChild::staticMetaObject, nestedFactory},
        {QStringLiteral("test.batch_derived"),
         &GtStringProperty::staticMetaObject, derivedFactory}};
    ASSERT_TRUE(gtObjectFactory->registerPropertyFactories(validBatch));
    EXPECT_TRUE(gtObjectFactory->hasPropertyFactory(
        QStringLiteral("test.batch_nested")));
    EXPECT_TRUE(gtObjectFactory->hasPropertyFactory(
        QStringLiteral("test.batch_derived")));
    EXPECT_TRUE(gtObjectFactory->unregisterPropertyFactory(
        QStringLiteral("test.batch_nested")));
    EXPECT_TRUE(gtObjectFactory->unregisterPropertyFactory(
        QStringLiteral("test.batch_derived")));
}

TEST_F(TestGtObject, unregisteredPropertySubclassCannotUseBaseTypeId)
{
    auto property = std::make_unique<UnregisteredStringProperty>(
        QStringLiteral("unregisteredSubclass"));
    EXPECT_FALSE(obj.addDynamicProperty(std::move(property)));
    EXPECT_EQ(obj.findProperty(QStringLiteral("unregisteredSubclass")),
              nullptr);
}

TEST_F(TestGtObject, dynamicDoubleConstructionConfigurationRoundtrips)
{
    auto property = std::make_unique<GtDoubleProperty>(
        QStringLiteral("configuredDouble"), QStringLiteral("Configured"),
        QStringLiteral("pressure"), GtUnit::Pressure,
        gt::Boundaries<double>::makeNormalized(1.25, 20.75), 4.125);
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));
    auto* original = qobject_cast<GtDoubleProperty*>(
        obj.findProperty(QStringLiteral("configuredDouble")));
    ASSERT_NE(original, nullptr);
    original->setVal(12.5);

    auto restored = GtObjectMemento(obj.toMemento().toByteArray())
                        .toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    auto* restoredProperty = qobject_cast<GtDoubleProperty*>(
        restored->findProperty(QStringLiteral("configuredDouble")));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_EQ(restoredProperty->unitCategory(), GtUnit::Pressure);
    EXPECT_TRUE(restoredProperty->lowSideBoundaryActive());
    EXPECT_TRUE(restoredProperty->highSideBoundaryActive());
    EXPECT_DOUBLE_EQ(restoredProperty->lowSideBoundary(), 1.25);
    EXPECT_DOUBLE_EQ(restoredProperty->highSideBoundary(), 20.75);
    EXPECT_DOUBLE_EQ(restoredProperty->initialValue(), 4.125);
    EXPECT_DOUBLE_EQ(restoredProperty->getVal(), 12.5);
}

TEST_F(TestGtObject, connectedDynamicPropertyStoresConfiguredReadOnlyFlag)
{
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("connectedValue"), QStringLiteral("Connected"));
    property->setReadOnly(false);
    GtPropertyConnection connection;
    property->propertyConnect(&connection);
    ASSERT_TRUE(property->isReadOnly());
    ASSERT_FALSE(property->isReadOnlyConfigured());
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));
    ASSERT_FALSE(dynamicPropertyDefinitions(obj).front().hasFlag(
        DynamicPropertyData::ReadOnly));

    auto restored = GtObjectMemento(obj.toMemento().toByteArray())
                        .toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    auto* restoredProperty =
        restored->findProperty(QStringLiteral("connectedValue"));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_FALSE(restoredProperty->isReadOnlyConfigured());
    EXPECT_FALSE(restoredProperty->isReadOnly());
}

TEST_F(TestGtObject, unavailableDynamicPropertyTypeIsPreservedAndRecovers)
{
    PropertyFactoryRegistration registration(
        *gtObjectFactory, QStringLiteral("test.recoverable_string"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(registration.isValid());

    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    auto property = std::make_unique<ConfigurableStringProperty>(
        QStringLiteral("recoverable"));
    property->setVal(QStringLiteral("preserved value"));
    property->setReadOnly(true);
    property->hide(true);
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    const auto memento = object.toMemento();
    const auto bytes = memento.toByteArray();

    ASSERT_TRUE(gtObjectFactory->unregisterPropertyFactory(
        QStringLiteral("test.recoverable_string")));
    auto unavailable = GtObjectMemento(bytes).toObject(*gtObjectFactory);
    ASSERT_NE(unavailable, nullptr);
    auto* dummy = qobject_cast<GtDummyProperty*>(
        unavailable->findProperty(QStringLiteral("recoverable")));
    ASSERT_NE(dummy, nullptr);
    EXPECT_EQ(dummy->storedValue().toString(),
              QStringLiteral("preserved value"));
    EXPECT_FALSE(dummy->isHidden());
    EXPECT_TRUE(dummy->isReadOnly());
    EXPECT_EQ(dummy->originalDefinition().typeId,
              QStringLiteral("test.recoverable_string"));
    EXPECT_EQ(unavailable->toMemento().dynamicProperties.front().typeId,
              QStringLiteral("test.recoverable_string"));

    ASSERT_TRUE(gtObjectFactory->registerPropertyFactory(
        QStringLiteral("test.recoverable_string"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); }));
    auto recovered = GtObjectMemento(bytes).toObject(*gtObjectFactory);
    ASSERT_NE(recovered, nullptr);
    EXPECT_NE(qobject_cast<ConfigurableStringProperty*>(
                  recovered->findProperty(QStringLiteral("recoverable"))),
              nullptr);
}

TEST_F(TestGtObject, unreconstructibleDynamicPropertyPreservesRecordAndRecovers)
{
    // Factories may become unavailable or incompatible after module updates.
    for (const bool constructionFails : {false, true})
    {
        SCOPED_TRACE(constructionFails);
        bool available = true;
        const QString typeId = QStringLiteral("test.recoverable_config");
        PropertyFactoryRegistration registration(
            *gtObjectFactory, typeId,
            RecoverableStringProperty::staticMetaObject,
            [&](const QString& id) -> RecoverableStringProperty* {
                if (!available && constructionFails) return nullptr;
                return new RecoverableStringProperty(id, available);
            });
        ASSERT_TRUE(registration.isValid());

        auto property = std::make_unique<RecoverableStringProperty>(
            QStringLiteral("recoverable"), true);
        property->setObjectName(QStringLiteral("Recoverable property"));
        property->setBrief(QStringLiteral("Original brief"));
        property->setOptional(true);
        property->setActive(false);
        property->setReadOnly(true);
        property->hide(true);
        property->setMonitoring(true);
        property->setCategory(QStringLiteral("Custom category"));
        property->setVal(QStringLiteral("saved root"));
        property->child()->setVal(42);
        property->child()->setActive(false);
        ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));
        const auto original = obj.toMemento();
        original.calculateHashes();
        ASSERT_EQ(original.dynamicProperties.size(), 1);
        const auto& record = original.dynamicProperties.front();
        ASSERT_TRUE(gt::isJsonLikePropertyConfiguration(record.configuration));
        ASSERT_FALSE(record.configuration.isEmpty());

        available = false;
        QPointer<GtAbstractProperty> originalInstance(
            obj.findProperty(record.ident));
        ASSERT_TRUE(original.mergeTo(obj, *gtObjectFactory));
        EXPECT_FALSE(originalInstance);
        ASSERT_NE(
            qobject_cast<GtDummyProperty*>(obj.findProperty(record.ident)),
            nullptr);
        auto unavailable =
            GtObjectMemento(original.toByteArray()).toObject(*gtObjectFactory);
        ASSERT_NE(unavailable, nullptr);
        auto* dummy = qobject_cast<GtDummyProperty*>(
            unavailable->findProperty(record.ident));
        ASSERT_NE(dummy, nullptr);
        EXPECT_TRUE(dummy->isReadOnly());
        EXPECT_EQ(dummy->valueToVariant(QString()).toString(),
                  QStringLiteral("Unavailable"));
        EXPECT_EQ(dummy->originalDefinition(), record);
        EXPECT_EQ(dummy->storedValue(), record.propertyData.data());

        const auto resaved =
            GtObjectMemento(unavailable->toMemento().toByteArray());
        ASSERT_EQ(resaved.dynamicProperties.size(), 1);
        EXPECT_EQ(resaved.dynamicProperties.front(), record);
        resaved.calculateHashes();
        EXPECT_EQ(resaved.propertyHash(), original.propertyHash());
        QPointer<GtAbstractProperty> unchangedDummy(dummy);
        ASSERT_TRUE(resaved.mergeTo(*unavailable, *gtObjectFactory));
        EXPECT_TRUE(unchangedDummy);
        ASSERT_NE(qobject_cast<GtDummyProperty*>(
                      unavailable->findProperty(record.ident)),
                  nullptr);

        available = true;
        ASSERT_TRUE(resaved.mergeTo(*unavailable, *gtObjectFactory));
        EXPECT_FALSE(unchangedDummy);
        auto* recovered = dynamic_cast<RecoverableStringProperty*>(
            unavailable->findProperty(record.ident));
        ASSERT_NE(recovered, nullptr);
        EXPECT_EQ(recovered->configuration(), record.configuration);
        EXPECT_EQ(recovered->getVal(), QStringLiteral("saved root"));
        EXPECT_FALSE(recovered->isActive());
        EXPECT_EQ(recovered->child()->getVal(), 42);
        EXPECT_FALSE(recovered->child()->isActive());
        const auto recoveredMemento = unavailable->toMemento();
        EXPECT_EQ(recoveredMemento.dynamicProperties.front(), record);
        recoveredMemento.calculateHashes();
        EXPECT_EQ(recoveredMemento.propertyHash(), original.propertyHash());
        ASSERT_TRUE(obj.removeDynamicProperty(record.ident));
    }
}

TEST_F(TestGtObject, missingRequiredDynamicConfigurationBecomesDummy)
{
    ASSERT_TRUE(obj.addDynamicProperty(std::make_unique<GtStringProperty>(
        QStringLiteral("missingConfig"), QStringLiteral("Missing config"),
        QString(), QStringLiteral("saved value"))));
    auto memento = obj.toMemento();
    memento.dynamicProperties.front().configuration.clear();
    auto restored =
        GtObjectMemento(memento.toByteArray()).toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    auto* dummy = qobject_cast<GtDummyProperty*>(
        restored->findProperty(QStringLiteral("missingConfig")));
    ASSERT_NE(dummy, nullptr);
    EXPECT_EQ(dummy->originalDefinition(), memento.dynamicProperties.front());
    EXPECT_EQ(dummy->storedValue().toString(), QStringLiteral("saved value"));
}

TEST_F(TestGtObject, moveToThreadMovesDynamicProperties)
{
    DynamicPropertyTestObject object;
    GtStringProperty staticProperty("staticId", "Static", QString(), "s");
    ASSERT_TRUE(object.addStaticProperty(staticProperty));

    auto dynamicProperty = std::make_unique<GtStringProperty>(
        "dynamicId", "Dynamic", QString(), "d");
    auto* dynamicRaw = dynamicProperty.get();
    ASSERT_TRUE(object.addDynamicProperty(std::move(dynamicProperty)));

    QThread targetThread;
    targetThread.start();
    gt::moveToThread(object, &targetThread);

    EXPECT_EQ(object.thread(), &targetThread);
    EXPECT_EQ(staticProperty.thread(), &targetThread);
    EXPECT_EQ(dynamicRaw->thread(), &targetThread);

    auto* objectPtr = &object;
    ASSERT_TRUE(QMetaObject::invokeMethod(
        objectPtr,
        [objectPtr]() {
            gt::moveToThread(*objectPtr, QThread::currentThread());
        },
        Qt::BlockingQueuedConnection));
    targetThread.quit();
    targetThread.wait();
}

TEST_F(TestGtObject, dynamicPropertiesPersistDefinitionAndValue)
{
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, QStringLiteral("test.configurable"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(propertyFactory.isValid());
    gtObjectFactory->registerClass(GtObjectGroup::staticMetaObject);

    auto property =
        std::make_unique<ConfigurableStringProperty>("runtimeValue");
    property->setObjectName(QStringLiteral("Runtime value"));
    property->setBrief(QStringLiteral("persisted brief"));
    property->setVal(QStringLiteral("initial"));
    property->setReadOnly(true);
    property->setOptional(true);
    property->hide(true);
    property->setMonitoring(true);
    property->setCategory(QStringLiteral("Runtime custom category"));
    auto* rawProperty = property.get();
    gt::PropertyConfiguration configuration;
    configuration.insert(QStringLiteral("enabled"), true);
    configuration.insert(QStringLiteral("count"), 4);
    configuration.insert(QStringLiteral("threshold"), 1.2345678901234567);
    configuration.insert(QStringLiteral("label"), QStringLiteral("sample"));
    configuration.insert(
        QStringLiteral("items"),
        QVariantList{true, 3, 1.5, QStringLiteral("text"), QVariant{}});
    configuration.insert(
        QStringLiteral("object"),
        QVariantMap{{QStringLiteral("nested"), QStringLiteral("value")}});
    ASSERT_TRUE(property->applyConfiguration(configuration));
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));
    rawProperty->setVal("saved value");

    const auto bytes = obj.toMemento().toByteArray();
    const GtObjectMemento restoredMemento(bytes);
    ASSERT_EQ(restoredMemento.properties.size(), 0);
    ASSERT_EQ(restoredMemento.dynamicProperties.size(), 1);
    EXPECT_EQ(restoredMemento.dynamicProperties.front().propertyData.name,
              QStringLiteral("runtimeValue"));
    EXPECT_FALSE(restoredMemento.toByteArray().contains("formatVersion"));
    auto restored = restoredMemento.toObject(*gtObjectFactory);
    ASSERT_TRUE(restored);

    auto* restoredProperty = dynamic_cast<ConfigurableStringProperty*>(
        restored->findProperty("runtimeValue"));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_EQ(restoredProperty->objectName(), QStringLiteral("Runtime value"));
    EXPECT_EQ(restoredProperty->brief(), QStringLiteral("persisted brief"));
    EXPECT_EQ(restoredProperty->getVal(), QStringLiteral("saved value"));
    EXPECT_TRUE(restoredProperty->isReadOnly());
    EXPECT_TRUE(restoredProperty->isOptional());
    EXPECT_TRUE(restoredProperty->isHidden());
    EXPECT_TRUE(restoredProperty->isMonitoring());
    EXPECT_EQ(restoredProperty->categoryString(),
              QStringLiteral("Runtime custom category"));

    const auto definitions = dynamicPropertyDefinitions(*restored);
    ASSERT_EQ(definitions.size(), 1);
    EXPECT_EQ(definitions.front().typeId, QStringLiteral("test.configurable"));
    EXPECT_EQ(definitions.front().ident, QStringLiteral("runtimeValue"));
    EXPECT_EQ(definitions.front().configuration, configuration);
    EXPECT_TRUE(definitions.front().hasFlag(DynamicPropertyData::ReadOnly));
    EXPECT_TRUE(definitions.front().hasFlag(DynamicPropertyData::Optional));
    EXPECT_TRUE(definitions.front().hasFlag(DynamicPropertyData::Hidden));
    EXPECT_TRUE(definitions.front().hasFlag(DynamicPropertyData::Monitoring));
    EXPECT_EQ(definitions.front().category,
              QStringLiteral("Runtime custom category"));
    EXPECT_EQ(restoredProperty->configuration(), configuration);
    gt::PropertyConfiguration invalidConfiguration{
        {QStringLiteral("unsupported"),
         QVariant::fromValue(QDate{2026, 1, 1})}};
    EXPECT_FALSE(gt::isJsonLikePropertyConfiguration(invalidConfiguration));
}

TEST_F(TestGtObject, dynamicPropertyConfigurationDepthMatchesXmlReader)
{
    const auto nestedValue = [](int depth) {
        QVariant value = QStringLiteral("leaf");
        while (depth-- > 0)
        {
            value = QVariantMap{{QStringLiteral("nested"), value}};
        }
        return value;
    };
    const gt::PropertyConfiguration configuration{
        {QStringLiteral("nested"),
         nestedValue(gt::PropertyConfigurationMaxDepth - 1)}};
    const gt::PropertyConfiguration tooDeepConfiguration{
        {QStringLiteral("nested"),
         nestedValue(gt::PropertyConfigurationMaxDepth)}};
    ASSERT_TRUE(gt::isJsonLikePropertyConfiguration(configuration));
    EXPECT_FALSE(gt::isJsonLikePropertyConfiguration(tooDeepConfiguration));

    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, QStringLiteral("test.configurable-depth"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(propertyFactory.isValid());

    auto property = std::make_unique<ConfigurableStringProperty>("depthConfig");
    ASSERT_TRUE(property->applyConfiguration(configuration));
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));

    const GtObjectMemento parsed(obj.toMemento().toByteArray());
    ASSERT_FALSE(parsed.isNull());
    const auto restored = parsed.toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    const auto* restoredProperty = dynamic_cast<ConfigurableStringProperty*>(
        restored->findProperty(QStringLiteral("depthConfig")));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_EQ(restoredProperty->configuration(), configuration);
}

TEST_F(TestGtObject, nonStructDynamicPropertyChildStateRoundtripsAndApplies)
{
    const QString typeId = QStringLiteral("test.string-with-int-child");
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, typeId, StringPropertyWithIntChild::staticMetaObject,
        [](const QString& id) { return new StringPropertyWithIntChild(id); });
    ASSERT_TRUE(propertyFactory.isValid());
    gtObjectFactory->registerClass(GtObjectGroup::staticMetaObject);

    GtObjectGroup source;
    GtObjectGroup target;
    source.setFactory(gtObjectFactory);
    target.setFactory(gtObjectFactory);
    source.setUuid(QStringLiteral("dynamic-child-state"));
    target.setUuid(QStringLiteral("dynamic-child-state"));
    const auto before = source.toMemento(true);
    auto property = std::make_unique<StringPropertyWithIntChild>(
        QStringLiteral("dynamicRoot"));
    auto* raw = property.get();
    QPointer<GtAbstractProperty> originalRoot(raw);
    raw->setVal(QStringLiteral("root value"));
    ASSERT_TRUE(source.addDynamicProperty(std::move(property)));
    raw->child()->setVal(42);
    raw->child()->setActive(false);
    auto definition = dynamicPropertyDefinitions(source).front();
    definition.name = QStringLiteral("renamed dynamic root");
    ASSERT_TRUE(updateDynamicPropertyDefinition(source, definition));
    auto* replaced = dynamic_cast<StringPropertyWithIntChild*>(
        source.findProperty(QStringLiteral("dynamicRoot")));
    ASSERT_NE(replaced, nullptr);
    EXPECT_FALSE(originalRoot);
    EXPECT_EQ(replaced->getVal(), QStringLiteral("root value"));
    EXPECT_EQ(replaced->child()->getVal(), 42);
    EXPECT_FALSE(replaced->child()->isActive());

    auto invalidDefinition = definition;
    invalidDefinition.propertyData.childProperties.front().setData(
        QStringLiteral("not an integer"));
    QPointer<GtAbstractProperty> beforeFailure(replaced);
    EXPECT_FALSE(updateDynamicPropertyDefinition(source, invalidDefinition));
    EXPECT_TRUE(beforeFailure);
    EXPECT_EQ(source.findProperty(QStringLiteral("dynamicRoot")), replaced);
    EXPECT_EQ(replaced->child()->getVal(), 42);

    const auto after = source.toMemento(true);
    auto renamedMemento = source.toMemento(true);
    renamedMemento.dynamicProperties.front().name =
        QStringLiteral("renamed through memento");
    QPointer<GtAbstractProperty> beforeMementoMerge(replaced);
    ASSERT_TRUE(renamedMemento.mergeTo(source, *gtObjectFactory));
    EXPECT_FALSE(beforeMementoMerge);
    auto* mergedReplacement = dynamic_cast<StringPropertyWithIntChild*>(
        source.findProperty(QStringLiteral("dynamicRoot")));
    ASSERT_NE(mergedReplacement, nullptr);
    EXPECT_EQ(mergedReplacement->getVal(), QStringLiteral("root value"));
    EXPECT_EQ(mergedReplacement->child()->getVal(), 42);
    EXPECT_FALSE(mergedReplacement->child()->isActive());

    ASSERT_EQ(after.properties.size(), 0);
    ASSERT_EQ(after.dynamicProperties.size(), 1);
    const auto& propertyData = after.dynamicProperties.front().propertyData;
    ASSERT_EQ(propertyData.childProperties.size(), 1);
    EXPECT_EQ(propertyData.childProperties.front().name,
              QStringLiteral("dynamicChild"));
    EXPECT_EQ(propertyData.childProperties.front().data().toInt(), 42);

    const auto restored =
        GtObjectMemento(after.toByteArray()).toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    auto* restoredProperty = dynamic_cast<StringPropertyWithIntChild*>(
        restored->findProperty(QStringLiteral("dynamicRoot")));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_EQ(restoredProperty->getVal(), QStringLiteral("root value"));
    EXPECT_EQ(restoredProperty->child()->getVal(), 42);

    GtObjectMementoDiff addDiff(before, after);
    ASSERT_TRUE(target.applyDiff(addDiff));
    auto* appliedProperty = dynamic_cast<StringPropertyWithIntChild*>(
        target.findProperty(QStringLiteral("dynamicRoot")));
    ASSERT_NE(appliedProperty, nullptr);
    EXPECT_EQ(appliedProperty->getVal(), QStringLiteral("root value"));
    EXPECT_EQ(appliedProperty->child()->getVal(), 42);
}

TEST_F(TestGtObject, dynamicPropertyStoreOptOutOmitsChildProperties)
{
    const QString typeId = QStringLiteral("test.string-with-int-child");
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, typeId, StringPropertyWithIntChild::staticMetaObject,
        [](const QString& id) { return new StringPropertyWithIntChild(id); });
    ASSERT_TRUE(propertyFactory.isValid());

    auto property = std::make_unique<StringPropertyWithIntChild>("dynamicRoot");
    property->setStoreToMemento(false);
    property->child()->setVal(42);
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));

    const auto memento = obj.toMemento();
    EXPECT_TRUE(memento.dynamicProperties.isEmpty());
    EXPECT_EQ(GtObjectMemento::findPropertyByName(
                  memento.properties, QStringLiteral("dynamicRoot")),
              nullptr);
    EXPECT_EQ(GtObjectMemento::findPropertyByName(
                  memento.properties, QStringLiteral("dynamicChild")),
              nullptr);
}

TEST_F(TestGtObject, addDynamicPropertyPreservesLiveValue)
{
    DynamicPropertyTestObject object;
    auto property = std::make_unique<GtDoubleProperty>(
        QStringLiteral("runtimeDouble"), QStringLiteral("Runtime double"),
        QString(), GtUnit::Default, 50.0);
    ASSERT_TRUE(property->setValueFromVariant(12.0, QString()));
    auto* raw = property.get();
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    EXPECT_EQ(object.findProperty(QStringLiteral("runtimeDouble")), raw);
    EXPECT_DOUBLE_EQ(raw->valueToVariant(QString()).toDouble(), 12.0);
}

TEST_F(TestGtObject, dynamicPropertyDefinitionChangesAreReversible)
{
    const auto empty = obj.toMemento();
    auto property = std::make_unique<GtStringProperty>(
        "runtimeValue", "Runtime value", "brief", "default");
    auto* rawProperty = property.get();
    ASSERT_TRUE(obj.addDynamicProperty(std::move(property)));
    rawProperty->setVal("saved value");
    const auto added = obj.toMemento();

    GtObjectMementoDiff addDiff(empty, added);
    ASSERT_TRUE(obj.revertDiff(addDiff));
    EXPECT_EQ(obj.findProperty("runtimeValue"), nullptr);
    ASSERT_TRUE(obj.applyDiff(addDiff));
    ASSERT_NE(obj.findProperty("runtimeValue"), nullptr);
    EXPECT_EQ(qobject_cast<GtStringProperty*>(obj.findProperty("runtimeValue"))
                  ->getVal(),
              QStringLiteral("saved value"));

    auto definition = dynamicPropertyDefinitions(obj).front();
    definition.name = QStringLiteral("Renamed runtime value");
    definition.brief = QStringLiteral("updated brief");
    ASSERT_TRUE(updateDynamicPropertyDefinition(obj, definition));
    const auto changed = obj.toMemento();
    GtObjectMementoDiff changeDiff(added, changed);
    ASSERT_TRUE(obj.revertDiff(changeDiff));
    EXPECT_EQ(obj.findProperty("runtimeValue")->objectName(),
              QStringLiteral("Runtime value"));
    ASSERT_TRUE(obj.applyDiff(changeDiff));
    EXPECT_EQ(obj.findProperty("runtimeValue")->objectName(),
              QStringLiteral("Renamed runtime value"));

    ASSERT_TRUE(obj.removeDynamicProperty("runtimeValue"));
    const auto removed = obj.toMemento();
    GtObjectMementoDiff removeDiff(changed, removed);
    ASSERT_TRUE(obj.revertDiff(removeDiff));
    EXPECT_NE(obj.findProperty("runtimeValue"), nullptr);
    ASSERT_TRUE(obj.applyDiff(removeDiff));
    EXPECT_EQ(obj.findProperty("runtimeValue"), nullptr);
}

TEST_F(TestGtObject, builtinDynamicConstructionConfigurationRoundtrips)
{
    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    gtObjectFactory->registerClass(GtObjectGroup::staticMetaObject);
    auto integer = std::make_unique<GtIntProperty>(
        QStringLiteral("runtimeInt"), QStringLiteral("Runtime Int"),
        QStringLiteral("bounded"), gt::Boundaries<int>::makeNormalized(2, 20),
        7);
    const auto intConfig = integer->configuration();
    ASSERT_TRUE(object.addDynamicProperty(std::move(integer)));

    QRegularExpression regex(QStringLiteral("[A-Z]+"),
                             QRegularExpression::CaseInsensitiveOption);
    auto string = std::make_unique<GtStringProperty>(
        QStringLiteral("runtimeString"), QStringLiteral("Runtime String"),
        QString(), QStringLiteral("alpha"), regex);
    const auto stringConfig = string->configuration();
    ASSERT_TRUE(object.addDynamicProperty(std::move(string)));

    auto boolean = std::make_unique<GtBoolProperty>(
        QStringLiteral("runtimeBool"), QStringLiteral("Runtime Bool"),
        QString(), true);
    const auto boolConfig = boolean->configuration();
    ASSERT_TRUE(object.addDynamicProperty(std::move(boolean)));

    const auto restored = GtObjectMemento(object.toMemento().toByteArray())
                              .toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(
        restored->findProperty(QStringLiteral("runtimeInt"))->configuration(),
        intConfig);
    EXPECT_EQ(restored->findProperty(QStringLiteral("runtimeString"))
                  ->configuration(),
              stringConfig);
    EXPECT_EQ(
        restored->findProperty(QStringLiteral("runtimeBool"))->configuration(),
        boolConfig);
}

TEST_F(TestGtObject, dynamicStringInitialValueRoundtripsThroughXml)
{
    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    gtObjectFactory->registerClass(GtObjectGroup::staticMetaObject);
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtimeString"), QStringLiteral("Runtime String"),
        QString(), QStringLiteral("default"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    auto* current = qobject_cast<GtStringProperty*>(
        object.findProperty(QStringLiteral("runtimeString")));
    ASSERT_NE(current, nullptr);
    current->setVal(QStringLiteral("edited"));

    const auto xml = object.toMemento().toByteArray();
    const auto restored = GtObjectMemento(xml).toObject(*gtObjectFactory);
    ASSERT_NE(restored, nullptr);
    auto* restoredProperty = qobject_cast<GtStringProperty*>(
        restored->findProperty(QStringLiteral("runtimeString")));
    ASSERT_NE(restoredProperty, nullptr);
    EXPECT_EQ(restoredProperty->getVal(), QStringLiteral("edited"));
    restoredProperty->revert();
    EXPECT_EQ(restoredProperty->getVal(), QStringLiteral("default"));
}

TEST_F(TestGtObject, unchangedDynamicPropertiesKeepPointersAndConnections)
{
    GtObjectGroup parent;
    auto* source = new DynamicPropertyTestObject;
    source->setUuid(QStringLiteral("source"));
    auto* target = new DynamicPropertyTestObject;
    target->setUuid(QStringLiteral("target"));
    ASSERT_TRUE(parent.appendChild(source));
    ASSERT_TRUE(parent.appendChild(target));

    GtStringProperty sourceProperty(QStringLiteral("sourceValue"),
                                    QStringLiteral("Source"), QString(),
                                    QStringLiteral("initial"));
    ASSERT_TRUE(source->addStaticProperty(sourceProperty));
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("dynamicValue"), QStringLiteral("Dynamic"), QString(),
        QStringLiteral("initial"));
    auto* dynamicProperty = property.get();
    ASSERT_TRUE(target->addDynamicProperty(std::move(property)));

    auto* connection = new GtPropertyConnection;
    connection->setSourceUuid(source->uuid());
    connection->setTargetUuid(target->uuid());
    connection->setSourceProp(QStringLiteral("sourceValue"));
    connection->setTargetProp(QStringLiteral("dynamicValue"));
    ASSERT_TRUE(parent.appendChild(connection));
    connection->makeConnection();
    ASSERT_TRUE(dynamicProperty->isConnected());
    EXPECT_TRUE(connection->isConnected());

    auto snapshot = target->toMemento();
    ASSERT_TRUE(snapshot.mergeTo(*target, *gtObjectFactory));
    EXPECT_EQ(target->findProperty(QStringLiteral("dynamicValue")),
              dynamicProperty);
    EXPECT_TRUE(dynamicProperty->isConnected());

    auto definition = dynamicPropertyDefinitions(*target).front();
    definition.name = QStringLiteral("Renamed dynamic value");
    definition.brief = QStringLiteral("updated brief");
    QPointer<GtAbstractProperty> beforeRename(dynamicProperty);
    ASSERT_TRUE(updateDynamicPropertyDefinition(*target, definition));
    auto* renamedProperty =
        target->findProperty(QStringLiteral("dynamicValue"));
    ASSERT_NE(renamedProperty, nullptr);
    EXPECT_FALSE(beforeRename);
    EXPECT_TRUE(renamedProperty->isConnected());

    sourceProperty.setVal(QStringLiteral("after merge"));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(renamedProperty)->getVal(),
              QStringLiteral("after merge"));

    auto definitionChange = target->toMemento();
    definitionChange.dynamicProperties.front().name =
        QStringLiteral("Definition changed by merge");
    QPointer<GtAbstractProperty> beforeDefinitionMerge(renamedProperty);
    ASSERT_TRUE(definitionChange.mergeTo(*target, *gtObjectFactory));
    EXPECT_FALSE(beforeDefinitionMerge);
    auto* replacedProperty =
        target->findProperty(QStringLiteral("dynamicValue"));
    ASSERT_NE(replacedProperty, nullptr);
    EXPECT_TRUE(connection->isConnected());
    EXPECT_TRUE(replacedProperty->isConnected());
    sourceProperty.setVal(QStringLiteral("after definition merge"));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(replacedProperty)->getVal(),
              QStringLiteral("after definition merge"));
}

TEST_F(TestGtObject, dynamicSourceAndTargetReplacementReconnectOnUndoRedo)
{
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, QStringLiteral("test.connection_configured_string"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(propertyFactory.isValid());

    GtObjectGroup root;
    root.setFactory(gtObjectFactory);
    auto* source = new GtObjectGroup;
    source->setFactory(gtObjectFactory);
    source->setUuid(QStringLiteral("connection-source"));
    auto* target = new GtObjectGroup;
    target->setFactory(gtObjectFactory);
    target->setUuid(QStringLiteral("connection-target"));
    ASSERT_TRUE(root.appendChild(source));
    ASSERT_TRUE(root.appendChild(target));

    auto makeProperty = [](const QString& id, const QString& value) {
        auto property = std::make_unique<ConfigurableStringProperty>(id);
        property->setVal(value);
        property->applyConfiguration({{QStringLiteral("size"), 3}});
        return property;
    };
    ASSERT_TRUE(source->addDynamicProperty(makeProperty(
        QStringLiteral("sourceDynamic"), QStringLiteral("initial source"))));
    ASSERT_TRUE(target->addDynamicProperty(makeProperty(
        QStringLiteral("targetDynamic"), QStringLiteral("initial target"))));

    auto* connection = new GtPropertyConnection;
    connection->setSourceUuid(source->uuid());
    connection->setTargetUuid(target->uuid());
    connection->setSourceProp(QStringLiteral("sourceDynamic"));
    connection->setTargetProp(QStringLiteral("targetDynamic"));
    ASSERT_TRUE(root.appendChild(connection));
    connection->makeConnection();
    ASSERT_TRUE(connection->isConnected());
    QPointer<GtPropertyConnection> guardedConnection(connection);
    auto* originalSource =
        source->findProperty(QStringLiteral("sourceDynamic"));
    auto* originalTarget =
        target->findProperty(QStringLiteral("targetDynamic"));
    QPointer<GtAbstractProperty> originalSourceGuard(originalSource);
    QPointer<GtAbstractProperty> originalTargetGuard(originalTarget);

    const auto before = root.toMemento();
    auto sourceDefinition = dynamicPropertyDefinitions(*source).front();
    auto targetDefinition = dynamicPropertyDefinitions(*target).front();
    sourceDefinition.configuration = {{QStringLiteral("size"), 8}};
    targetDefinition.configuration = {{QStringLiteral("size"), 8}};
    ASSERT_TRUE(updateDynamicPropertyDefinition(*source, sourceDefinition));
    ASSERT_TRUE(updateDynamicPropertyDefinition(*target, targetDefinition));
    const auto after = root.toMemento();
    GtObjectMementoDiff diff(before, after);

    ASSERT_TRUE(root.revertDiff(diff));
    auto* revertedSource =
        source->findProperty(QStringLiteral("sourceDynamic"));
    auto* revertedTarget =
        target->findProperty(QStringLiteral("targetDynamic"));
    EXPECT_FALSE(originalSourceGuard);
    EXPECT_FALSE(originalTargetGuard);
    ASSERT_TRUE(connection->isConnected());
    ASSERT_TRUE(revertedTarget->isConnected());
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    ASSERT_TRUE(guardedConnection);
    qobject_cast<GtStringProperty*>(revertedSource)
        ->setVal(QStringLiteral("source after undo"));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(revertedTarget)->getVal(),
              QStringLiteral("source after undo"));
    QPointer<GtAbstractProperty> revertedSourceGuard(revertedSource);
    QPointer<GtAbstractProperty> revertedTargetGuard(revertedTarget);

    ASSERT_TRUE(root.applyDiff(diff));
    auto* appliedSource = source->findProperty(QStringLiteral("sourceDynamic"));
    auto* appliedTarget = target->findProperty(QStringLiteral("targetDynamic"));
    EXPECT_FALSE(revertedSourceGuard);
    EXPECT_FALSE(revertedTargetGuard);
    ASSERT_TRUE(connection->isConnected());
    ASSERT_TRUE(appliedTarget->isConnected());
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    ASSERT_TRUE(guardedConnection);
    qobject_cast<GtStringProperty*>(appliedSource)
        ->setVal(QStringLiteral("source after redo"));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(appliedTarget)->getVal(),
              QStringLiteral("source after redo"));
}

TEST_F(TestGtObject, removedDynamicEndpointsReconnectWhenAddedAgain)
{
    GtObjectGroup root;
    auto* source = new DynamicPropertyTestObject;
    auto* target = new DynamicPropertyTestObject;
    ASSERT_TRUE(root.appendChild(source));
    ASSERT_TRUE(root.appendChild(target));
    const QString id = QStringLiteral("endpoint");
    const auto addEndpoint = [&id](GtObject& owner) {
        return owner.addDynamicProperty(std::make_unique<GtStringProperty>(
            id, id, QString(), QStringLiteral("initial")));
    };
    ASSERT_TRUE(addEndpoint(*source));
    ASSERT_TRUE(addEndpoint(*target));
    auto* connection = new GtPropertyConnection;
    connection->setSourceUuid(source->uuid());
    connection->setTargetUuid(target->uuid());
    connection->setSourceProp(id);
    connection->setTargetProp(id);
    ASSERT_TRUE(root.appendChild(connection));
    connection->makeConnection();
    QPointer<GtPropertyConnection> guardedConnection(connection);
    ASSERT_TRUE(connection->isConnected());

    int transfers = 0;
    QObject::connect(connection, &GtPropertyConnection::triggerValueTransfer,
                     [&transfers]() { ++transfers; });
    for (auto* owner : {source, target})
    {
        ASSERT_TRUE(owner->removeDynamicProperty(id));
        EXPECT_FALSE(connection->isConnected());
        EXPECT_FALSE(target->findProperty(id) &&
                     target->findProperty(id)->isConnected());
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        ASSERT_TRUE(guardedConnection);

        ASSERT_TRUE(addEndpoint(*owner));
        ASSERT_TRUE(connection->isConnected());
        EXPECT_TRUE(target->findProperty(id)->isConnected());
        auto* sourceProperty =
            qobject_cast<GtStringProperty*>(source->findProperty(id));
        auto* targetProperty =
            qobject_cast<GtStringProperty*>(target->findProperty(id));
        ASSERT_NE(sourceProperty, nullptr);
        ASSERT_NE(targetProperty, nullptr);
        sourceProperty->setVal(QStringLiteral("after re-add"));
        EXPECT_EQ(targetProperty->getVal(), QStringLiteral("after re-add"));

        ASSERT_TRUE(
            owner->addDynamicProperty(std::make_unique<GtStringProperty>(
                QStringLiteral("unrelated"), QStringLiteral("Unrelated"))));
        QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
        ASSERT_TRUE(guardedConnection);
        const auto before = transfers;
        sourceProperty->setVal(QStringLiteral("after unrelated add"));
        EXPECT_EQ(transfers, before + 1);
        EXPECT_EQ(targetProperty->getVal(),
                  QStringLiteral("after unrelated add"));
    }
}

TEST_F(TestGtObject, configurationChangesReplaceConstructionInstance)
{
    PropertyFactoryRegistration propertyFactory(
        *gtObjectFactory, QStringLiteral("test.configurable"),
        ConfigurableStringProperty::staticMetaObject,
        [](const QString& id) { return new ConfigurableStringProperty(id); });
    ASSERT_TRUE(propertyFactory.isValid());

    GtObject object;
    object.setFactory(gtObjectFactory);
    auto property = std::make_unique<ConfigurableStringProperty>(
        QStringLiteral("configured"));
    QPointer<GtAbstractProperty> originalProperty(property.get());
    const gt::PropertyConfiguration originalConfiguration{
        {QStringLiteral("size"), 3}};
    ASSERT_TRUE(property->applyConfiguration(originalConfiguration));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    auto before = object.toMemento();

    auto definition = dynamicPropertyDefinitions(object).front();
    definition.configuration = {{QStringLiteral("size"), 8}};
    ASSERT_TRUE(updateDynamicPropertyDefinition(object, definition));
    auto after = object.toMemento();
    before.calculateHashes();
    after.calculateHashes();
    EXPECT_EQ(before.propertyHash(), after.propertyHash());
    EXPECT_NE(before.fullHash(), after.fullHash());

    auto* replacement = object.findProperty(QStringLiteral("configured"));
    ASSERT_NE(replacement, nullptr);
    EXPECT_FALSE(originalProperty);
    EXPECT_EQ(replacement->configuration(), definition.configuration);

    GtObjectMementoDiff configurationDiff(before, after);
    ASSERT_TRUE(object.revertDiff(configurationDiff));
    EXPECT_EQ(dynamicPropertyDefinitions(object).front().configuration,
              originalConfiguration);
    ASSERT_TRUE(object.applyDiff(configurationDiff));
    EXPECT_EQ(dynamicPropertyDefinitions(object).front().configuration,
              definition.configuration);
}

TEST_F(TestGtObject, unknownDynamicPropertyTypeBecomesDummyOnMerge)
{
    GtObject object;
    object.setFactory(gtObjectFactory);
    object.setObjectName(QStringLiteral("original name"));
    const auto originalUuid = object.uuid();
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtime"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("value"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    auto invalid = object.toMemento();
    invalid.setUuid(QStringLiteral("replacement uuid"));
    invalid.setIdent(QStringLiteral("replacement name"));
    invalid.dynamicProperties.front().typeId = QStringLiteral("missing.type");
    EXPECT_TRUE(invalid.mergeTo(object, *gtObjectFactory));
    EXPECT_EQ(object.uuid(), QStringLiteral("replacement uuid"));
    EXPECT_EQ(object.objectName(), QStringLiteral("replacement name"));
    auto* placeholder = qobject_cast<GtDummyProperty*>(
        object.findProperty(QStringLiteral("runtime")));
    ASSERT_NE(placeholder, nullptr);
    EXPECT_EQ(placeholder->originalDefinition().typeId,
              QStringLiteral("missing.type"));
}

TEST_F(TestGtObject, unavailableDynamicStructKeepsChildDataAcrossResave)
{
    const auto typeId = QStringLiteral("test.unavailable_struct");
    auto makeFactory = []() {
        return gt::PropertyFactoryFunction([](const QString& id) {
            GtPropertyStructDefinition definition(QStringLiteral("TestStruct"));
            definition.defineMember(
                QStringLiteral("childValue"), [](const QString& childId) {
                    return new GtStringProperty(childId, childId);
                });
            return definition.newInstance(id).release();
        });
    };
    PropertyFactoryRegistration registration(
        *gtObjectFactory, typeId, GtPropertyStructInstance::staticMetaObject,
        makeFactory());
    ASSERT_TRUE(registration.isValid());

    auto structProperty = gt::createProperty(*gtObjectFactory, typeId,
                                             QStringLiteral("unknownStruct"));
    ASSERT_NE(structProperty, nullptr);
    auto* child = structProperty->findProperty(QStringLiteral("childValue"));
    ASSERT_NE(child, nullptr);
    ASSERT_TRUE(child->setValueFromVariant(QStringLiteral("nested value")));
    ASSERT_TRUE(obj.addDynamicProperty(std::move(structProperty)));
    const auto bytes = obj.toMemento().toByteArray();

    ASSERT_TRUE(gtObjectFactory->unregisterPropertyFactory(typeId));
    auto unavailable = GtObjectMemento(bytes).toObject(*gtObjectFactory);
    ASSERT_NE(unavailable, nullptr);
    auto* dummy = qobject_cast<GtDummyProperty*>(
        unavailable->findProperty(QStringLiteral("unknownStruct")));
    ASSERT_NE(dummy, nullptr);
    ASSERT_FALSE(
        dummy->originalDefinition().propertyData.childProperties.isEmpty());
    const auto resavedBytes = unavailable->toMemento().toByteArray();

    ASSERT_TRUE(gtObjectFactory->registerPropertyFactory(
        typeId, GtPropertyStructInstance::staticMetaObject, makeFactory()));
    auto recovered = GtObjectMemento(resavedBytes).toObject(*gtObjectFactory);
    ASSERT_NE(recovered, nullptr);
    auto* restoredRoot =
        recovered->findProperty(QStringLiteral("unknownStruct"));
    ASSERT_NE(restoredRoot, nullptr);
    auto* restoredChild =
        restoredRoot->findProperty(QStringLiteral("childValue"));
    ASSERT_NE(restoredChild, nullptr);
    EXPECT_EQ(restoredChild->valueToVariant().toString(),
              QStringLiteral("nested value"));
}

TEST_F(TestGtObject, dynamicMergePreservesStaticModeProperties)
{
    DynamicPropertyTestObject object;
    GtModeProperty mode(QStringLiteral("mode"), QStringLiteral("Mode"), {});
    GtModeTypeProperty first(QStringLiteral("First"), {});
    GtModeTypeProperty second(QStringLiteral("Second"), {});
    GtStringProperty nested(QStringLiteral("reserved"),
                            QStringLiteral("Reserved"));
    first.registerSubProperty(nested);
    mode.registerSubProperty(first);
    mode.registerSubProperty(second);
    ASSERT_TRUE(first.ident().isEmpty());
    ASSERT_TRUE(second.ident().isEmpty());
    ASSERT_TRUE(object.addStaticProperty(mode));
    ASSERT_TRUE(object.addDynamicProperty(std::make_unique<GtStringProperty>(
        QStringLiteral("runtime"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("saved"))));
    const auto saved = GtObjectMemento(object.toMemento().toByteArray());
    ASSERT_TRUE(object.removeDynamicProperty(QStringLiteral("runtime")));
    ASSERT_TRUE(saved.mergeTo(object, *gtObjectFactory));
    EXPECT_EQ(object.findProperty(QStringLiteral("mode")), &mode);
    EXPECT_EQ(object.findProperty(QStringLiteral("reserved")), &nested);
    auto* property = object.findProperty(QStringLiteral("runtime"));
    ASSERT_NE(property, nullptr);
    EXPECT_EQ(property->valueToVariant().toString(), QStringLiteral("saved"));

    auto collision = saved;
    collision.dynamicProperties.front().ident = QStringLiteral("reserved");
    collision.dynamicProperties.front().propertyData.name =
        QStringLiteral("reserved");
    EXPECT_FALSE(collision.mergeTo(object, *gtObjectFactory));
    EXPECT_EQ(object.findProperty(QStringLiteral("runtime")), property);
    EXPECT_EQ(object.findProperty(QStringLiteral("reserved")), &nested);
}

TEST_F(TestGtObject, dynamicMergeRejectsNestedIdsSharedByDifferentRoots)
{
    const QString typeId = QStringLiteral("test.colliding_child");
    PropertyFactoryRegistration registration(
        *gtObjectFactory, typeId, StringPropertyWithIntChild::staticMetaObject,
        [](const QString& id) { return new StringPropertyWithIntChild(id); });
    ASSERT_TRUE(registration.isValid());
    ASSERT_TRUE(obj.addDynamicProperty(
        std::make_unique<StringPropertyWithIntChild>(QStringLiteral("first"))));
    auto conflicting = obj.toMemento();
    auto duplicate = conflicting.dynamicProperties.front();
    duplicate.ident = QStringLiteral("second");
    duplicate.propertyData.name = duplicate.ident;
    conflicting.dynamicProperties.push_back(duplicate);
    QPointer<GtAbstractProperty> original(
        obj.findProperty(QStringLiteral("first")));
    EXPECT_FALSE(conflicting.mergeTo(obj, *gtObjectFactory));
    EXPECT_TRUE(original);
    EXPECT_EQ(obj.findProperty(QStringLiteral("first")), original.data());
    EXPECT_EQ(obj.findProperty(QStringLiteral("second")), nullptr);
}

TEST_F(TestGtObject, dynamicAddDiffCannotOverwriteStaticProperty)
{
    DynamicPropertyTestObject source;
    DynamicPropertyTestObject target;
    source.setUuid(QStringLiteral("same-object"));
    target.setUuid(QStringLiteral("same-object"));
    const auto before = source.toMemento(true);
    auto dynamicProperty = std::make_unique<GtStringProperty>(
        QStringLiteral("collision"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("runtime value"));
    ASSERT_TRUE(source.addDynamicProperty(std::move(dynamicProperty)));
    const auto after = source.toMemento(true);

    GtStringProperty staticProperty(QStringLiteral("collision"),
                                    QStringLiteral("Static"), QString(),
                                    QStringLiteral("static value"));
    ASSERT_TRUE(target.addStaticProperty(staticProperty));
    GtObjectMementoDiff addDiff(before, after);
    EXPECT_FALSE(target.applyDiff(addDiff));
    EXPECT_EQ(staticProperty.getVal(), QStringLiteral("static value"));
    EXPECT_EQ(dynamicPropertyDefinitions(target).size(), 0);
}

TEST_F(TestGtObject, invalidDynamicDiffValueLeavesLivePropertyUnchanged)
{
    DynamicPropertyTestObject source;
    DynamicPropertyTestObject target;
    source.setUuid(QStringLiteral("same-invalid-diff-object"));
    target.setUuid(QStringLiteral("same-invalid-diff-object"));

    auto makeProperty = []() {
        return std::make_unique<GtIntProperty>(QStringLiteral("runtimeValue"),
                                               QStringLiteral("Runtime"),
                                               QString(), 3);
    };
    ASSERT_TRUE(source.addDynamicProperty(makeProperty()));
    ASSERT_TRUE(target.addDynamicProperty(makeProperty()));
    auto* originalProperty =
        target.findProperty(QStringLiteral("runtimeValue"));
    ASSERT_NE(originalProperty, nullptr);

    const auto before = source.toMemento(true);
    qobject_cast<GtIntProperty*>(
        source.findProperty(QStringLiteral("runtimeValue")))
        ->setVal(4);
    auto after = source.toMemento(true);
    after.dynamicProperties.front().propertyData.setData(
        QStringLiteral("not an integer"));
    GtObjectMementoDiff diff(before, after);

    EXPECT_FALSE(target.applyDiff(diff));
    EXPECT_EQ(target.findProperty(QStringLiteral("runtimeValue")),
              originalProperty);
    EXPECT_EQ(qobject_cast<GtIntProperty*>(originalProperty)->getVal(), 3);
    ASSERT_EQ(dynamicPropertyDefinitions(target).size(), 1);
    EXPECT_EQ(dynamicPropertyDefinitions(target).front().ident,
              QStringLiteral("runtimeValue"));
}

TEST_F(TestGtObject, dynamicOnlyDiffPreservesConcurrentStaticValue)
{
    DynamicPropertyTestObject source;
    DynamicPropertyTestObject target;
    source.setUuid(QStringLiteral("same-concurrent-object"));
    target.setUuid(QStringLiteral("same-concurrent-object"));
    auto sourceConcurrent = std::make_unique<GtStringProperty>(
        QStringLiteral("independentDynamic"), QStringLiteral("Independent"),
        QString(), QStringLiteral("snapshot dynamic"));
    auto targetConcurrent = std::make_unique<GtStringProperty>(
        QStringLiteral("independentDynamic"), QStringLiteral("Independent"),
        QString(), QStringLiteral("snapshot dynamic"));
    auto* targetConcurrentRaw = targetConcurrent.get();
    ASSERT_TRUE(source.addDynamicProperty(std::move(sourceConcurrent)));
    ASSERT_TRUE(target.addDynamicProperty(std::move(targetConcurrent)));
    GtStringProperty sourceStatic(QStringLiteral("staticValue"),
                                  QStringLiteral("Static"), QString(),
                                  QStringLiteral("snapshot"));
    GtStringProperty targetStatic(QStringLiteral("staticValue"),
                                  QStringLiteral("Static"), QString(),
                                  QStringLiteral("snapshot"));
    ASSERT_TRUE(source.addStaticProperty(sourceStatic));
    ASSERT_TRUE(target.addStaticProperty(targetStatic));

    const auto before = source.toMemento(true);
    auto dynamic = std::make_unique<GtStringProperty>(
        QStringLiteral("runtime"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("new dynamic value"));
    ASSERT_TRUE(source.addDynamicProperty(std::move(dynamic)));
    const auto after = source.toMemento(true);
    targetStatic.setVal(QStringLiteral("concurrent edit"));
    targetConcurrentRaw->setVal(QStringLiteral("concurrent dynamic edit"));

    GtObjectMementoDiff diff(before, after);
    ASSERT_TRUE(target.applyDiff(diff));
    EXPECT_EQ(targetStatic.getVal(), QStringLiteral("concurrent edit"));
    EXPECT_EQ(targetConcurrentRaw->getVal(),
              QStringLiteral("concurrent dynamic edit"));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(
                  target.findProperty(QStringLiteral("runtime")))
                  ->getVal(),
              QStringLiteral("new dynamic value"));
    ASSERT_TRUE(target.revertDiff(diff));
    EXPECT_EQ(targetConcurrentRaw->getVal(),
              QStringLiteral("concurrent dynamic edit"));
}

TEST_F(TestGtObject, invalidDynamicDefinitionMergeIsAtomic)
{
    GtObject object;
    object.setFactory(gtObjectFactory);
    object.setObjectName(QStringLiteral("original"));
    const auto uuid = object.uuid();
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtime"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("original value"));
    auto* originalProperty = property.get();
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));

    auto invalid = object.toMemento();
    invalid.setUuid(QStringLiteral("new uuid"));
    invalid.setIdent(QStringLiteral("new name"));
    invalid.dynamicProperties.push_back(invalid.dynamicProperties.front());
    EXPECT_FALSE(invalid.mergeTo(object, *gtObjectFactory));
    EXPECT_EQ(object.uuid(), uuid);
    EXPECT_EQ(object.objectName(), QStringLiteral("original"));
    EXPECT_EQ(object.findProperty(QStringLiteral("runtime")), originalProperty);
    EXPECT_EQ(qobject_cast<GtStringProperty*>(originalProperty)->getVal(),
              QStringLiteral("original value"));
}

TEST_F(TestGtObject, incompatibleDynamicValueMementoFailsAtomically)
{
    GtObject object;
    object.setFactory(gtObjectFactory);
    auto property = std::make_unique<GtDoubleProperty>(
        QStringLiteral("boundedRuntime"), QStringLiteral("Bounded"), QString(),
        GtUnit::Default, gt::Boundaries<double>::makeNormalized(0.0, 10.0),
        5.0);
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    auto* original = object.findProperty(QStringLiteral("boundedRuntime"));
    const auto originalUuid = object.uuid();

    auto invalid = object.toMemento();
    invalid.dynamicProperties.front().propertyData.setData(12.0);
    const auto xmlRoundTrip = GtObjectMemento(invalid.toByteArray());
    EXPECT_FALSE(xmlRoundTrip.mergeTo(object, *gtObjectFactory));
    EXPECT_EQ(object.uuid(), originalUuid);
    EXPECT_EQ(object.findProperty(QStringLiteral("boundedRuntime")), original);
    EXPECT_DOUBLE_EQ(qobject_cast<GtDoubleProperty*>(original)->getVal(), 5.0);
    EXPECT_EQ(xmlRoundTrip.toObject(*gtObjectFactory), nullptr);
}

TEST_F(TestGtObject, invalidChildDynamicValueDoesNotMutateParentOrSibling)
{
    GtObjectGroup parent;
    parent.setFactory(gtObjectFactory);
    auto* invalidChild = new GtObjectGroup;
    invalidChild->setFactory(gtObjectFactory);
    auto invalidProperty = std::make_unique<GtDoubleProperty>(
        QStringLiteral("childBounded"), QStringLiteral("Bounded"), QString(),
        GtUnit::Default, gt::Boundaries<double>::makeNormalized(0.0, 10.0),
        5.0);
    ASSERT_TRUE(invalidChild->addDynamicProperty(std::move(invalidProperty)));
    ASSERT_TRUE(parent.appendChild(invalidChild));

    auto* sibling = new GtObjectGroup;
    sibling->setFactory(gtObjectFactory);
    sibling->setObjectName(QStringLiteral("unchanged sibling"));
    ASSERT_TRUE(parent.appendChild(sibling));

    auto invalid = parent.toMemento();
    auto childMemento =
        std::find_if(invalid.childObjects.begin(), invalid.childObjects.end(),
                     [invalidChild](const GtObjectMemento& child) {
                         return child.uuid() == invalidChild->uuid();
                     });
    ASSERT_NE(childMemento, invalid.childObjects.end());
    childMemento->dynamicProperties.front().propertyData.setData(12.0);

    EXPECT_FALSE(invalid.mergeTo(parent, *gtObjectFactory));
    EXPECT_TRUE(parent.findDirectChildren<GtObject*>().contains(sibling));
    EXPECT_EQ(sibling->objectName(), QStringLiteral("unchanged sibling"));
    EXPECT_TRUE(parent.findDirectChildren<GtObject*>().contains(invalidChild));
    EXPECT_DOUBLE_EQ(
        qobject_cast<GtDoubleProperty*>(
            invalidChild->findProperty(QStringLiteral("childBounded")))
            ->getVal(),
        5.0);
}

TEST_F(TestGtObject, dynamicPropertyDiffDoesNotRequireObjectFactory)
{
    GtObject source;
    source.setFactory(gtObjectFactory);
    source.setUuid(QStringLiteral("dynamic_diff_object"));
    const auto before = source.toMemento(true);
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtime"), QStringLiteral("Runtime"), QString(),
        QStringLiteral("value"));
    ASSERT_TRUE(source.addDynamicProperty(std::move(property)));
    const auto after = source.toMemento(true);
    GtObjectMementoDiff diff(before, after);

    GtObject target;
    target.setUuid(source.uuid());
    ASSERT_TRUE(target.revertDiff(diff));
    EXPECT_EQ(target.findProperty(QStringLiteral("runtime")), nullptr);
    ASSERT_TRUE(target.applyDiff(diff));
    auto* restored = qobject_cast<GtStringProperty*>(
        target.findProperty(QStringLiteral("runtime")));
    ASSERT_NE(restored, nullptr);
    EXPECT_EQ(restored->getVal(), QStringLiteral("value"));
}

TEST_F(TestGtObject, fromMemento)
{
    qDebug() << "#### TEST_F(TestGtObject, fromMemento)";

    gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);

    // with properties
    TestSpecialGtObject op1;

    op1.setBool(true);
    op1.setDouble(1.0);
    op1.setFile("bla");
    op1.setInt(20);
    op1.setLabel("lab");
    op1.setLink("link");
    op1.setString("str");
    QVector<double> doubleVec;
    doubleVec << 0.1 << 0.2 << 0.3;
    op1.setDoubleVec(doubleVec);

    GtObjectMemento mp1 = op1.toMemento();
    ASSERT_FALSE(mp1.isNull());
}


TEST_F(TestGtObject, copy)
{
    bool signalCalled = false;

    QObject guard;
    QObject::connect(&obj, &GtObject::uuidChanged, &guard,
                     [&](QString oldUuid) { signalCalled = true; });

    GtObject* myCopy = obj.copy();

    ASSERT_TRUE(myCopy != nullptr);
    ASSERT_TRUE(myCopy != &obj);
    ASSERT_FALSE(signalCalled);

    // check that uuid are not equal
    ASSERT_NE(myCopy->uuid(), obj.uuid());

    delete myCopy;

    GtObject origWithoutFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);

    GtObject* copiedWithDefaultFactory = origWithoutFactory.clone();

    ASSERT_TRUE(copiedWithDefaultFactory != nullptr);

    delete copiedWithDefaultFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);
}

TEST_F(TestGtObject, objectPath)
{
    GtObjectGroup* child1 = new GtObjectGroup(&obj);
    child1->setObjectName("Child1");

    GtObjectGroup* child2 = new GtObjectGroup(child1);
    child2->setObjectName("Child2");

    QString path = child2->objectPath();
    QString pathCompare = obj.objectName() + ";" + child1->objectName() + ";" +
                          child2->objectName();

    // check that uuid are not equal
    ASSERT_STREQ(path.toStdString().c_str(), pathCompare.toStdString().c_str());
}

TEST_F(TestGtObject, getObjectByPath)
{
    GtObjectGroup* child1 = new GtObjectGroup(&obj);
    child1->setObjectName("Child1");

    GtObjectGroup* child2 = new GtObjectGroup(&obj);
    child2->setObjectName("Child2");

    GtObjectGroup* child3 = new GtObjectGroup(child1);
    child3->setObjectName("Child3");

    QString path = obj.objectName() + ";" + child1->objectName() + ";" +
                   child3->objectName();

    qDebug() << path;

    GtObject* fo = obj.getObjectByPath(path);

    ASSERT_TRUE(fo != nullptr);
    ASSERT_TRUE(fo == child3);
}

TEST_F(TestGtObject, uuidChanged)
{
    bool signalCalled = false;
    QString emittedUuid;

    QObject guard;
    QObject::connect(&obj, &GtObject::uuidChanged, &guard,
                     [&](QString oldUuid) {
                         signalCalled = true;
                         emittedUuid = oldUuid;
                     });

    obj.setUuid(obj.uuid());
    ASSERT_FALSE(signalCalled);

    QString oldUuid = obj.uuid();
    obj.newUuid();
    QString newUuid = obj.uuid();

    EXPECT_TRUE(signalCalled);
    EXPECT_EQ(emittedUuid, oldUuid);
    EXPECT_NE(newUuid, oldUuid);

    signalCalled = false;
    emittedUuid = QString{};

    obj.setUuid(oldUuid);

    EXPECT_TRUE(signalCalled);
    EXPECT_EQ(emittedUuid, newUuid);
    EXPECT_EQ(obj.uuid(), oldUuid);
}

TEST_F(TestGtObject, clone)
{
    bool signalCalled = false;

    QObject guard;
    QObject::connect(&obj, &GtObject::uuidChanged, &guard,
                     [&](QString oldUuid) { signalCalled = true; });

    GtObject* cloned = obj.clone();

    ASSERT_TRUE(cloned != nullptr);
    EXPECT_FALSE(signalCalled);

    // check that uuid are equal
    ASSERT_EQ(cloned->uuid(), obj.uuid());

    delete cloned;

    GtObject origWithoutFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);

    GtObject* clonedWithDefaultFactory = origWithoutFactory.clone();

    ASSERT_TRUE(clonedWithDefaultFactory != nullptr);

    delete clonedWithDefaultFactory;

    ASSERT_TRUE(origWithoutFactory.factory() == nullptr);
}

TEST_F(TestGtObject, appendChild)
{
    GtObject* child = new GtObject;
    TestSpecialGtObject parent;

    ASSERT_FALSE(obj.appendChild(nullptr));

    ASSERT_TRUE(obj.appendChild(child));
    ASSERT_FALSE(parent.appendChild(child));
}

#include "test_gt_object.moc"

TEST_F(TestGtObject, flags)
{
    GtObject::ObjectFlags f = obj.objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);
}

TEST_F(TestGtObject, acceptChanges)
{
    GtObject::ObjectFlags f = obj.objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);

    obj.acceptChanges();

    f = obj.objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);
}

TEST_F(TestGtObject, insertChild)
{
    QPointer<GtObject> parentObject = new GtObject;

    QPointer<GtObject> child1 = new GtObject;
    QPointer<GtObject> child2 = new GtObject;
    QPointer<GtObject> child3 = new GtObject;
    QPointer<GtObject> child4 = new GtObject;
    QPointer<GtObject> child5 = new GtObject;

    parentObject->appendChild(child1);
    parentObject->appendChild(child2);
    parentObject->appendChild(child3);
    parentObject->appendChild(child4);
    parentObject->appendChild(child5);

    parentObject->acceptChangesRecursively();

    QPointer<GtObject> objectToInsert = new GtObject;

    parentObject->insertChild(2, objectToInsert.data());
    QList<GtObject*> children = parentObject->findDirectChildren<GtObject*>();

    ASSERT_EQ(children.size(), 6);

    ASSERT_FALSE(children.at(2) == child3);
    ASSERT_FALSE(children.at(3) == child4);
    ASSERT_FALSE(children.at(4) == child5);

    ASSERT_TRUE(children.at(0) == child1);
    ASSERT_TRUE(children.at(1) == child2);
    ASSERT_TRUE(children.at(2) == objectToInsert);
    ASSERT_TRUE(children.at(3) == child3);
    ASSERT_TRUE(children.at(4) == child4);
    ASSERT_TRUE(children.at(5) == child5);


    GtObject::ObjectFlags f = parentObject->objectFlags();

    ASSERT_TRUE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);

    f = objectToInsert->objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_TRUE(f & GtObject::NewlyCreated);

    f = child3->objectFlags();

    ASSERT_FALSE(f & GtObject::HasOwnChanges);
    ASSERT_FALSE(f & GtObject::HasChildChanges);
    ASSERT_FALSE(f & GtObject::NewlyCreated);

    delete parentObject;
}

TEST_F(TestGtObject, isDerivedFromClass)
{
    GtLabelData label;
    auto* p = &label;
    auto* m = p->metaObject();
    ASSERT_TRUE(m);

    /// test with empty classname
    ASSERT_FALSE(gt::isDerivedFromClass(p, ""));
    ASSERT_FALSE(gt::isDerivedFromClass(m, ""));
    /// check valid result
    ASSERT_TRUE(gt::isDerivedFromClass(p, GT_CLASSNAME(GtObjectGroup)));
    ASSERT_TRUE(gt::isDerivedFromClass(m, GT_CLASSNAME(GtObjectGroup)));
    /// check for wrong superclass
    ASSERT_FALSE(gt::isDerivedFromClass(p, "GtCalculator"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "GtCalculator"));
    /// check for its own class
    ASSERT_FALSE(gt::isDerivedFromClass(p, "GtLabelData"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "GtLabelData"));
    /// check if object is derived from GtObject
    ASSERT_TRUE(gt::isDerivedFromClass(p, "GtObject"));
    ASSERT_TRUE(gt::isDerivedFromClass(m, "GtObject"));
    /// check if function stops before QObject
    ASSERT_FALSE(gt::isDerivedFromClass(p, "QObject"));
    ASSERT_FALSE(gt::isDerivedFromClass(m, "QObject"));
}

TEST_F(TestGtObject, findParent)
{
    TestObject obj;
    obj.setObjectName("Bla");
    EXPECT_EQ(obj.findParent(), nullptr);

    auto* child = new GtObject;
    obj.appendChild(child);

    EXPECT_EQ(obj.findParent(), nullptr);
    EXPECT_EQ(child->findParent<TestObject*>(), &obj);
    EXPECT_EQ(child->findParent<GtObjectGroup*>(), nullptr);
    EXPECT_EQ(child->findParent<QObject*>("Bla"), &obj);
}

TEST_F(TestGtObject, findRoot)
{
    TestObject obj;
    EXPECT_EQ(obj.findParent(), nullptr);

    auto* child = new GtObject;
    obj.appendChild(child);

    auto* childchild = new GtObject;
    child->appendChild(childchild);

    EXPECT_EQ(obj.findRoot(), nullptr);
    EXPECT_EQ(obj.findRoot(&obj), &obj);
    EXPECT_EQ(childchild->findRoot<TestObject*>(), &obj);
    EXPECT_EQ(childchild->findRoot<GtObjectGroup*>(), nullptr);
    EXPECT_EQ(childchild->findRoot<QObject*>(), &obj);
}

TEST_F(TestGtObject, constness)
{
    GtObject* obj{};
    GtObject const* constObj{};

    static_assert(std::is_same<GtObject*, decltype(obj->findChild())>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtObject const*, decltype(constObj->findChild())>(),
        "Should be const");

    static_assert(std::is_same<GtObject*, decltype(obj->findDirectChild())>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtObject const*, decltype(constObj->findDirectChild())>(),
        "Should be const");

    static_assert(std::is_same<QObject*, decltype(obj->parent())>(),
                  "Should be non-const");
    static_assert(std::is_same<QObject const*, decltype(constObj->parent())>(),
                  "Should be const");

    static_assert(std::is_same<GtObject*, decltype(obj->findParent())>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtObject const*, decltype(constObj->findParent())>(),
        "Should be const");

    static_assert(std::is_same<GtObject*, decltype(obj->findRoot())>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtObject const*, decltype(constObj->findRoot())>(),
        "Should be const");

    static_assert(
        std::is_same<GtAbstractProperty*, decltype(obj->findProperty("id"))>(),
        "Should be non-const");
    static_assert(std::is_same<GtAbstractProperty const*,
                               decltype(constObj->findProperty("id"))>(),
                  "Should be const");

    static_assert(std::is_same<GtAbstractProperty*,
                               decltype(obj->findPropertyByName("name"))>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtAbstractProperty const*,
                     decltype(constObj->findPropertyByName("name"))>(),
        "Should be const");

    static_assert(std::is_same<GtPropertyStructContainer*,
                               decltype(obj->findPropertyContainer("id"))>(),
                  "Should be non-const");
    static_assert(
        std::is_same<GtPropertyStructContainer const*,
                     decltype(constObj->findPropertyContainer("id"))>(),
        "Should be const");

    static_assert(
        std::is_same<QList<GtObject*>, decltype(obj->findChildren())>(),
        "Should be non-const");
    static_assert(std::is_same<QList<GtObject const*>,
                               decltype(constObj->findChildren())>(),
                  "Should be non-const");

    static_assert(
        std::is_same<QList<GtObject*>, decltype(obj->findDirectChildren())>(),
        "Should be non-const");
    static_assert(std::is_same<QList<GtObject const*>,
                               decltype(constObj->findDirectChildren())>(),
                  "Should be non-const");
}

TEST(GtObjectBugs, issue276)
{
    TestSpecialGtObject testobj;
    QPointer<GtObject> child1 = new TestSpecialGtObject;
    QPointer<GtObject> child2 = new TestSpecialGtObject;
    EXPECT_TRUE(testobj.appendChild(child1));
    EXPECT_TRUE(testobj.appendChild(child2));

    // uncommenting this line returns the correct no. of children but deletes
    // all children in the process
    // -> now deactivatd using SFINAE
    //    EXPECT_EQ(2, testobj.childCount<GtObject>());

    EXPECT_EQ(2, testobj.childCount<GtObject*>());

    EXPECT_TRUE(child1 != nullptr);
    EXPECT_TRUE(child2 != nullptr);
}

TEST(GtObjectBugs, issue325)
{
    TestSpecialGtObject obj;
    obj.acceptChanges();

    bool propertyChanged = false;

    SlotAdaptor a;
    a.on(&obj, SIGNAL(dataChanged(GtObject*, GtAbstractProperty*)),
         [&propertyChanged]() { propertyChanged = true; });

    ASSERT_FALSE(obj.hasChanges());

    EXPECT_FALSE(propertyChanged);
    obj.setDouble(23456.);
    EXPECT_TRUE(propertyChanged);
    EXPECT_TRUE(obj.hasChanges());
}

/*
 * This test checks, that a signal change from a sub
 * property is propagated correctly
 *
 * This was initially not the case, as the thread switching was not properly
 * propagated to all childs
 */
TEST(GtObjectBugs, subPropsSignalsFromThread)
{
    std::unique_ptr<TestSpecialGtObject> obj;
    QtConcurrent::run([&obj, mainThread = QThread::currentThread()]() {
        obj.reset(new TestSpecialGtObject);

        // note, we may not use QObject->moveToThread here!!!
        gt::moveToThread(*obj, mainThread);
    }).waitForFinished();

    bool changed = false;
    QObject::connect(&(obj->m_modeProp), &GtAbstractProperty::subPropChanged,
                     [&changed](GtAbstractProperty*) { changed = true; });

    ASSERT_TRUE(obj != nullptr);
    EXPECT_FALSE(changed);

    obj->m_modeTypeProp.setVal("a new value");
    EXPECT_TRUE(changed);
}


/**
 * In bug 539, the number of child objects kept increasing
 * if they were unnamed
 */
TEST_F(TestGtObject, bug539)
{
    gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);
    gtObjectFactory->registerClass(TestObject::staticMetaObject);

    // with properties
    TestSpecialGtObject parent;
    parent.setFactory(gtObjectFactory);

    auto o1 = new TestObject;
    o1->setObjectName("");
    parent.appendChild(o1);

    auto o2 = new TestSpecialGtObject;
    o2->setObjectName("");
    parent.appendChild(o2);

    ASSERT_EQ(parent.findDirectChildren<TestObject*>().size(), 1);
    ASSERT_EQ(parent.findDirectChildren<TestSpecialGtObject*>().size(), 1);

    // merge back its data into itself, it should not change its layout
    auto memento = parent.toMemento();
    parent.fromMemento(memento);

    EXPECT_EQ(parent.findDirectChildren<TestObject*>().size(), 1);
    EXPECT_EQ(parent.findDirectChildren<TestSpecialGtObject*>().size(), 1);
}
