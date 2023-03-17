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
        static const auto register_class_once = [](){
            gtObjectFactory->registerClass(GT_METADATA(TestExternalizedObject));
            return 0;
        }();

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

    /*
     * Helper methods and data for diffing
     */

    QVector<double> origValues = {1};
    QStringList origParams = {"p1"};

    QVector<double> changedValues = {1};
    QStringList changedParams = {"p1", "p2"};

    template <typename T>
    bool compareHelper(T const& current, T const& expected, const char* name)
    {
        if (current != expected)
        {
            gtError().nospace()
                    << "MISMATCH: " << name << ": '"
                    << current << "' vs expected '" << expected << "'";
            return false;
        }
        return true;
    }

    bool doSetupObject()
    {
        {
            auto data = obj->fetchData();
            data.setParams(origParams);
            data.setValues(origValues);
        }
        return obj->isFetched();
    }

    bool doChangeData()
    {
        {
            auto data = obj->fetchData();
            data.setParams(changedParams);
            data.setValues(changedValues);
        }

        return obj->isFetched();
    }

    bool hasOriginalData()
    {
        auto data = obj->fetchData();
        bool eq = compareHelper(data.params(), origParams, "PARAMS");
        eq     &= compareHelper(data.values(), origValues, "VALUES");
        return eq;
    }

    bool hasChangedData()
    {
        auto data = obj->fetchData();
        bool eq = compareHelper(data.params(), changedParams, "PARAMS");
        eq     &= compareHelper(data.values(), changedValues, "VALUES");
        return eq;
    }

    bool doExternalize()
    {
        return obj->externalize() && !obj->isFetched();
    }
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

/// An externalized object should only clear its data if no other object/method
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

    // data is no longer acessed -> data should be cleared again
    EXPECT_EQ(obj->refCount(), 0);
    EXPECT_FALSE(obj->isFetched());
    EXPECT_TRUE(isDataExternalized());
}

