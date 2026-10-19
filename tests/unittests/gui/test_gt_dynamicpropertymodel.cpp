/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "property/gt_dummyproperty.h"
#include "gt_objectmementodiff.h"
#include "gt_propertymodel.h"
#include "gt_stringproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

class EditorRecoveryStringProperty : public GtStringProperty
{
    Q_OBJECT
public:
    explicit EditorRecoveryStringProperty(const QString& id) :
        GtStringProperty(id, id)
    {
    }
};

class DynamicContainerObject : public GtObjectGroup
{
public:
    bool registerContainer(GtPropertyStructContainer& container)
    {
        return registerPropertyStructContainer(container);
    }
};

using DynamicPropertyData = GtObjectMemento::DynamicPropertyData;

static bool
restoreDynamicProperties(GtObject& object,
                         const QVector<DynamicPropertyData>& definitions)
{
    auto memento = object.toMemento();
    memento.dynamicProperties = definitions;
    return memento.mergeDynamicPropertiesTo(object, object.factory());
}

TEST(TestGtDynamicPropertyModel, updatesAfterRuntimeAddAndRemove)
{
    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    GtPropertyModel model(&object);
    model.setObject(&object);
    int resetCount = 0;
    QObject::connect(&model, &QAbstractItemModel::modelReset,
                     [&resetCount]() { ++resetCount; });
    bool aboutToChangeSawOldProperties = false;
    int aboutToChangeCount = 0;
    QObject::connect(
        &object, &GtObject::dynamicPropertyStructureAboutToChange, [&]() {
            if (aboutToChangeCount++ == 0)
                aboutToChangeSawOldProperties = object.properties().isEmpty();
        });

    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("runtimeEditorValue"), QStringLiteral("Runtime value"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    EXPECT_TRUE(aboutToChangeSawOldProperties);
    EXPECT_EQ(model.rowCount(), 1);
    EXPECT_EQ(resetCount, 1);
    const auto categoryIndex = model.index(0, 0);
    const auto valueIndex = model.index(0, 2, categoryIndex);
    ASSERT_TRUE(valueIndex.isValid());
    ASSERT_TRUE(model.setData(valueIndex, QStringLiteral("edited in model"),
                              Qt::EditRole));
    EXPECT_EQ(qobject_cast<GtStringProperty*>(
                  object.findProperty(QStringLiteral("runtimeEditorValue")))
                  ->getVal(),
              QStringLiteral("edited in model"));

    const auto saved = object.toMemento();
    auto reloaded = saved.toObject(*gtObjectFactory);
    ASSERT_NE(reloaded, nullptr);
    EXPECT_EQ(qobject_cast<GtStringProperty*>(
                  reloaded->findProperty(QStringLiteral("runtimeEditorValue")))
                  ->getVal(),
              QStringLiteral("edited in model"));

    ASSERT_TRUE(
        object.removeDynamicProperty(QStringLiteral("runtimeEditorValue")));
    EXPECT_EQ(model.rowCount(), 0);
    EXPECT_EQ(resetCount, 2);
    const auto removed = object.toMemento();
    GtObjectMementoDiff removeDiff(saved, removed);
    ASSERT_TRUE(object.revertDiff(removeDiff));
    EXPECT_EQ(model.rowCount(), 1);
    EXPECT_EQ(resetCount, 3);
    ASSERT_TRUE(object.applyDiff(removeDiff));
    EXPECT_EQ(model.rowCount(), 0);
    EXPECT_EQ(resetCount, 4);
}

TEST(TestGtDynamicPropertyModel, unavailablePropertyIsVisibleAndReadOnly)
{
    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    DynamicPropertyData definition;
    definition.typeId = QStringLiteral("test.not_loaded.property");
    definition.ident = QStringLiteral("unavailableValue");
    definition.name = QStringLiteral("Unavailable value");
    ASSERT_TRUE(restoreDynamicProperties(object, {definition}));

    GtPropertyModel model(&object);
    model.setObject(&object);
    const auto categoryIndex = model.index(0, 0);
    ASSERT_TRUE(categoryIndex.isValid());
    const auto propertyIndex = model.index(0, 0, categoryIndex);
    ASSERT_TRUE(propertyIndex.isValid());
    const auto valueIndex = model.index(0, 2, categoryIndex);
    ASSERT_TRUE(valueIndex.isValid());
    EXPECT_EQ(model.data(propertyIndex), QStringLiteral("Unavailable value"));
    EXPECT_EQ(model.data(valueIndex), QStringLiteral("Unavailable"));
    EXPECT_TRUE(model.data(valueIndex, GtPropertyModel::ReadOnlyRole).toBool());
    EXPECT_FALSE(model.flags(valueIndex) & Qt::ItemIsEditable);
}

TEST(TestGtDynamicPropertyModel, switchingThroughContainerKeepsObjectSignals)
{
    GtPropertyStructContainer container(QStringLiteral("testContainer"));
    GtPropertyStructDefinition definition(QStringLiteral("EditorEntry"));
    definition.defineMember(QStringLiteral("value"), gt::makeStringProperty());
    container.registerAllowedType(definition);

    DynamicContainerObject object;
    object.setFactory(gtObjectFactory);
    ASSERT_TRUE(object.registerContainer(container));
    GtPropertyModel model(&object);

    model.setObject(&object, container);
    auto& entry = container.newEntry(QStringLiteral("EditorEntry"));
    entry.setMemberVal(QStringLiteral("value"), QStringLiteral("entry"));
    EXPECT_EQ(model.rowCount(), 1);

    int resetCount = 0;
    QObject::connect(&model, &QAbstractItemModel::modelReset,
                     [&]() { ++resetCount; });
    auto whileContainer = std::make_unique<GtStringProperty>(
        QStringLiteral("whileContainer"), QStringLiteral("While container"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(whileContainer)));
    EXPECT_EQ(model.rowCount(), 1);
    EXPECT_EQ(resetCount, 0);

    model.setObject(&object);
    resetCount = 0;
    auto property = std::make_unique<GtStringProperty>(
        QStringLiteral("afterContainerSwitch"), QStringLiteral("After"));
    ASSERT_TRUE(object.addDynamicProperty(std::move(property)));
    ASSERT_EQ(model.rowCount(), 1);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 2);
    EXPECT_EQ(resetCount, 1);
    ASSERT_TRUE(
        object.removeDynamicProperty(QStringLiteral("afterContainerSwitch")));
    ASSERT_EQ(model.rowCount(), 1);
    EXPECT_EQ(model.rowCount(model.index(0, 0)), 1);
    EXPECT_EQ(resetCount, 2);

    model.setObject(&object, container);
    ASSERT_EQ(model.rowCount(), 1);
    container.removeEntry(container.begin());
    EXPECT_EQ(model.rowCount(), 0);
}

