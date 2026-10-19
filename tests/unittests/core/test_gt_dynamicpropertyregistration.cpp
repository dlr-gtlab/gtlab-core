/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"
#include "gt_datamodelinterface.h"
#include "gt_coreapplication.h"
#include <QCoreApplication>
#include "gt_moduleinterface.h"
#include "gt_moduleloader.h"
#include "gt_objectfactory.h"
#include "gt_package.h"
#include "gt_stringproperty.h"

namespace
{
    class RuntimeData : public GtObject
    {
        Q_OBJECT
    public:
        Q_INVOKABLE RuntimeData() = default;
    };

    class OtherRuntimeData : public GtObject
    {
        Q_OBJECT
    public:
        Q_INVOKABLE OtherRuntimeData() = default;
    };

    class RuntimePackage : public GtPackage
    {
        Q_OBJECT
    public:
        Q_INVOKABLE RuntimePackage() = default;
    };

    class RuntimeStringProperty : public GtStringProperty
    {
    public:
        explicit RuntimeStringProperty(const QString& id) :
            GtStringProperty(id, id)
        {
        }
    };

    class RuntimeModule : public QObject,
                          public GtModuleInterface,
                          public GtDatamodelInterface
    {
        Q_OBJECT
        Q_INTERFACES(GtModuleInterface GtDatamodelInterface)
    public:
        QString ident() const override
        {
            return QStringLiteral("TestRuntimeModule");
        }
        GtVersionNumber version() override
        {
            return {1, 0, 0};
        }
        QString description() const override
        {
            return {};
        }
        QMetaObject package() override
        {
            return RuntimePackage::staticMetaObject;
        }
        QList<QMetaObject> data() override
        {
            return classes;
        }
        bool standAlone() override
        {
            return false;
        }
        QList<PropertyFactoryData> propertyFactories() const override
        {
            return factories;
        }

        QList<QMetaObject> classes{RuntimeData::staticMetaObject};
        QList<PropertyFactoryData> factories{
            {QStringLiteral("test.loader.runtime"),
             &GtStringProperty::staticMetaObject,
             [](const QString& id) { return new RuntimeStringProperty(id); }}};
    };

    class RuntimeModuleLoader : public GtModuleLoader
    {
    public:
        using GtModuleLoader::check;
        using GtModuleLoader::insert;
        bool rejectAdditional{false};

    protected:
        bool insertAdditional(GtModuleInterface* module) override
        {
            return !rejectAdditional &&
                   GtModuleLoader::insertAdditional(module);
        }
    };

    class RuntimeTestApplication : public GtCoreApplication
    {
    public:
        RuntimeTestApplication() :
            GtCoreApplication(QCoreApplication::instance(), AppMode::Batch)
        {
            init();
        }

    protected:
        bool initFirstRun() override
        {
            return true;
        }
    };

    class DynamicPropertyRegistration : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            application = std::make_unique<RuntimeTestApplication>();
            loader = std::make_unique<RuntimeModuleLoader>();
        }

        void TearDown() override
        {
            gtObjectFactory->unregisterPropertyFactory(
                module.factories.front().typeId, module.ident());
            gtObjectFactory->unregisterClass(RuntimeData::staticMetaObject);
            gtObjectFactory->unregisterClass(
                OtherRuntimeData::staticMetaObject);
            gtObjectFactory->unregisterClass(RuntimePackage::staticMetaObject);
            loader.reset();
            application.reset();
        }

        void expectUnpublished() const
        {
            EXPECT_TRUE(loader->moduleIds().isEmpty());
            EXPECT_FALSE(gtObjectFactory->knownClass(
                RuntimeData::staticMetaObject.className()));
            EXPECT_FALSE(gtObjectFactory->hasPropertyFactory(
                module.factories.front().typeId));
        }

        RuntimeModule module;
        std::unique_ptr<RuntimeTestApplication> application;
        std::unique_ptr<RuntimeModuleLoader> loader;
    };
} // namespace

TEST_F(DynamicPropertyRegistration, invalidPropertyFactoryRejectsModule)
{
    module.factories.front().factory = {};
    EXPECT_FALSE(loader->check(&module));
    EXPECT_FALSE(loader->insert(&module));
    expectUnpublished();
    EXPECT_FALSE(gtObjectFactory->knownClass(
        RuntimePackage::staticMetaObject.className()));
}

TEST_F(DynamicPropertyRegistration,
       classConflictAfterValidationRollsBackOnlyNewClasses)
{
    module.classes.push_back(OtherRuntimeData::staticMetaObject);
    ASSERT_TRUE(loader->check(&module));
    // Simulate a class registered between the preflight and actual insertion.
    ASSERT_TRUE(gtObjectFactory->registerClass(
        OtherRuntimeData::staticMetaObject, QStringLiteral("ExistingModule")));
    EXPECT_FALSE(loader->insert(&module));
    expectUnpublished();
    EXPECT_EQ(gtObjectFactory->moduleId(
                  OtherRuntimeData::staticMetaObject.className()),
              QStringLiteral("ExistingModule"));
}

TEST_F(DynamicPropertyRegistration,
       propertyConflictAfterValidationPreservesExistingRegistration)
{
    ASSERT_TRUE(loader->check(&module));
    PropertyFactoryRegistration existing(
        *gtObjectFactory, QStringLiteral("test.loader.existing"),
        GtStringProperty::staticMetaObject,
        [](const QString& id) { return new RuntimeStringProperty(id); });
    ASSERT_TRUE(existing.isValid());

    EXPECT_FALSE(loader->insert(&module));
    expectUnpublished();
    EXPECT_FALSE(gtObjectFactory->knownClass(
        RuntimePackage::staticMetaObject.className()));
    EXPECT_TRUE(gtObjectFactory->hasPropertyFactory(
        QStringLiteral("test.loader.existing")));
}

TEST_F(DynamicPropertyRegistration,
       additionalRegistrationFailureRollsBackClassesAndProperties)
{
    ASSERT_TRUE(loader->check(&module));
    loader->rejectAdditional = true;
    EXPECT_FALSE(loader->insert(&module));
    expectUnpublished();
    EXPECT_FALSE(gtObjectFactory->knownClass(
        RuntimePackage::staticMetaObject.className()));
}

TEST_F(DynamicPropertyRegistration, successfulRegistrationPublishesModule)
{
    ASSERT_TRUE(loader->check(&module));
    ASSERT_TRUE(loader->insert(&module));
    EXPECT_TRUE(loader->moduleIds().contains(module.ident()));
    EXPECT_TRUE(
        gtObjectFactory->knownClass(RuntimeData::staticMetaObject.className()));
    EXPECT_TRUE(gtObjectFactory->knownClass(
        RuntimePackage::staticMetaObject.className()));
    EXPECT_EQ(
        gtObjectFactory->propertyMetaObject(module.factories.front().typeId),
        &GtStringProperty::staticMetaObject);
}

#include "test_gt_dynamicpropertyregistration.moc"
