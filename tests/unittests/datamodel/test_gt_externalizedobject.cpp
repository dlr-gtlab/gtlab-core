/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include <QtMath>

#include "test_externalizedobject.h"
#include "gt_testhelper.h"
#include "gt_externalizationmanager.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"

/// This is a test fixture that does a init for each test
class TestGtExternalizedObject : public ::testing::Test
{
protected:

    void SetUp() override
    {
        gtExternalizationManager->enableExternalization(true);

        obj = std::make_unique<TestExternalizedObject>();
        obj->setObjectName("TestObject");

        m_values = gtTestHelper->linearDataVector<double>(m_length, 0.0, 2.0);
        m_params = gtTestHelper->randomStringList(1);
    }

    bool isDataExternalized()
    {
        // check if object was used to store params as well
        if (obj->hasParams())
        {
            return obj->internalValues().isEmpty() &&
                   obj->internalParams().isEmpty();
        }
        return obj->internalValues().isEmpty();
    }

    std::unique_ptr<TestExternalizedObject> obj{};

    int m_length{10};
    QVector<double> m_values;
    QStringList m_params;
};

/// external object should be fetched by default
TEST_F(TestGtExternalizedObject, initialization)
{
    // currently no access
    EXPECT_EQ(obj->refCount(), 0);
    // however data is fetched
    EXPECT_TRUE(obj->isFetched());

    EXPECT_TRUE(obj->fetchInitialVersion());
    auto prop = obj->findProperty("metaData");
    ASSERT_TRUE(prop && prop->isValid());
    EXPECT_TRUE(obj->hasModifiedData());
}

/// ref count will be incremented and decremented equally for each access
TEST_F(TestGtExternalizedObject, refCount)
{
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_TRUE(obj->isFetched());

    { // fetch and set data
        auto data = obj->fetchData(); // accessing data no. 1
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        EXPECT_EQ(obj->refCount(), 1);

        { // fetch a second time
            GtExternalizedObject* extObj = obj.get();

            auto data2 = extObj->fetchData(); // accessing data no. 2

            ASSERT_TRUE(data2.isValid());
            EXPECT_EQ(obj->refCount(), 2);
        }

        EXPECT_EQ(obj->refCount(), 1);
    }

    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_TRUE(obj->isFetched());
}

/// an externalized object should only clear its data if no other object/method
/// is accessing it (i.e. ref count is 0)
TEST_F(TestGtExternalizedObject, externalize)
{
    { // fetch data
        auto data = obj->fetchData();
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        EXPECT_EQ(obj->internalValues(), m_values);
        EXPECT_TRUE(obj->isFetched());

        // externalize (ie. write data to disc)
        EXPECT_TRUE(obj->externalize());

        // data was not cleared because its still accessed
        EXPECT_EQ(obj->refCount(), 1);
        EXPECT_TRUE(obj->isFetched());
        EXPECT_FALSE(isDataExternalized());
    }

    // data is no longer acessed -> data is cleared
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_FALSE(obj->isFetched());
    EXPECT_TRUE(isDataExternalized());

    { // fetching once again will internalize data
        auto data = obj->fetchData();

        EXPECT_EQ(obj->refCount(), 1);
        EXPECT_TRUE(obj->isFetched());
        EXPECT_FALSE(isDataExternalized());
    }

    // data is no longer acessed -> data shoudl be cleared again
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_FALSE(obj->isFetched());
    EXPECT_TRUE(isDataExternalized());
}

/// an ext object can be internalized. THe data will be kept internalized
/// until externalize is called explcitly
TEST_F(TestGtExternalizedObject, internalize)
{
    { // fetch data
        auto data = obj->fetchData();
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        // externalize (ie. write data to disc)
        EXPECT_TRUE(obj->externalize());
    }

    // object should be externalized
    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());
    EXPECT_EQ(obj->refCount(), 0);

    // explcitly intrnalize data
    EXPECT_TRUE(obj->internalize());

    { // fetch data
        auto data = obj->fetchData();
    }

    // object should still be internalized
    EXPECT_FALSE(isDataExternalized());
    EXPECT_TRUE(obj->isFetched());
    EXPECT_EQ(obj->refCount(), 0);

    // externalize should externalize object
    EXPECT_TRUE(obj->externalize());
    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());
}

/// an externalized object should not externalize, clear or fetch data
/// if the externalization feature is disabled
TEST_F(TestGtExternalizedObject, disableExternalization)
{
    { // fetch data
        auto data = obj->fetchData();
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        // externalizing data
        EXPECT_TRUE(obj->externalize());

        // disable externalization -> no fetching, externalizing or clearing
        gtExternalizationManager->enableExternalization(false);

        // data was not cleared because its still accessed
        EXPECT_EQ(obj->refCount(), 1);
        EXPECT_TRUE(obj->isFetched());
        EXPECT_FALSE(isDataExternalized());
    }

    // data is no longer acessed but data was not cleared due to disabled
    // externalization
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_TRUE(obj->isFetched());
    EXPECT_FALSE(isDataExternalized());

    { // fetching once again
        auto data = obj->fetchData();

        EXPECT_EQ(obj->refCount(), 1);
        EXPECT_TRUE(obj->isFetched());
        EXPECT_FALSE(isDataExternalized());

        // reenable externalization
        gtExternalizationManager->enableExternalization(true);
    }

    // data is no longer acessed, externalization is enabled -> data was cleared
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());

    // disableing externalization once more
    gtExternalizationManager->enableExternalization(false);

    { // fetching data still works now
        auto data = obj->fetchData();

        // not fetched but ref count is 1
        EXPECT_EQ(obj->refCount(), 1);
        EXPECT_FALSE(isDataExternalized());
        EXPECT_TRUE(obj->isFetched());
    }

    // but data is not externalized
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_FALSE(isDataExternalized());
    EXPECT_TRUE(obj->isFetched());
}

