/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_objectpathproperty.cpp
 */

#include "gtest/gtest.h"

#include <memory>

#include "gt_object.h"
#include "gt_objectpath.h"
#include "gt_objectpathproperty.h"

class TestGtObjectPathProperty : public ::testing::Test
{
protected:
    void SetUp() override
    {
        root = std::make_unique<GtObject>();
        root->setObjectName("root");

        owner = new GtObject(root.get());
        owner->setObjectName("owner");

        target = new GtObject(owner);
        target->setObjectName("target");

        sibling = new GtObject(root.get());
        sibling->setObjectName("sibling");
    }

    std::unique_ptr<GtObject> root;
    GtObject* owner{nullptr};
    GtObject* target{nullptr};
    GtObject* sibling{nullptr};
};

TEST_F(TestGtObjectPathProperty, accessorsReturnConfiguredValues)
{
    QStringList allowedClasses{QStringLiteral("GtObject"),
                               QStringLiteral("Other")};
    GtObjectPathProperty property("ident", "display", "brief",
                                  "root/owner/target", owner, allowedClasses);

    EXPECT_EQ(property.object(), owner);
    EXPECT_EQ(property.allowedClasses(), allowedClasses);
    EXPECT_EQ(property.path().toString(), QString("root/owner/target"));
}

TEST_F(TestGtObjectPathProperty, valueRoundTripUsesStringPath)
{
    GtObjectPathProperty property("ident", "display", "brief",
                                  "root/owner/target", owner, {});

    bool success = false;
    QVariant value = property.valueToVariant("", &success);

    EXPECT_TRUE(success);
    EXPECT_EQ(value.toString(), QString("root/owner/target"));

    EXPECT_TRUE(
        property.setValueFromVariant(QVariant(QString("root/sibling")), ""));
    EXPECT_EQ(property.getVal(), QString("root/sibling"));
    EXPECT_EQ(property.path().toString(), QString("root/sibling"));
}

TEST_F(TestGtObjectPathProperty, linkedObjectReturnsNullForEmptyPath)
{
    GtObjectPathProperty property("ident", "display", "brief", QString(), owner,
                                  {});

    EXPECT_EQ(property.linkedObject(root.get()), nullptr);
    EXPECT_EQ(property.linkedObject(), nullptr);
}

TEST_F(TestGtObjectPathProperty, linkedObjectResolvesWithExplicitRoot)
{
    GtObjectPathProperty property("ident", "display", "brief",
                                  GtObjectPath(target).toString(), nullptr, {});

    EXPECT_EQ(property.linkedObject(root.get()), target);
    EXPECT_EQ(property.linkedObject(sibling), nullptr);
}

TEST_F(TestGtObjectPathProperty, linkedObjectResolvesFromOwningRoot)
{
    GtObjectPathProperty property("ident", "display", "brief",
                                  GtObjectPath(target).toString(), owner, {});

    EXPECT_EQ(property.linkedObject(), target);
}

TEST_F(TestGtObjectPathProperty, linkedObjectReturnsNullWithoutRootAndOwner)
{
    GtObjectPathProperty property("ident", "display", "brief",
                                  GtObjectPath(target).toString(), nullptr, {});

    EXPECT_EQ(property.linkedObject(), nullptr);
}
