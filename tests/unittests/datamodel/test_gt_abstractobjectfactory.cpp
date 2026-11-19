/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_abstractobjectfactory.h"
#include "gt_factorygroup.h"
#include "gt_object.h"
#include "gt_objectgroup.h"
#include "gt_label.h"
#include "gt_labeldata.h"
#include "gt_task.h"

namespace
{

class TestObjectFactory : public GtAbstractObjectFactory
{
};

/// Factory that overrides the registration hook for a single class only
class SubclassOverrideFactory : public GtAbstractObjectFactory
{
public:
    bool registerClass(QMetaObject metaObj, const QString& moduleId) override
    {
        m_registered << metaObj.className();
        return GtAbstractObjectFactory::registerClass(metaObj, moduleId);
    }

    QStringList registered() const
    {
        return m_registered;
    }

private:
    QStringList m_registered;
};

class FactoryTestObject : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE FactoryTestObject() = default;
};

class FactoryTestOtherObject : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE FactoryTestOtherObject() = default;
};

} // namespace


TEST(GtAbstractObjectFactory, RegistersAndRemovesClassModuleId)
{
    TestObjectFactory factory;

    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject),
                                      QStringLiteral("TestModule")));
    EXPECT_TRUE(factory.knownClass(GT_CLASSNAME(GtObject)));
    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("TestModule"));

    ASSERT_TRUE(factory.unregisterClass(GT_METADATA(GtObject)));
    EXPECT_TRUE(factory.moduleId(GT_CLASSNAME(GtObject)).isEmpty());
}

TEST(GtAbstractObjectFactory, EmptyModuleIdIsNotAllowed)
{
    TestObjectFactory factory;

    ASSERT_FALSE(factory.registerClass(GT_METADATA(GtObject), QString{}));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtObject)));
    EXPECT_TRUE(factory.moduleId(GT_CLASSNAME(GtObject)).isEmpty());
}

TEST(GtAbstractObjectFactory, BulkRegistrationAssignsModuleId)
{
    TestObjectFactory factory;
    const QList<QMetaObject> classes{GT_METADATA(GtObject),
                                     GT_METADATA(GtObjectGroup)};

    ASSERT_TRUE(factory.registerClasses(classes, QStringLiteral("TestModule")));
    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("TestModule"));
    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObjectGroup)),
              QStringLiteral("TestModule"));
}

TEST(GtAbstractObjectFactory, DuplicateDoesNotReplaceModuleId)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject),
                                      QStringLiteral("FirstModule")));

    EXPECT_FALSE(factory.registerClass(GT_METADATA(GtObject),
                                       QStringLiteral("SecondModule")));
    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("FirstModule"));
}

TEST(GtAbstractObjectFactory, BulkRegistrationRejectsDuplicates)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject),
                                      QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.registerClasses({GT_METADATA(GtObject)},
                                         QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.registerClasses({GT_METADATA(GtObject)},
                                         factory.coreModuleId()));

    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("TestModule"));
}

// calling registerClasses calls `registerClass`
TEST(GtAbstractObjectFactory, BulkRegistrationUsesSubclassOverride)
{
    SubclassOverrideFactory factory;
    GtAbstractObjectFactory& base = factory;
    const QList<QMetaObject> classes{GT_METADATA(GtObject),
                                     GT_METADATA(GtObjectGroup)};

    ASSERT_TRUE(base.registerClasses(classes, QStringLiteral("TestModule")));

    const QStringList expected{GT_CLASSNAME(GtObject),
                               GT_CLASSNAME(GtObjectGroup)};
    EXPECT_EQ(factory.registered(), expected);
    EXPECT_EQ(base.moduleId(GT_CLASSNAME(GtObjectGroup)),
              QStringLiteral("TestModule"));
}

