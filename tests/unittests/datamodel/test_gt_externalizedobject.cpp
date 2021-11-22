/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 16.11.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include "gt_externalizationsettings.h"
#include "gt_datazone0d.h"
#include "gt_testhelper.h"

/// This is a test fixture that does a init for each test
class TestGtExternalizedObject : public ::testing::Test
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

TEST_F(TestGtExternalizedObject, refCount)
{
    // fetch
    ASSERT_TRUE(dz->fetchData());
    EXPECT_EQ(dz->refCount(), 1);
    // properties should not be empty
    checkisFetched();

    // fetch a second time
    ASSERT_TRUE(dz->fetchData());
    EXPECT_EQ(dz->refCount(), 2);
    // properties should not be empty
    checkisFetched();

    // externalize once
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    EXPECT_EQ(dz->refCount(), 1);
    // properties should not be empty
    checkisFetched();

    // externalize twice
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    EXPECT_EQ(dz->refCount(), 0);
    // properties should be empty now
    checkisExternalized();

    // further externalizing should do nothing
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    EXPECT_EQ(dz->refCount(), 0);
    checkisExternalized();
}

TEST_F(TestGtExternalizedObject, fetchAndRelease)
{
    // fetch
    ASSERT_TRUE(dz->fetchData());
    checkisFetched();

    // release the data
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    checkisExternalized();

    // fetch and check contents
    ASSERT_TRUE(dz->fetchData());
    checkisFetched();
}

TEST_F(TestGtExternalizedObject, standalone)
{
    // datazone is new and was not externalized before
    // -> fetched even though ref count is 0
    // -> data should not be cleared
    EXPECT_TRUE(dz->objectStates() & GtExternalizedObject::Fetched);
    EXPECT_TRUE(dz->objectStates() & GtExternalizedObject::Standalone);
    EXPECT_EQ(dz->refCount(), 0);
    checkisFetched();

    // discarding shoud do nothing
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Discard));
    EXPECT_EQ(dz->refCount(), 0);
    // properties should not be cleared
    checkisFetched();

    // data must be externalized in this case
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    EXPECT_FALSE(dz->objectStates() & GtExternalizedObject::Fetched);
    EXPECT_FALSE(dz->objectStates() & GtExternalizedObject::Standalone);
    EXPECT_EQ(dz->refCount(), 0);
    // properties should be cleared now
    checkisExternalized();
}

TEST_F(TestGtExternalizedObject, disableExternalization)
{
    ASSERT_TRUE(dz->fetchData());
    EXPECT_EQ(dz->refCount(), 1);
    checkisFetched();

    // disable externalization feature
    gtExternalizationSettings->onEnbaleExternalizationChanged(false);

    // fetching should do nothing now
    ASSERT_TRUE(dz->fetchData());
    EXPECT_EQ(dz->refCount(), 1);
    checkisFetched();

    // externalizing should do nothing now
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    EXPECT_EQ(dz->refCount(), 1);
    checkisFetched();
}

TEST_F(TestGtExternalizedObject, hasChanges)
{
    // fetch & externalize the data
    ASSERT_TRUE(dz->fetchData());
    checkisFetched();
    ASSERT_TRUE(dz->releaseData(GtExternalizedObject::Externalize));
    checkisExternalized();

    // cached hash should be set
    EXPECT_FALSE(dz->cachedHash().isEmpty());
    QString cHash = dz->cachedHash();

    // modify values
    for (QString& p : dz->params())
    {
        dz->setValue(p, 0);
    }

    EXPECT_TRUE(dz->hasChanges());
    EXPECT_FALSE(dz->hasChanges(cHash));
}
