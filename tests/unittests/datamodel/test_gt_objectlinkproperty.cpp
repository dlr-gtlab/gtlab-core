/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 7.6.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "test_gt_object.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectfactory.h"
#include "gt_labeldata.h"

struct TestFactory : public GtAbstractObjectFactory
{
    TestFactory()
    {
        registerClass(GT_METADATA(GtObject));
        registerClass(GT_METADATA(GtObjectGroup));
        registerClass(GT_METADATA(TestSpecialGtObject));
    }
};

struct TestObjectLinkProperty : public testing::Test
{
    TestFactory factory;
};

TEST_F(TestObjectLinkProperty, isAllowed)
{
    GtObject myObject;
    myObject.setFactory(&factory);
    GtObjectLinkProperty prop{
        "my_id",
        "my_name",
        "my_brief",
        {},
        &myObject,
        QStringList{},
        false
    };

    TestSpecialGtObject o;
    GtLabelData l;
    // not allowed classes set
    EXPECT_FALSE(prop.isAllowed(o));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));

    prop.setAllowedClasses(QStringList{
        GT_CLASSNAME(TestSpecialGtObject)
    });

    EXPECT_TRUE(prop.isAllowed(o));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));

    prop.setAllowedClasses(
        prop.allowedClasses() + QStringList{
        GT_CLASSNAME(GtObjectGroup)
    });

    EXPECT_TRUE(prop.isAllowed(o));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));
}

TEST_F(TestObjectLinkProperty, isAllowed_superClasses)
{
    GtObject myObject;
    myObject.setFactory(&factory);
    GtObjectLinkProperty prop{
        "my_id",
        "my_name",
        "my_brief",
        {},
        &myObject,
        QStringList{},
        true
    };

    TestSpecialGtObject o;
    GtLabelData l;
    // not allowed classes set
    EXPECT_FALSE(prop.isAllowed(o));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));

    prop.setAllowedClasses(QStringList{
        GT_CLASSNAME(TestSpecialGtObject)
    });

    EXPECT_TRUE(prop.isAllowed(o));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));

    prop.setAllowedClasses(QStringList{
        GT_CLASSNAME(GtObjectGroup)
    });

    // is derived of object group
    EXPECT_TRUE(prop.isAllowed(o));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    // while GtLabelData is not part of our factory, it is registered in
    // GtObjectFactory which is used as a backup
    ASSERT_TRUE(gtObjectFactory->knownClass(GT_CLASSNAME(GtLabelData)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_FALSE(prop.isAllowed(GT_CLASSNAME(GtObject)));

    prop.setAllowedClasses(QStringList{
        GT_CLASSNAME(GtObject)
    });

    EXPECT_TRUE(prop.isAllowed(o));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(TestSpecialGtObject)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtLabelData)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtObjectGroup)));
    EXPECT_TRUE(prop.isAllowed(GT_CLASSNAME(GtObject)));
}

TEST_F(TestObjectLinkProperty, setAllowedClassesMyClearLinkedObject)
{
    GtObject myObject;
    myObject.setFactory(&factory);

    GtObjectLinkProperty prop{
        "my_id",
        "my_name",
        "my_brief",
        {},
        &myObject,
        QStringList{
            GT_CLASSNAME(TestSpecialGtObject)
        }
    };

    auto* o = new TestSpecialGtObject;
    myObject.appendChild(o);

    // set linked object
    prop.setVal(o->uuid());

    EXPECT_EQ(prop.linkedObject(), o);

    // removing the classname from the list should reset the linked object
    prop.setAllowedClasses(QStringList{
    });

    EXPECT_EQ(prop.linkedObject(), nullptr);
}
