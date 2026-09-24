/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *  Author: Matthias Schuff (SG-VTM)
 */

#include "gtest/gtest.h"

#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectuuidmap.h"

#include "test_copy_gtobject_with_uuidmap.h"

class TestCopyGtObjectWithUuidMapping : public ::testing::Test
{
public:
    void SetUp() override
    {
        gtObjectFactory->registerClass(MyGtObject::staticMetaObject);

        objgroup = new GtObjectGroup;

        objgroup->setObjectName("MyGroup");
        objgroup->setFactory(gtObjectFactory);
    }

    void TearDown() override
    {
        delete objgroup;
    }

    void clearObjGroup()
    {
        for(auto c: objgroup->findDirectChildren())
        {
            delete c;
        }
    }

    void fillObjGroup(uint n_firstlevel, uint n_secondlevel = 0, bool clearExisting=true)
    {
        if(clearExisting) clearObjGroup();

        int i_start = objgroup->childCount();

        for(uint i=i_start; i<n_firstlevel+i_start; i++)
        {
            auto obj = new MyGtObject;
            obj->setObjectName("obj"+QString::number(i));
            this->objgroup->appendChild(obj);

            if (n_secondlevel > 0)
            {
                for(uint j=0; j<n_secondlevel; j++)
                {
                    auto obj2 = new MyGtObject;
                    obj2->setObjectName("obj"+QString::number(i)+"."+QString::number(j));
                    obj->appendChild(obj2);
                }
            }

        }
    };

    GtObjectGroup* objgroup;
};

TEST_F(TestCopyGtObjectWithUuidMapping, baseFunctionalityOfGtObjectUUIDMap)
{
    GtObjectUUIDMap uuidMap;
    ASSERT_TRUE(uuidMap.isEmpty());

    uuidMap.insert("hello", "world");
    ASSERT_EQ(uuidMap.size(), 1);

    uuidMap.insert("foo", "bar");
    ASSERT_EQ(uuidMap.size(), 2);

    ASSERT_TRUE(uuidMap.containsOriginalUuid("hello"));
    ASSERT_FALSE(uuidMap.containsOriginalUuid("42"));

    ASSERT_EQ(uuidMap.copiedUuid("hello"), "world");
    ASSERT_EQ(uuidMap.copiedUuid("42"), "");

    ASSERT_FALSE(uuidMap.isEmpty());

    uuidMap.clear();
    ASSERT_TRUE(uuidMap.isEmpty());
    ASSERT_FALSE(uuidMap.containsOriginalUuid("hello"));
}

void checkObjectsUuidChange(GtObject* orig, GtObject* copied, GtObjectUUIDMap* uuidMap = nullptr)
{
    ASSERT_EQ(orig->childCount(), copied->childCount());
    ASSERT_NE(orig->uuid(), copied->uuid());

    if(uuidMap)
    {
        ASSERT_TRUE(uuidMap->containsOriginalUuid(orig->uuid()));
        ASSERT_EQ(uuidMap->copiedUuid(orig->uuid()), copied->uuid() );
    }

    auto childrenOrig = orig->findDirectChildren();
    auto childrenCopied = copied->findDirectChildren();

    for (int i=0; i<childrenOrig.size(); i++)
    {
        checkObjectsUuidChange( childrenOrig.at(i), childrenCopied.at(i), uuidMap);
    }

}

TEST_F(TestCopyGtObjectWithUuidMapping, copyWithoutMapping)
{
    this->fillObjGroup(2, 2);
    auto newobj = objgroup->copy();

    checkObjectsUuidChange(objgroup, newobj);

    delete newobj;
}

TEST_F(TestCopyGtObjectWithUuidMapping, copyWithMapping)
{
    this->fillObjGroup(2, 2);

    GtObjectUUIDMap uuidMap;
    auto newobj = objgroup->copy(uuidMap);
    ASSERT_EQ(uuidMap.size(), 7);

    checkObjectsUuidChange(objgroup, newobj, &uuidMap);

    delete newobj;
}

TEST_F(TestCopyGtObjectWithUuidMapping, copyWithMappingReplaceOldList)
{
    this->fillObjGroup(2, 2);

    GtObjectUUIDMap uuidMap;
    uuidMap.insert("hello", "world");

    auto newobj = objgroup->copy(uuidMap);
    Q_UNUSED(newobj);

    ASSERT_FALSE(uuidMap.containsOriginalUuid("hello"));
    ASSERT_EQ(uuidMap.size(), 7);

    delete newobj;
}