TEST(TestGtDynamicPropertyModel, dummyRecoveryResetsOpenEditorModel)
{
    const auto typeId = QStringLiteral("test.editor_recovery.property");
    GtObjectGroup object;
    object.setFactory(gtObjectFactory);
    DynamicPropertyData definition;
    definition.typeId = typeId;
    definition.ident = QStringLiteral("recoverableValue");
    definition.name = QStringLiteral("Recoverable value");
    definition.configuration =
        EditorRecoveryStringProperty(definition.ident).configuration();
    ASSERT_TRUE(restoreDynamicProperties(object, {definition}));

    GtPropertyModel model(&object);
    model.setObject(&object);
    int resetCount = 0;
    QObject::connect(&model, &QAbstractItemModel::modelReset,
                     [&resetCount]() { ++resetCount; });
    PropertyFactoryRegistration registration(
        *gtObjectFactory, typeId,
        EditorRecoveryStringProperty::staticMetaObject,
        [](const QString& id) { return new EditorRecoveryStringProperty(id); });
    ASSERT_TRUE(registration.isValid());
    ASSERT_TRUE(gtObjectFactory->hasPropertyFactory(typeId));
    ASSERT_NE(gt::createProperty(*gtObjectFactory, typeId, definition.ident),
              nullptr);
    ASSERT_TRUE(restoreDynamicProperties(object, {definition}));
    ASSERT_NE(qobject_cast<EditorRecoveryStringProperty*>(
                  object.findProperty(definition.ident)),
              nullptr);
    EXPECT_EQ(resetCount, 1);
    EXPECT_EQ(model.rowCount(), 1);
}

#include "test_gt_dynamicpropertymodel.moc"