/// An ext object can be internalized. The data will be kept internalized
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

    // oject should not be fetched
    EXPECT_FALSE(isDataExternalized());
    EXPECT_TRUE(obj->isFetched());
    EXPECT_EQ(obj->refCount(), 0);

    { // fetch data should not change anything
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

/// An externalized object should not externalize, clear or fetch data
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

/// Once an externalized object was externalized and thus had written
/// modified data to disk it should no longer fetch its initial version but the
/// modified one
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

/* Diffing behavious of an externalized object:

  CASE 1: diff while fetched (normal diffing behaviour)
    > fetch object
    > create M1
    > change object
    > create M2
    > revert diff

  Case 2: (diff after externalization)
    > fetch object
    > create M1
    > change object
    > create M2
    > externalize object
    > revert diff

  Case 3: m1 before fetch and diff after externalization
    > create M1
    > fetch object
    > change object
    > create M2
    > externalize object
    > revert diff

  Case 4: m1 before fetch and m2 and diff while fetched
    (probably the case most of the times)
    > create M1
    > fetch object
    > change object
    > create M2
    > revert diff

  NOTE: The following cases will happend very rarely as usally the data wont be
        saved/externalized before creating m2:

  Case 5: m2 and diff after externalization
    > fetch object
    > create M1
    > change object
    > externalize object
    > create M2
    > revert diff

  Case 6: m1 before fetch and m2 and diff after externalization
    > create M1
    > fetch object
    > change object
    > externalize object
    > create M2
    > revert diff
 */

// To make sure that externalizing before redoing the diff does not change
// anything we use a parametric test
class TestGtExternalizedObjectDiff :
        public TestGtExternalizedObject,
        public ::testing::WithParamInterface<bool>
{ };

INSTANTIATE_TEST_SUITE_P(TestGtExternalizedObjectDiff,
                         TestGtExternalizedObjectDiff,
                         testing::Values(true, false));


/// Object was fetched when creating m1 and m2 and when reverting diff. The
/// diffing should behave as if the object was an ordinary GtObject.
TEST_P(TestGtExternalizedObjectDiff, diff_while_fetched)
{
    bool externalizeBeforeRedo = GetParam();

    gtDebug() << "### Externalize before redo:" << externalizeBeforeRedo;

    ASSERT_TRUE(doSetupObject());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    ASSERT_TRUE(obj->revertDiff(diff));

    EXPECT_TRUE(hasOriginalData());

    if (externalizeBeforeRedo)
    {
        EXPECT_TRUE(doExternalize());
    }

    ASSERT_TRUE(obj->applyDiff(diff));

    EXPECT_TRUE(hasChangedData());
}

/// Object was fetched when creating m1 and m2.
/// Before reverting diff object was externalized.
/// Reverting should internalie object and apply m1 correctly
TEST_P(TestGtExternalizedObjectDiff, diff_after_externalizing)
{
    bool externalizeBeforeRedo = GetParam();

    gtDebug() << "### Externalize before redo:" << externalizeBeforeRedo;

    ASSERT_TRUE(doSetupObject());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    EXPECT_TRUE(doExternalize());

    ASSERT_TRUE(obj->revertDiff(diff));

    EXPECT_TRUE(hasOriginalData());

    if (externalizeBeforeRedo)
    {
        EXPECT_TRUE(doExternalize());
    }

    ASSERT_TRUE(obj->applyDiff(diff));

    EXPECT_TRUE(hasChangedData());
}

/// Object was NOT fetched when creating m1 and m2.
/// Before reverting diff object was externalized.
/// Reverting should internalie object and apply m1 correctly
TEST_P(TestGtExternalizedObjectDiff, m1_before_fetch_and_diff_after_externalizing)
{
    bool externalizeBeforeRedo = GetParam();

    gtDebug() << "### Externalize before redo:" << externalizeBeforeRedo;

    ASSERT_TRUE(doSetupObject());

    EXPECT_TRUE(doExternalize());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    EXPECT_TRUE(doExternalize());

    ASSERT_TRUE(obj->revertDiff(diff));

    EXPECT_TRUE(hasOriginalData());

    if (externalizeBeforeRedo)
    {
        EXPECT_TRUE(doExternalize());
    }

    ASSERT_TRUE(obj->applyDiff(diff));

    EXPECT_TRUE(hasChangedData());
}

/// Object was NOT fetched when creating m1 and m2.
/// Reverting should internalie object and apply m1 correctly
TEST_P(TestGtExternalizedObjectDiff, m1_before_fetch_and_diff_while_fetched)
{
    bool externalizeBeforeRedo = GetParam();

    gtDebug() << "### Externalize before redo:" << externalizeBeforeRedo;

    ASSERT_TRUE(doSetupObject());

    EXPECT_TRUE(doExternalize());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    ASSERT_TRUE(obj->revertDiff(diff));

    EXPECT_TRUE(hasOriginalData());

    if (externalizeBeforeRedo)
    {
        EXPECT_TRUE(doExternalize());
    }

    ASSERT_TRUE(obj->applyDiff(diff));

    EXPECT_TRUE(hasChangedData());
}

/// Object was fetched when reating m1 but was externalized when creating m2 and
/// diffing
TEST_P(TestGtExternalizedObjectDiff, m2_and_diff_after_externalizing)
{
    bool externalizeBeforeRedo = GetParam();

    gtDebug() << "### Externalize before redo:" << externalizeBeforeRedo;

    ASSERT_TRUE(doSetupObject());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    EXPECT_TRUE(doExternalize());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    ASSERT_TRUE(obj->revertDiff(diff));

    EXPECT_TRUE(hasOriginalData());

    if (externalizeBeforeRedo)
    {
        EXPECT_TRUE(doExternalize());
    }

    ASSERT_TRUE(obj->applyDiff(diff));

    EXPECT_TRUE(hasChangedData());
}

TEST_F(TestGtExternalizedObject, m1_before_fetch_and_m2_and_diff_after_externalizing)
{
    ASSERT_TRUE(doSetupObject());

    EXPECT_TRUE(doExternalize());

    auto m1 = obj->toMemento();

    ASSERT_TRUE(doChangeData());

    EXPECT_TRUE(doExternalize());

    auto m2 = obj->toMemento();

    GtObjectMementoDiff diff{m1, m2};

    // diff creation failed as both m1 and m2 contain externalized states of the object
    ASSERT_TRUE(diff.isNull());
}

