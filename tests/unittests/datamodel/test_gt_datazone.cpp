/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_datazone.cpp
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
#include "gt_datazone.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_testhelper.h"
#include "gt_table.h"

/// This is a test fixture that does a init for each test
class TestGtDataZone : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        dz = new GtDataZone;

        m_params = gtTestHelper->randomStringList(m_paramLength);
        m_units  = gtTestHelper->randomStringList(m_paramLength);

        m_ticks1 = gtTestHelper->linearDataVector(m_axis1Size, 0.0, 2.0);
        m_ticks2 = gtTestHelper->linearDataVector(m_axis2Size, 1.0, 2.0);
    }

    virtual void TearDown()
    {
        delete dz;
    }

    QMap<QString, QVector<double>> generateValues(int size)
    {
        QMap<QString, QVector<double>> data;

        for (int i = 0; i < m_paramLength; ++i)
        {
            data.insert(m_params.at(i), gtTestHelper->randomDataVector(size));
        }

        return data;
    }

    bool set1DData()
    {
        auto data = dz->fetchData();
        data.setData1D(m_params, m_ticks1, "axis1",
                       generateValues(m_axis1Size), m_units);
        return data.isValid() && dz->nDims() == 1;
    }

    bool set2DData()
    {
        auto data = dz->fetchData();
        data.setData2D(m_params, m_ticks1, m_ticks2, "axis1", "axis2",
                       generateValues(m_axis1Size * m_axis2Size), m_units);
        return data.isValid() && dz->nDims() == 2;
    }

    GtDataZone* dz{};
    int m_paramLength{10};
    int m_axis1Size{5};
    int m_axis2Size{10};

    QStringList m_params;
    QStringList m_units;
    QVector<double> m_ticks1;
    QVector<double> m_ticks2;
};

TEST_F(TestGtDataZone, is0D)
{
    EXPECT_FALSE(dz->is0D());
}

TEST_F(TestGtDataZone, nDimsEmpty)
{
    EXPECT_EQ(dz->nDims(), 0);
}

TEST_F(TestGtDataZone, nDims1D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());
    EXPECT_EQ(dz->nDims(), 1);
}

TEST_F(TestGtDataZone, nDims2D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set2DData());
    EXPECT_EQ(dz->nDims(), 2);
}

TEST_F(TestGtDataZone, isValidEmpty)
{
    // empty dz
    auto data = dz->fetchData();
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, isValid1D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, isValid2D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set2DData());
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, minValue1D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    auto tabVals = dz->findDirectChild<GtTable*>()->tabVals(m_params.at(0));

    auto* min = std::min_element(std::begin(tabVals), std::end(tabVals));
    bool success = true;

    EXPECT_NE(min, std::end(tabVals));
    EXPECT_EQ(*min, data.minValue1D(m_params.at(0), &success));
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, maxValue1D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    auto tabVals = dz->findDirectChild<GtTable*>()->tabVals(m_params.at(0));

    auto* max = std::max_element(std::begin(tabVals), std::end(tabVals));
    bool success = true;

    EXPECT_NE(max, std::end(tabVals));
    EXPECT_EQ(*max, data.maxValue1D(m_params.at(0), &success));
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, minValue2D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set2DData());

    auto tabVals = dz->findDirectChild<GtTable*>()->tabVals(m_params.at(0));

    auto* min = std::min_element(std::begin(tabVals), std::end(tabVals));
    bool success = true;

    EXPECT_NE(min, std::end(tabVals));
    EXPECT_EQ(*min, data.minValue2D(m_params.at(0), &success));
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, maxValue2D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set2DData());

    auto tabVals = dz->findDirectChild<GtTable*>()->tabVals(m_params.at(0));

    auto* max = std::max_element(std::begin(tabVals), std::end(tabVals));
    bool success = true;

    EXPECT_NE(max, std::end(tabVals));
    EXPECT_EQ(*max, data.maxValue2D(m_params.at(0), &success));
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, setData1DSelfAsign)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    // test self assignment of params and units
    data.setData1D(data.params(), m_ticks1, "axis1",
                   generateValues(m_axis1Size), data.units());

    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, setData2DSelfAsign)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    // test self assignment of params and units
    data.setData2D(data.params(), m_ticks1, m_ticks2, "axis1", "axis2",
                   generateValues(m_axis1Size * m_axis2Size), data.units());

    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, setData1DNullTable)
{
    auto data = dz->fetchData();
    // hacky way to delete table
    delete dz->findDirectChild<GtTable*>();

    // table should be created
    EXPECT_TRUE(set1DData());
    EXPECT_NE(dz->findDirectChild<GtTable*>(), nullptr);
}

TEST_F(TestGtDataZone, setData2DNullTable)
{
    auto data = dz->fetchData();
    // hacky way to delete table
    delete dz->findDirectChild<GtTable*>();

    // table should be created
    EXPECT_TRUE(set2DData());
    EXPECT_NE(dz->findDirectChild<GtTable*>(), nullptr);
}

TEST_F(TestGtDataZone, set1DDataNoTicks)
{
    auto data = dz->fetchData();

    QMap<double, QVector<double>> values;

    for (int i = 0; i < m_paramLength; ++i)
    {
        values.insert(i, gtTestHelper->linearDataVector(m_axis1Size, 0.0, 1.0));
    }

    data.setData1D(data.params(), values, "axis1", data.units());

    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, addModuleName)
{
    // empty dz
    auto data   = dz->fetchData();
    ASSERT_TRUE(set2DData());
    data.addModuleName("test");

    auto& params = data.params();
    EXPECT_TRUE(std::all_of(std::begin(params), std::end(params),
                            [](auto& param) {
        return param.startsWith("test.");
    }));
    auto tabValKeys = dz->tabValsKeys();
    EXPECT_TRUE(std::all_of(std::begin(tabValKeys), std::end(tabValKeys),
                            [](auto& param) {
        return param.startsWith("test.");
    }));
}

TEST_F(TestGtDataZone, set1DFrom2D)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set2DData());

    EXPECT_EQ(dz->nDims(), 2);
    data.setData1Dfrom2DDataZone(dz, 0, 0);

    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 1);
}