TEST(GtAbstractObjectFactory, ReregisteringKnownClassesFails)
{
    TestObjectFactory factory;

    EXPECT_TRUE(factory.registerClass(GT_METADATA(GtObject),
                                      QStringLiteral("TestModule")));
    EXPECT_TRUE(factory.knownClass(GT_CLASSNAME(GtObject)));

    // registerClasses does not register partially
    EXPECT_FALSE(factory.registerClasses(
        {GT_METADATA(GtObject), GT_METADATA(GtObjectGroup)},
        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtObjectGroup)));

    EXPECT_TRUE(factory.registerClasses(
        {GT_METADATA(GtLabel), GT_METADATA(GtLabelData)},
        QStringLiteral("TestModule")));
    EXPECT_TRUE(factory.knownClass(GT_CLASSNAME(GtLabel)));
    EXPECT_TRUE(factory.knownClass(GT_CLASSNAME(GtLabelData)));

    EXPECT_FALSE(
        factory.registerClasses({GT_METADATA(GtLabel), GT_METADATA(GtTask)},
                                QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtTask)));
}

TEST(GtAbstractObjectFactory, RegisteringDuplciateClassesFails)
{
    TestObjectFactory factory;

    // registerClasses does not register partially
    EXPECT_FALSE(factory.registerClasses(
        {GT_METADATA(GtObject), GT_METADATA(GtObjectGroup),
         GT_METADATA(GtLabel), GT_METADATA(GtLabelData),
         GT_METADATA(GtObjectGroup), GT_METADATA(GtTask)},
        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtObject)));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtLabel)));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtLabelData)));
    EXPECT_FALSE(factory.knownClass(GT_CLASSNAME(GtTask)));
}

TEST(GtAbstractObjectFactory, LegacyBulkRegistrationRejectsDuplicates)
{
    TestObjectFactory factory;

    ASSERT_TRUE(factory.registerClasses(
        {GT_METADATA(GtObject), GT_METADATA(GtObjectGroup)},
        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory.registerClasses({GT_METADATA(GtObjectGroup)},
                                         QStringLiteral("TestModule")));
    EXPECT_EQ(factory.knownClasses().size(), 2);
}

TEST(GtAbstractObjectFactory, InvokableDetection)
{
    TestObjectFactory factory;

    EXPECT_TRUE(factory.invokable(GT_METADATA(FactoryTestObject)));
    EXPECT_FALSE(factory.invokable(GT_METADATA(GtObject)));

    EXPECT_TRUE(factory.allInvokable(
        {GT_METADATA(FactoryTestObject), GT_METADATA(FactoryTestOtherObject)}));
    EXPECT_FALSE(factory.allInvokable(
        {GT_METADATA(FactoryTestObject), GT_METADATA(GtObject)}));
}

TEST(GtAbstractObjectFactory, SuperClassName)
{
    TestObjectFactory factory;

    EXPECT_TRUE(
        factory.superClassName(GT_CLASSNAME(FactoryTestObject)).isEmpty());

    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));
    EXPECT_EQ(factory.superClassName(GT_CLASSNAME(FactoryTestObject)),
              QString(GT_CLASSNAME(GtObject)));
}

TEST(GtAbstractObjectFactory, MetaObjectLookup)
{
    TestObjectFactory factory;

    EXPECT_EQ(factory.metaObject(GT_CLASSNAME(FactoryTestObject)), nullptr);

    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));
    EXPECT_NE(factory.metaObject(GT_CLASSNAME(FactoryTestObject)), nullptr);
}

TEST(GtAbstractObjectFactory, MakeObjectReturnsUnownedObject)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));

    auto object = factory.makeObject(GT_CLASSNAME(FactoryTestObject));
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parent(), nullptr);
}

TEST(GtAbstractObjectFactory, MakeObjectTypedCast)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClasses(
        {GT_METADATA(FactoryTestObject), GT_METADATA(FactoryTestOtherObject)},
        QStringLiteral("TestModule")));

    auto object = factory.makeObject<FactoryTestOtherObject>(
        GT_CLASSNAME(FactoryTestOtherObject));
    EXPECT_NE(object, nullptr);

    auto invalid = factory.makeObject<FactoryTestOtherObject>(
        GT_CLASSNAME(FactoryTestObject));
    EXPECT_EQ(invalid, nullptr);
}

TEST(GtAbstractObjectFactory, MakeObjectWithParentTransfersOwnership)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));

    GtObject parent;
    GtObject* object =
        factory.makeChild(GT_CLASSNAME(FactoryTestObject), parent);

    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parentObject(), &parent);
}

TEST(GtAbstractObjectFactory, MakeObjectUnknownClassReturnsNull)
{
    TestObjectFactory factory;
    GtObject parent;

    EXPECT_EQ(factory.makeObject(QStringLiteral("NotAClass")), nullptr);
    EXPECT_EQ(factory.makeChild(QStringLiteral("NotAClass"), parent), nullptr);
}

