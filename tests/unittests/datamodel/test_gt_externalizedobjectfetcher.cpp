/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include "test_externalizedobject.h"
#include "gt_externalizedobjectfetcher.h"
#include "gt_testhelper.h"
#include "gt_externalizationmanager.h"

/// This is a test fixture that does a init for each test
class TestGtExternalizedObjectFetcher : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        gtExternalizationManager->enableExternalization(true);

        m_obj = new TestExternalizedObject();

        m_data = gtTestHelper->linearDataVector<double>(m_length, 0.0, 2.0);
        m_obj->fetchData().setValues(m_data);

        ASSERT_TRUE(m_obj->externalize());
    }

    virtual void TearDown()
    {
        fetcher.clear();
        delete m_obj;
    }

    TestExternalizedObject* m_obj{};
    GtExternalizedObjectFetcher<TestExternalizedObject> fetcher{};

    int m_length{10};
    QVector<double> m_data;
};

/// helper obect should not hold any object by default
TEST_F(TestGtExternalizedObjectFetcher, initialization)
{
    EXPECT_FALSE(fetcher.isValid());
    EXPECT_EQ(fetcher.get(), nullptr);
    EXPECT_EQ(fetcher.data(), nullptr);
}

/// setting an object will automatically fetch the data
/// clearing will release the data but not internal pointer
/// calling fetch will internalize the data
TEST_F(TestGtExternalizedObjectFetcher, setAndClear)
{
    // obj is not fetched
    EXPECT_FALSE(m_obj->isFetched());
    EXPECT_EQ(m_obj->refCount(), 0);

    // should fetch obj
    fetcher.set(m_obj);

    EXPECT_EQ(fetcher.get(), m_obj);
    EXPECT_TRUE(fetcher.isValid());

    EXPECT_TRUE(m_obj->isFetched());
    EXPECT_EQ(m_obj->refCount(), 1);
    EXPECT_NE(fetcher.data(), nullptr);

    // should release obj
    fetcher.clear();

    EXPECT_FALSE(m_obj->isFetched());
    EXPECT_EQ(m_obj->refCount(), 0);

    EXPECT_FALSE(fetcher.isValid());
    EXPECT_EQ(fetcher.get(), m_obj);
    EXPECT_EQ(fetcher.data(), nullptr);

    fetcher.fetch();

    EXPECT_EQ(fetcher.get(), m_obj);
    EXPECT_TRUE(fetcher.isValid());

    EXPECT_TRUE(m_obj->isFetched());
    EXPECT_EQ(m_obj->refCount(), 1);
    EXPECT_NE(fetcher.data(), nullptr);
}
