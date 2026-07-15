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

TEST(GtFactoryGroup, ReturnsModuleIdFromProvidingFactory)
{
    TestObjectFactory first;
    TestObjectFactory second;
    ASSERT_TRUE(second.registerClass(GT_METADATA(GtObject),
                                     QStringLiteral("TestModule")));

    GtFactoryGroup group;
    ASSERT_TRUE(group.addFactory(&first));
    ASSERT_TRUE(group.addFactory(&second));

    EXPECT_EQ(group.moduleId(GT_CLASSNAME(GtObject)),
              QStringLiteral("TestModule"));
    EXPECT_TRUE(group.moduleId(QStringLiteral("UnknownClass")).isEmpty());
}

TEST(GtFactoryGroup, RegistrationApisAreRejected)
{
    GtFactoryGroup group;

    EXPECT_FALSE(group.registerClass(GT_METADATA(GtObject),
                                     QStringLiteral("TestModule")));
    EXPECT_FALSE(group.registerClasses({GT_METADATA(GtObject)},
                                       QStringLiteral("TestModule")));
}
