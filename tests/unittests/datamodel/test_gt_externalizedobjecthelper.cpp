/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_externalizedobjecthelper.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 16.11.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include "gt_externalizedobjecthelper.h"
#include "gt_externalizationsettings.h"
#include "gt_datazone0d.h"
#include "gt_testhelper.h"

/// This is a test fixture that does a init for each test
class TestGtExternalizedObjectHelper : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        // enable externalization
        gtExternalizationSettings->onEnbaleExternalizationChanged(true);

        params = gtTestHelper->randomStringList(10);
        units  = QVector<QString>(10, "[-]").toList();
        values = gtTestHelper->randomDataVector(10);

        dz = new GtDataZone0D;
        dz->setData(params, values, units);

        // reset ref count
        dz->resetRefCount();
        ASSERT_EQ(dz->refCount(), 0);
        ASSERT_TRUE(dz->isValid());
        // externalize data
        dz->releaseData(GtExternalizedObject::Externalize);
        ASSERT_FALSE(dz->isFetched());
    }

    virtual void TearDown()
    {
        delete dz;
    }

    void checkisFetched()
    {
        EXPECT_TRUE(dz->isFetched());
        EXPECT_EQ(dz->params(), params);
        EXPECT_EQ(dz->units(), units);
        EXPECT_EQ(dz->values(), values);
    }

    void checkisExternalized()
    {
        EXPECT_FALSE(dz->isFetched());
        EXPECT_EQ(dz->params().length(), 0);
        EXPECT_EQ(dz->values().length(), 0);
        EXPECT_EQ(dz->units().length(), 0);
    }

    GtDataZone0D* dz;
    QStringList params;
    QStringList units;
    QVector<double> values;
};

TEST_F(TestGtExternalizedObjectHelper, raii)
{
    // data is not fetched
    EXPECT_EQ(dz->params().length(), 0);

    // new scope
    {
        // init helper
        GtExternalizedObjectHelper<GtDataZone0D> helper
                (dz, GtExternalizedObject::Discard);

        checkisFetched();
        EXPECT_EQ(helper->refCount(), 1);

        // new scope
        {
            GtExternalizedObjectHelper<GtDataZone0D> helper2
                    (dz, GtExternalizedObject::Discard);

            checkisFetched();
            EXPECT_EQ(helper2->refCount(), 2);
        }

        checkisFetched();
        EXPECT_EQ(helper->refCount(), 1);
    }

    // all helpers are destroyed -> data is externalized
    checkisExternalized();
    EXPECT_EQ(dz->refCount(), 0);
}

TEST_F(TestGtExternalizedObjectHelper, isValid)
{
    // init helper
    GtExternalizedObjectHelper<GtDataZone0D> helper
            (dz, GtExternalizedObject::Discard);

    EXPECT_TRUE(helper.isValid());
    EXPECT_EQ(helper->refCount(), 1);    
    checkisFetched();

    // manually externalization
    helper->releaseData(GtExternalizedObject::Discard);
    EXPECT_FALSE(helper.isValid());
    EXPECT_EQ(helper->refCount(), 0);
    checkisExternalized();
}
