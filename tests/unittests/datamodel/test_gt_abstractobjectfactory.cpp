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

namespace
{

    class TestObjectFactory : public GtAbstractObjectFactory
    {
    };

} // namespace

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

TEST(GtAbstractObjectFactory, ExistingRegistrationApiHasNoModuleId)
{
    TestObjectFactory factory;

    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject)));
    EXPECT_TRUE(factory.knownClass(GT_CLASSNAME(GtObject)));
    EXPECT_TRUE(factory.moduleId(GT_CLASSNAME(GtObject)).isEmpty());
}

TEST(GtAbstractObjectFactory, RegistersAndRemovesClassModuleId)
{
    TestObjectFactory factory;

    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject),
                                      QStringLiteral("TestModule")));
    EXPECT_EQ(factory.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("TestModule"));

    ASSERT_TRUE(factory.unregisterClass(GT_METADATA(GtObject)));
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

TEST(GtAbstractObjectFactory, LegacyBulkRegistrationHasNoModuleId)
{
    TestObjectFactory factory;

    ASSERT_TRUE(factory.registerClasses({GT_METADATA(GtObject)}));
    EXPECT_TRUE(factory.moduleId(GT_CLASSNAME(GtObject)).isEmpty());
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
    ASSERT_TRUE(factory.registerClass(GT_METADATA(GtObject)));

    EXPECT_FALSE(factory.registerClasses({GT_METADATA(GtObject)},
                                         QStringLiteral("TestModule")));
    EXPECT_TRUE(factory.moduleId(GT_CLASSNAME(GtObject)).isEmpty());
}

TEST(GtAbstractObjectFactory, LegacyBulkRegistrationRejectsDuplicates)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClasses({GT_METADATA(GtObject),
                                         GT_METADATA(GtObjectGroup)}));

    EXPECT_FALSE(factory.registerClasses({GT_METADATA(GtObjectGroup)}));
    EXPECT_EQ(factory.knownClasses().size(), 2);
}

TEST(GtAbstractObjectFactory, InvokableDetection)
{
    TestObjectFactory factory;

    EXPECT_TRUE(factory.invokable(GT_METADATA(FactoryTestObject)));
    EXPECT_FALSE(factory.invokable(GT_METADATA(GtObject)));

    EXPECT_TRUE(factory.allInvokable({GT_METADATA(FactoryTestObject),
                                      GT_METADATA(FactoryTestOtherObject)}));
    EXPECT_FALSE(factory.allInvokable({GT_METADATA(FactoryTestObject),
                                       GT_METADATA(GtObject)}));
}

TEST(GtAbstractObjectFactory, SuperClassName)
{
    TestObjectFactory factory;

    EXPECT_TRUE(factory.superClassName(GT_CLASSNAME(FactoryTestObject))
                    .isEmpty());

    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));
    EXPECT_EQ(factory.superClassName(GT_CLASSNAME(FactoryTestObject)),
              QString(GT_CLASSNAME(GtObject)));
}

TEST(GtAbstractObjectFactory, MetaObjectLookup)
{
    TestObjectFactory factory;

    EXPECT_EQ(factory.metaObject(GT_CLASSNAME(FactoryTestObject)), nullptr);

    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));
    EXPECT_NE(factory.metaObject(GT_CLASSNAME(FactoryTestObject)), nullptr);
}

TEST(GtAbstractObjectFactory, MakeObjectReturnsUnownedObject)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));

    auto object = factory.makeObject(GT_CLASSNAME(FactoryTestObject));
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parent(), nullptr);
}

TEST(GtAbstractObjectFactory, MakeObjectTypedCast)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClasses(
        {GT_METADATA(FactoryTestObject), GT_METADATA(FactoryTestOtherObject)}));

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
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));

    GtObject parent;
    GtObject* object =
        factory.makeObject(GT_CLASSNAME(FactoryTestObject), parent);

    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parentObject(), &parent);
}

TEST(GtAbstractObjectFactory, MakeObjectUnknownClassReturnsNull)
{
    TestObjectFactory factory;
    GtObject parent;

    EXPECT_EQ(factory.makeObject(QStringLiteral("NotAClass")), nullptr);
    EXPECT_EQ(factory.makeObject(QStringLiteral("NotAClass"), parent), nullptr);
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
    GtAbstractObjectFactory* factory = &group;

    EXPECT_FALSE(factory->registerClass(GT_METADATA(FactoryTestObject)));
    EXPECT_FALSE(factory->registerClass(GT_METADATA(FactoryTestObject),
                                        QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->registerClasses({GT_METADATA(FactoryTestObject)}));
    EXPECT_FALSE(factory->registerClasses({GT_METADATA(FactoryTestObject)},
                                          QStringLiteral("TestModule")));
    EXPECT_FALSE(factory->unregisterClass(GT_METADATA(FactoryTestObject)));
}

TEST(GtFactoryGroup, DelegatesSuperClassNameToProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    EXPECT_EQ(group.superClassName(GT_CLASSNAME(FactoryTestObject)),
              QString(GT_CLASSNAME(GtObject)));
    EXPECT_TRUE(group.superClassName(QStringLiteral("UnknownClass")).isEmpty());
}

TEST(GtFactoryGroup, DelegatesMetaObjectLookupToProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    EXPECT_NE(group.metaObject(GT_CLASSNAME(FactoryTestObject)), nullptr);
    EXPECT_EQ(group.metaObject(QStringLiteral("UnknownClass")), nullptr);
}

TEST(GtFactoryGroup, MakeObjectCreatesThroughProvidingFactory)
{
    TestObjectFactory factory;
    ASSERT_TRUE(factory.registerClass(GT_METADATA(FactoryTestObject)));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&factory));

    auto object = group.makeObject(GT_CLASSNAME(FactoryTestObject));
    ASSERT_NE(object, nullptr);
    EXPECT_EQ(object->parent(), nullptr);

    GtObject parent;
    GtObject* owned = group.makeObject(GT_CLASSNAME(FactoryTestObject), parent);
    ASSERT_NE(owned, nullptr);
    EXPECT_EQ(owned->parentObject(), &parent);

    EXPECT_EQ(group.makeObject(QStringLiteral("NotAClass")), nullptr);
    EXPECT_EQ(group.makeObject(QStringLiteral("NotAClass"), parent), nullptr);
}

#include "test_gt_abstractobjectfactory.moc"
