/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_datazone0d.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 04.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include <QDebug>
#include <QMap>

#include "gtest/gtest.h"
#include "gt_datazonetable.h"
#include "gt_datazone0d.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_testhelper.h"

/// This is a test fixture that does a init for each test
class TestGtDataZone0D : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        dz = new GtDataZone0D;

        m_params = gtTestHelper->randomStringList(m_paramLength);
        m_units  = gtTestHelper->randomStringList(m_paramLength);
        m_values = gtTestHelper->randomDataVector(m_paramLength);
    }

    virtual void TearDown()
    {
        delete dz;
    }

    bool setData()
    {
        auto data = dz->fetchData();
        return data.setData(m_params, m_values, m_units);
    }

    GtDataZone0D* dz{};
    int m_paramLength{10};

    QStringList m_params;
    QStringList m_units;
    QVector<double> m_values;
};

TEST_F(TestGtDataZone0D, is0D)
{
    EXPECT_TRUE(dz->is0D());
}

TEST_F(TestGtDataZone0D, nDims)
{
    EXPECT_EQ(dz->nDims(), 0);
}

TEST_F(TestGtDataZone0D, isValidEmpty)
{
    // empty dz
    auto data = dz->fetchData();
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone0D, isValid0D)
{
    // empty dz
    auto data = dz->fetchData();
    ASSERT_TRUE(setData());
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone0D, addModuleName)
{
    // empty dz
    auto data   = dz->fetchData();
    ASSERT_TRUE(setData());
    data.addModuleName("test");

    auto& params = data.params();
    EXPECT_TRUE(std::all_of(std::begin(params), std::end(params),
                            [](auto& param) {
        return param.startsWith("test.");
    }));
}

TEST_F(TestGtDataZone0D, unit)
{
    // empty dz
    auto data   = dz->fetchData();

    m_units.first() = "[s]";
    ASSERT_TRUE(setData());

    EXPECT_EQ(data.unit(m_params.at(0)), QString{"[s]"});
}
