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

enum PARAMETER
{
    // set object attribute of property
    WITH_OBJECT,
    // (see issue #587) do not set object attribute of property (i.e. use nullptr)
    WITH_NULLPTR
};

struct TestObjectLinkProperty : public testing::Test
{
    TestFactory factory;
};

struct TestObjectLinkPropertyParametric : public testing::TestWithParam<PARAMETER>
{
    TestFactory factory;
};

TEST_P(TestObjectLinkPropertyParametric, isAllowed)
{
    PARAMETER param = GetParam();

    GtObject myObject;
    myObject.setFactory(&factory);
    GtObjectLinkProperty prop{
        "my_id",
        "my_name",
        "my_brief",
        {},
        (param == WITH_OBJECT) ? &myObject : nullptr,
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

TEST_P(TestObjectLinkPropertyParametric, isAllowed_with_super_classes)
{
    PARAMETER param = GetParam();

    GtObject myObject;
    myObject.setFactory(&factory);
    GtObjectLinkProperty prop{
        "my_id",
        "my_name",
        "my_brief",
        {},
        (param == WITH_OBJECT) ? &myObject : nullptr,
        QStringList{},
        true
    };

    TestSpecialGtObject o;
    GtLabelData l;

    bool wasRegistered = gtObjectFactory->knownClass(o.metaObject()->className());
    if (!wasRegistered) gtObjectFactory->registerClass(*o.metaObject());

    auto cleanup = gt::finally([&](){
        if (!wasRegistered) gtObjectFactory->unregisterClass(*o.metaObject());
    });

    Q_UNUSED(cleanup);

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

INSTANTIATE_TEST_SUITE_P(ParamSet,
                         TestObjectLinkPropertyParametric,
                         ::testing::Values(PARAMETER::WITH_OBJECT,
                                           PARAMETER::WITH_NULLPTR)
                         );

TEST_F(TestObjectLinkProperty, setAllowedClasses_clears_linked_object)
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

TEST_F(TestObjectLinkProperty, canConnect)
{
    GtObjectLinkProperty prop1{"my_id", "my_name", "my_brief", {},
                               nullptr,
                               QStringList{ GT_CLASSNAME(GtObjectGroup) },
                               false};

    GtObjectLinkProperty prop2{"my_id2", "my_name2", "my_brief2",
                               {}, nullptr,
                               QStringList{ GT_CLASSNAME(GtLabelData) },
                               false};

    GtObjectLinkProperty prop3{"my_id3", "my_name3", "my_brief3",
                               {}, nullptr,
                               QStringList{ GT_CLASSNAME(GtLabelData),
                                            GT_CLASSNAME(GtObjectGroup) },
                               false};

    ASSERT_FALSE(prop1.canConnect(prop2));
    ASSERT_TRUE(prop2.canConnect(prop3));
    ASSERT_TRUE(prop1.canConnect(prop3));
}