TEST(GtFactoryGroup, ReturnsModuleIdFromProvidingFactory)
{
    TestObjectFactory first;
    TestObjectFactory second;
    ASSERT_TRUE(second.registerClass(GT_METADATA(FactoryTestObject),
                                     QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&first));
    ASSERT_TRUE(group.addFactory(&second));

    EXPECT_EQ(group.moduleId(GT_CLASSNAME(FactoryTestObject)),
              QStringLiteral("TestModule"));
    EXPECT_TRUE(group.moduleId(QStringLiteral("UnknownClass")).isEmpty());
}

TEST(GtFactoryGroup, RegistrationApisAreRejected)
{
    GtFactoryGroup group;

    TestObjectFactory subfactory;
    group.addFactory(&subfactory);

    GtAbstractObjectFactory* factory = &group;

    EXPECT_FALSE(factory->registerClass(GT_METADATA(FactoryTestObject),
                                        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->registerClass(GT_METADATA(FactoryTestOtherObject),
                                        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->registerClasses({GT_METADATA(FactoryTestObject)},
                                          QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->registerClasses({GT_METADATA(FactoryTestOtherObject)},
                                          QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->unregisterClass(GT_METADATA(FactoryTestObject)));
    EXPECT_FALSE(factory->unregisterClass(GT_METADATA(FactoryTestOtherObject)));

    EXPECT_TRUE(subfactory.registerClass(GT_METADATA(FactoryTestObject),
                                         QStringLiteral("TestModule")));
    EXPECT_TRUE(subfactory.registerClasses(
        {GT_METADATA(FactoryTestOtherObject)}, QStringLiteral("TestModule")));
    EXPECT_TRUE(subfactory.unregisterClass(GT_METADATA(FactoryTestObject)));
    EXPECT_TRUE(
        subfactory.unregisterClass(GT_METADATA(FactoryTestOtherObject)));
}

TEST(GtFactoryGroup, DelegatesSuperClassNameToProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    EXPECT_EQ(group.superClassName(GT_CLASSNAME(FactoryTestObject)),
              QString(GT_CLASSNAME(GtObject)));
    EXPECT_TRUE(group.superClassName(QStringLiteral("UnknownClass")).isEmpty());
}

TEST(GtFactoryGroup, KnownClassesAggregatesProvidingFactories)
{
    TestObjectFactory first;
    TestObjectFactory second;
    ASSERT_TRUE(first.registerClass(GT_METADATA(FactoryTestObject),
                                    QStringLiteral("TestModule")));
    ASSERT_TRUE(second.registerClass(GT_METADATA(FactoryTestOtherObject),
                                     QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&first));
    ASSERT_TRUE(group.addFactory(&second));

    const QStringList known = group.knownClasses();
    EXPECT_TRUE(known.contains(GT_CLASSNAME(FactoryTestObject)));
    EXPECT_TRUE(known.contains(GT_CLASSNAME(FactoryTestOtherObject)));
    EXPECT_FALSE(known.contains(GT_CLASSNAME(GtObject)));
}

TEST(GtFactoryGroup, DelegatesMetaObjectLookupToProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    const QMetaObject* mo = group.metaObject(GT_CLASSNAME(FactoryTestObject));
    ASSERT_TRUE(mo);
    ASSERT_STREQ(mo->className(), GT_METADATA(FactoryTestObject).className());
    mo = group.metaObject("UnknownClass");
    ASSERT_FALSE(mo);
}

TEST(GtFactoryGroup, MakeObjectCreatesThroughProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject),
                                      QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    auto object = group.makeObject(GT_CLASSNAME(FactoryTestObject));
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parent(), nullptr);

    GtObject parent;
    GtObject* owned = group.makeChild(GT_CLASSNAME(FactoryTestObject), parent);
    ASSERT_NE(owned, nullptr);
    EXPECT_EQ(owned->parentObject(), &parent);

    EXPECT_EQ(group.makeObject(QStringLiteral("NotAClass")), nullptr);
    EXPECT_EQ(group.makeChild(QStringLiteral("NotAClass"), parent), nullptr);
}

#include "test_gt_abstractobjectfactory.moc"
