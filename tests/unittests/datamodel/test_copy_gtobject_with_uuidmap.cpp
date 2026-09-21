/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_copy_gt_object_with_uuidmap.cpp
 *
 *  Created on: 18.09.2026
 *  Author: Matthias Schuff (SG-VTM)
 */

#include <QDebug>
#include <chrono>
#include <string>

#include "gtest/gtest.h"

#include "test_copy_gtobject_with_uuidmap.h"

#include "gt_objectfactory.h"
#include "gt_objectgroup.h"


using Clock = std::chrono::steady_clock;


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


    void printObjects(const QString& headermsg)
    {
        qDebug().noquote() << ":: " << headermsg;
        qDebug().noquote() << "has children ("+QString::number(objgroup->childCount())+")";
        for(auto c: objgroup->findDirectChildren())
        {
            qDebug().noquote() << c->objectName() << "->" << c;
        }
    };

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

TEST_F(TestCopyGtObjectWithUuidMapping, testFillingAndClearingOfTestGroup)
{
    ASSERT_EQ(this->objgroup->childCount(), 0);

    this->printObjects("init");

    this->clearObjGroup();

    this->printObjects("after clear");

    this->fillObjGroup(5, 2);
    this->printObjects("after new children");

    ASSERT_EQ(this->objgroup->childCount(), 5);
    auto c = this->objgroup->findDirectChildren();
    ASSERT_EQ(c[0]->childCount(), 2);
    ASSERT_EQ(c[1]->childCount(), 2);
    ASSERT_EQ(c[2]->childCount(), 2);
    ASSERT_EQ(c[3]->childCount(), 2);
    ASSERT_EQ(c[4]->childCount(), 2);

    this->clearObjGroup();
    this->printObjects("after clear 2");

    ASSERT_EQ(this->objgroup->childCount(), 0);
}

TEST_F(TestCopyGtObjectWithUuidMapping, testAppendingToTestGroup)
{

    this->fillObjGroup(2, 2);

    {
        ASSERT_EQ(this->objgroup->childCount(), 2);
        auto c = objgroup->findDirectChildren();
        ASSERT_EQ(c[0]->objectName(), "obj0");
        {
            ASSERT_EQ(c[0]->childCount(), 2);
            auto cc = c[0]->findDirectChildren();
            ASSERT_EQ(cc[0]->objectName(), "obj0.0");
            ASSERT_EQ(cc[1]->objectName(), "obj0.1");
        }
        ASSERT_EQ(c[1]->objectName(), "obj1");
        {
            ASSERT_EQ(c[1]->childCount(), 2);
            auto cc = c[1]->findDirectChildren();
            ASSERT_EQ(cc[0]->objectName(), "obj1.0");
            ASSERT_EQ(cc[1]->objectName(), "obj1.1");
        }
    }

    this->fillObjGroup(2, 0, false);
    {
        ASSERT_EQ(this->objgroup->childCount(), 4);
        auto c = objgroup->findDirectChildren();
        ASSERT_EQ(c[0]->objectName(), "obj0");
        ASSERT_EQ(c[0]->childCount(), 2);
        ASSERT_EQ(c[1]->objectName(), "obj1");
        ASSERT_EQ(c[1]->childCount(), 2);
        ASSERT_EQ(c[2]->objectName(), "obj2");
        ASSERT_EQ(c[2]->childCount(), 0);
        ASSERT_EQ(c[3]->objectName(), "obj3");
        ASSERT_EQ(c[3]->childCount(), 0);
    }

    this->fillObjGroup(3, 1, true);

    {
        ASSERT_EQ(this->objgroup->childCount(), 3);
        auto c = objgroup->findDirectChildren();
        ASSERT_EQ(c[0]->objectName(), "obj0");
        ASSERT_EQ(c[1]->objectName(), "obj1");
        ASSERT_EQ(c[2]->objectName(), "obj2");


        {
            ASSERT_EQ(c[0]->childCount(), 1);
            auto cc = c[0]->findDirectChildren();
            ASSERT_EQ(cc[0]->objectName(), "obj0.0");
        }

        {
            ASSERT_EQ(c[1]->childCount(), 1);
            auto cc = c[1]->findDirectChildren();
            ASSERT_EQ(cc[0]->objectName(), "obj1.0");
        }

        {
            ASSERT_EQ(c[2]->childCount(), 1);
            auto cc = c[2]->findDirectChildren();
            ASSERT_EQ(cc[0]->objectName(), "obj2.0");
        }
    }
}