/// An externalized object should keep track of the version/state of the
/// externalized data (i.e.to check if the object has unsaved changes).
/// Once saved the object should no longer have any changes.
TEST_F(TestGtExternalizedObject, hasModifiedData)
{
    // store calculated hash
    QString hash;

    { // access data
        auto data = obj->fetchData();
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        hash = obj->calcExtHash();

        // by default the object has changes
        EXPECT_TRUE(obj->hasModifiedData());

        // after externalization obj has no more changes
        EXPECT_TRUE(obj->externalize());

        EXPECT_FALSE(obj->hasModifiedData());
        EXPECT_FALSE(obj->hasModifiedData(hash));

        { // fetching a second time wont change that
            auto data2 = obj->fetchData();

            EXPECT_FALSE(obj->hasModifiedData());
            EXPECT_FALSE(obj->hasModifiedData(hash));
        }
    }

    // data should be cleared
    EXPECT_TRUE(isDataExternalized());
    // internal data changed -> thus object has changes
    EXPECT_TRUE(obj->hasModifiedData());
    // however compared to the previous fetched version the object has no
    // changes
    EXPECT_FALSE(obj->hasModifiedData(hash));

    { // fetch once again
        auto data = obj->fetchData();

        // data is again unchanged
        EXPECT_FALSE(obj->hasModifiedData());
        EXPECT_FALSE(obj->hasModifiedData(hash));

        // however when changing any value...
        auto values = data.values();
        values[0] = 101;
        data.setValues(values);

        // ...the object has unsaved changes
        EXPECT_TRUE(obj->hasModifiedData());
        EXPECT_FALSE(obj->hasModifiedData(hash));
    }
}

/// once an externalized object was externalized and thus had written
/// modified data to disk it should no longer fetch its intial version
TEST_F(TestGtExternalizedObject, fetchIntialVersion)
{
    { // access data
        auto data = obj->fetchData();
        data.setValues(m_values);
        ASSERT_TRUE(data.isValid());

        ASSERT_TRUE(obj->externalize());
    }

    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());
    // data was externalized -> data should now fetch modified data
    EXPECT_FALSE(obj->fetchInitialVersion());
}

/// Once externalized object was modified and externalized it will no longer
/// fetch its initial version.
/// When undoing the modification the initial version should be fetched again.
///
/// Also changing only a single property of a compound object would would mess
/// up the diffing process as described in issue 251
TEST_F(TestGtExternalizedObject, issue_251)
{
    /* ADDITIONAL SETUP */
    // simulate obj beeing loaded with its intial data on disk
    {
        auto data = obj->fetchData();
        data.setInitialValues(m_values);
        data.setInitialParams(m_params);
        ASSERT_TRUE(data.isValid());
    }

    ASSERT_TRUE(obj->externalize());
    obj->setFetchInitialVersion(true);

    /* ACTUAL TEST */

    // memento before change
    auto memA = obj->toMemento();

    { // access and modify only values
        auto data = obj->fetchData();
        data.setValues(gtTestHelper->linearDataVector<double>(m_length / 2, 1.0, 2.0));
        ASSERT_TRUE(data.isValid());
    }

    // object should be fetched
    ASSERT_FALSE(isDataExternalized());
    ASSERT_TRUE(obj->isFetched());

    // memento after change
    auto memB = obj->toMemento();
    // simulates a save
    ASSERT_TRUE(obj->externalize());

    // data was externalized
    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());

    { // double check that values and params were externalized properly
        auto data = obj->fetchData();
        EXPECT_NE(obj->internalValues(), m_values);
        EXPECT_EQ(obj->internalParams(), m_params);
    }

    gtDebug() << "HERE 1" << obj->internalValues();
    gtDebug() << "HERE 2" << obj->internalParams();

    // data was externalized
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_TRUE(isDataExternalized());
    EXPECT_FALSE(obj->isFetched());
    // data should no longer fetch its initial version
    EXPECT_FALSE(obj->fetchInitialVersion());

    // revert changes
    GtObjectMementoDiff diff{memA, memB};
    ASSERT_FALSE(diff.isNull());
    EXPECT_TRUE(obj->revertDiff(diff));

    gtDebug() << diff.toByteArray();

    // once the diff was applied the object should have fetched its
    // initial version
//    EXPECT_EQ(obj->refCount(), 0);
//    EXPECT_TRUE(obj->isFetched());
//    EXPECT_EQ(obj->internalValues(), m_values);
//    EXPECT_EQ(obj->internalParams(), m_params);

    { // double check that values and params were externalized properly
        auto data = obj->fetchData();
        EXPECT_EQ(obj->internalValues(), m_values);
        EXPECT_EQ(obj->internalParams(), m_params);
    }
}