TEST_F(TestCopyGtObjectWithUuidMapping, copyWithoutMapping)
{
    this->fillObjGroup(2, 2);
    auto newobj = objgroup->copy();

    ASSERT_EQ(this->objgroup->childCount(), 2);
    ASSERT_EQ(newobj->childCount(), 2);

    ASSERT_NE(objgroup->uuid(), newobj->uuid());

    auto cOrig = this->objgroup->findDirectChildren();
    auto cNew = newobj->findDirectChildren();

    ASSERT_NE(cOrig[0]->uuid(), cNew[0]->uuid());
    ASSERT_NE(cOrig[1]->uuid(), cNew[1]->uuid());
}



TEST_F(TestCopyGtObjectWithUuidMapping, copyWithMapping)
{
    this->fillObjGroup(2, 2);

    GtObjectUUIDMap uuidMap;
    auto newobj = objgroup->copy(uuidMap);

    auto cOrig = this->objgroup->findDirectChildren();
    auto cNew = newobj->findDirectChildren();

    auto cOrig0 = this->objgroup->findDirectChildren().at(0);
    auto cOrig00 = this->objgroup->findDirectChildren().at(0)->findDirectChildren().at(0);
    auto cOrig01 = this->objgroup->findDirectChildren().at(0)->findDirectChildren().at(1);
    auto cOrig1 = this->objgroup->findDirectChildren().at(1);
    auto cOrig10 = this->objgroup->findDirectChildren().at(1)->findDirectChildren().at(0);
    auto cOrig11 = this->objgroup->findDirectChildren().at(1)->findDirectChildren().at(1);

    auto cNew0 = newobj->findDirectChildren().at(0);
    auto cNew00 = newobj->findDirectChildren().at(0)->findDirectChildren().at(0);
    auto cNew01 = newobj->findDirectChildren().at(0)->findDirectChildren().at(1);
    auto cNew1 = newobj->findDirectChildren().at(1);
    auto cNew10 = newobj->findDirectChildren().at(1)->findDirectChildren().at(0);
    auto cNew11 = newobj->findDirectChildren().at(1)->findDirectChildren().at(1);


    ASSERT_EQ(cOrig0->objectName(), cNew0->objectName() );
    ASSERT_EQ(cOrig1->objectName(), cNew1->objectName() );

    ASSERT_EQ(cOrig00->objectName(), cNew00->objectName() );
    ASSERT_EQ(cOrig01->objectName(), cNew01->objectName() );

    ASSERT_EQ(cOrig10->objectName(), cNew10->objectName() );
    ASSERT_EQ(cOrig11->objectName(), cNew11->objectName() );


    ASSERT_EQ(uuidMap.newUuid(this->objgroup->uuid()), newobj->uuid());

    ASSERT_EQ(uuidMap.newUuid(cOrig0->uuid()), cNew0->uuid() );
    ASSERT_EQ(uuidMap.newUuid(cOrig1->uuid()), cNew1->uuid() );

    ASSERT_EQ(uuidMap.newUuid(cOrig00->uuid()), cNew00->uuid() );
    ASSERT_EQ(uuidMap.newUuid(cOrig01->uuid()), cNew01->uuid() );

    ASSERT_EQ(uuidMap.newUuid(cOrig10->uuid()), cNew10->uuid() );
    ASSERT_EQ(uuidMap.newUuid(cOrig11->uuid()), cNew11->uuid() );


    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig0->uuid()))->objectName(), cNew0->objectName() );
    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig1->uuid()))->objectName(), cNew1->objectName() );

    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig00->uuid()))->objectName(), cNew00->objectName() );
    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig01->uuid()))->objectName(), cNew01->objectName() );

    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig10->uuid()))->objectName(), cNew10->objectName() );
    ASSERT_EQ(newobj->getObjectByUuid(uuidMap.newUuid(cOrig11->uuid()))->objectName(), cNew11->objectName() );


}





