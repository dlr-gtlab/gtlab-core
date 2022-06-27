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
        m_ticks3 = gtTestHelper->linearDataVector(m_axis3Size, 0.1, 1.1);
        m_ticks4 = gtTestHelper->linearDataVector(m_axis4Size, 1.2, 4.1);
    }

    virtual void TearDown()
    {
        delete dz;
    }

    QMap<QString, QVector<double>> generateValues(int size)
    {
        m_values.clear();
        for (int i = 0; i < m_paramLength; ++i)
        {
            m_values.insert(m_params.at(i),
                            gtTestHelper->randomDataVector(size));
        }

        return m_values;
    }

    bool set1DData()
    {
        auto data = dz->fetchData();
        return data.setData1D(m_params, m_ticks1, "axis1",
                              generateValues(m_axis1Size), m_units) &&
               data.isValid() && dz->nDims() == 1;
    }

    bool set2DData()
    {
        auto data = dz->fetchData();
        return data.setData2D(m_params, m_ticks1, m_ticks2, "axis1", "axis2",
                              generateValues(m_axis1Size * m_axis2Size),
                              m_units) &&
               data.isValid() && dz->nDims() == 2;
    }

    GtDataZone* dz{};
    int m_paramLength{10};
    int m_axis1Size{4};
    int m_axis2Size{5};
    int m_axis3Size{6};
    int m_axis4Size{7};

    QStringList m_params;
    QStringList m_units;
    QVector<double> m_ticks1;
    QVector<double> m_ticks2;
    QVector<double> m_ticks3;
    QVector<double> m_ticks4;

    QMap<QString, QVector<double>> m_values;
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

TEST_F(TestGtDataZone, tabValsKeys)
{
    EXPECT_EQ(dz->tabValsKeys(), QStringList{});
    auto data = dz->fetchData();
    EXPECT_EQ(dz->tabValsKeys(), data.params());

    ASSERT_TRUE(set2DData());

    auto keys = dz->tabValsKeys();
    auto params = data.params();
    EXPECT_EQ(keys.size(), m_paramLength);
    EXPECT_EQ(params.size(), m_paramLength);

    // keys and params may have different order
    std::sort(std::begin(keys), std::end(keys));
    std::sort(std::begin(params), std::end(params));

    EXPECT_EQ(keys, params);
}

TEST_F(TestGtDataZone, axisTicks)
{
    QVector<double> ticks = dz->axisTicks("axis1");
    EXPECT_EQ(ticks, QVector<double>{});

    ASSERT_TRUE(set2DData());

    ticks = dz->axisTicks("axis1");
    EXPECT_EQ(ticks, m_ticks1);
    ticks = dz->axisTicks("axis2");
    EXPECT_EQ(ticks, m_ticks2);

    QStringList list;
    for (auto it : qAsConst(ticks))
    {
        list << QString::number(it);
    }

    EXPECT_EQ(dz->axisTickStrings("axis2"), list);

    EXPECT_EQ(dz->allAxisTicks(),
              QVector<QVector<double>>() << m_ticks1 << m_ticks2);
}

TEST_F(TestGtDataZone, axisNames)
{
    QStringList names = dz->axisNames();
    EXPECT_EQ(names, QStringList{});

    ASSERT_TRUE(set2DData());

    names = dz->axisNames();
    EXPECT_EQ(names, (QStringList{"axis1", "axis2"}));
}

TEST_F(TestGtDataZone, setData1DSelfAsign)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    // test self assignment of params and units
    EXPECT_TRUE(data.setData1D(data.params(), m_ticks1, "axis1",
                               generateValues(m_axis1Size), data.units()));

    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, setData2DSelfAsign)
{
    auto data = dz->fetchData();
    ASSERT_TRUE(set1DData());

    // test self assignment of params and units
    EXPECT_TRUE(data.setData2D(data.params(), m_ticks1, m_ticks2, "axis1",
            "axis2", generateValues(m_axis1Size * m_axis2Size), data.units()));

    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, setData1DNullTable)
{
    auto data = dz->fetchData();
    // hacky way to delete table
    ASSERT_NE(dz->findDirectChild<GtTable*>(), nullptr);
    delete dz->findDirectChild<GtTable*>();
    ASSERT_EQ(dz->findDirectChild<GtTable*>(), nullptr);

    // table should have been created
    EXPECT_TRUE(set1DData());
    EXPECT_NE(dz->findDirectChild<GtTable*>(), nullptr);
}

TEST_F(TestGtDataZone, setData2DNullTable)
{
    auto data = dz->fetchData();
    // hacky way to delete table
    delete dz->findDirectChild<GtTable*>();

    // table should have been created
    EXPECT_TRUE(set2DData());
    EXPECT_NE(dz->findDirectChild<GtTable*>(), nullptr);
}

TEST_F(TestGtDataZone, setData1DNoTicks)
{
    auto data = dz->fetchData();
    QMap<double, QVector<double>> values;

    for (int i = 0; i < m_axis1Size; ++i)
    {
        values.insert(i, gtTestHelper->linearDataVector(m_paramLength, 0., 1.));
    }

    EXPECT_TRUE(data.setData1D(m_params, values, "axis1", m_units));
    EXPECT_TRUE(data.isValid());
}

TEST_F(TestGtDataZone, setData1DNoTicksInvalid)
{
    auto data = dz->fetchData();
    QMap<double, QVector<double>> valuesInvalid;

    for (int i = 0; i < m_axis1Size; ++i)
    {
        valuesInvalid.insert(i, gtTestHelper->linearDataVector(m_axis1Size * 2,
                                                               0.0, 1.0));
    }

    EXPECT_FALSE(data.setData1D(m_params, valuesInvalid, "axis1", m_units));
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

    // set 1D using axis 0
    EXPECT_TRUE(data.setData1Dfrom2DDataZone(dz, 0, 2));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 1);

    ASSERT_TRUE(set2DData());
    EXPECT_EQ(dz->nDims(), 2);

    // set 1D using axis 1
    EXPECT_TRUE(data.setData1Dfrom2DDataZone(dz, 1, 2));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 1);
}

TEST_F(TestGtDataZone, setDataAndGetValue1D)
{
    auto data = dz->fetchData();
    QMap< QString, QVector<double> > axis;
    axis.insert("axis1", m_ticks1);
    EXPECT_TRUE(data.setData(m_params, axis, generateValues(m_axis1Size),
                             m_units));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 1);
    ASSERT_EQ(m_values.size(), m_paramLength);
    ASSERT_EQ(m_values.first().size(), m_axis1Size);

    // read values
    bool success = true;
    int idx = 0;
    QString param = data.params()[0];

    EXPECT_EQ(data.value1D(param, m_ticks1.at(idx), &success),
              m_values.value(param)[idx]);
    EXPECT_TRUE(success);

    idx = 3;
    param = data.params()[1];
    EXPECT_EQ(data.value1D(param, m_ticks1.at(idx), &success),
              m_values.value(param)[idx]);
    EXPECT_TRUE(success);

    // invalid param
    data.value1D(param + param, m_ticks1.at(idx), &success);
    EXPECT_FALSE(success);
}

TEST_F(TestGtDataZone, setDataAndGetValue2D)
{
    auto data = dz->fetchData();
    QMap< QString, QVector<double> > axis;
    axis.insert("axis1", m_ticks1);
    axis.insert("axis2", m_ticks2);
    EXPECT_TRUE(data.setData(m_params, axis,
                             generateValues(m_axis1Size * m_axis2Size),
                             m_units));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 2);
    ASSERT_EQ(m_values.size(), m_paramLength);
    ASSERT_EQ(m_values.first().size(), m_axis1Size * m_axis2Size);

    // read values
    bool success = true;
    int idx1 = 1;
    int idx2 = 2;
    QString param = data.params()[3];

    // 1fst test
    EXPECT_EQ(data.value2D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2), &success),
              m_values.value(param)[idx1 + idx2 * m_axis1Size]);
    EXPECT_TRUE(success);

    // 2nd test
    idx1 = 3;
    idx2 = 4;
    param = data.params()[1];
    EXPECT_EQ(data.value2D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2), &success),
              m_values.value(param)[idx1 + idx2 * m_axis1Size]);
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, setDataAndGetValue3D)
{
    auto data = dz->fetchData();
    QMap< QString, QVector<double> > axis;
    axis.insert("axis1", m_ticks1);
    axis.insert("axis2", m_ticks2);
    axis.insert("axis3", m_ticks3);
    EXPECT_TRUE(data.setData(m_params, axis,
                             generateValues(m_axis1Size * m_axis2Size *
                                            m_axis3Size),
                             m_units));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 3);
    ASSERT_EQ(m_values.size(), m_paramLength);
    ASSERT_EQ(m_values.first().size(), m_axis1Size * m_axis2Size * m_axis3Size);

    // read values
    bool success = true;
    int idx1 = 1;
    int idx2 = 0;
    int idx3 = 4;
    QString param = data.params()[5];

    // 1fst test
    EXPECT_EQ(data.value3D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2),
                           m_ticks3.at(idx3), &success),
              m_values.value(param)[idx1 +
                                    idx2 * m_axis1Size +
                                    idx3 * m_axis1Size * m_axis2Size]);
    EXPECT_TRUE(success);

    // 2nd test
    idx1 = 3;
    idx2 = 4;
    idx3 = 0;
    param = data.params()[7];
    EXPECT_EQ(data.value3D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2),
                           m_ticks3.at(idx3), &success),
              m_values.value(param)[idx1 +
                                    idx2 * m_axis1Size +
                                    idx3 * m_axis1Size * m_axis2Size]);
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, setDataAndGetValue4D)
{
    auto data = dz->fetchData();
    QMap< QString, QVector<double> > axis;
    axis.insert("axis1", m_ticks1);
    axis.insert("axis2", m_ticks2);
    axis.insert("axis3", m_ticks3);
    axis.insert("axis4", m_ticks4);
    EXPECT_TRUE(data.setData(m_params, axis,
                             generateValues(m_axis1Size * m_axis2Size *
                                            m_axis3Size * m_axis4Size),
                             m_units));
    EXPECT_TRUE(data.isValid());
    EXPECT_EQ(dz->nDims(), 4);
    ASSERT_EQ(m_values.size(), m_paramLength);
    ASSERT_EQ(m_values.first().size(),
              m_axis1Size * m_axis2Size * m_axis3Size * m_axis4Size);

    // read values
    bool success = true;
    int idx1 = 1;
    int idx2 = 4;
    int idx3 = 2;
    int idx4 = 6;
    QString param = data.params()[5];

    // 1fst test
    EXPECT_EQ(data.value4D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2),
                           m_ticks3.at(idx3), m_ticks4.at(idx4), &success),
              m_values.value(param)[idx1 +
                                    idx2 * m_axis1Size +
                                    idx3 * m_axis1Size * m_axis2Size +
                                    idx4 * m_axis1Size * m_axis2Size *
                                    m_axis3Size]);
    EXPECT_TRUE(success);

    // 2nd test
    idx1 = 2;
    idx2 = 2;
    idx3 = 1;
    idx3 = 5;
    param = data.params()[7];
    EXPECT_EQ(data.value4D(param,
                           m_ticks1.at(idx1), m_ticks2.at(idx2),
                           m_ticks3.at(idx3), m_ticks4.at(idx4), &success),
              m_values.value(param)[idx1 +
                                    idx2 * m_axis1Size +
                                    idx3 * m_axis1Size * m_axis2Size +
                                    idx4 * m_axis1Size * m_axis2Size *
                                    m_axis3Size]);
    EXPECT_TRUE(success);
}

TEST_F(TestGtDataZone, addDataPoint1D)
{
    auto data = dz->fetchData();

    QStringList params = {"ParamA", "ParamB", "ParamC"};
    QStringList units = {"[kg]", "[K]", "[-]"};
    QVector<double> ticks = {0.0, 0.5, 1.0};
    QString axisName = "x";
    QMap<QString, QVector<double> > vals;

    vals.insert(params[0], {0.0, 1.0, 2.0});
    vals.insert(params[1], {3.0, 4.0, 5.0});
    vals.insert(params[2], {6.0, 7.0, 8.0});


    data.setData1D(params, ticks, axisName, vals, units);

    bool check = data.isValid() && dz->nDims() == 1;

    ASSERT_TRUE(check);
    ASSERT_EQ(data.value1D(params[0], 0.75), 1.5);


    QMap<QString, double> pointToAdd;
    pointToAdd.insert(params[0], 12.0);
    pointToAdd.insert(params[1], 2.0);
    pointToAdd.insert(params[2], 1.0);

    const double axValue = 0.75;
    ASSERT_TRUE(data.addDataPoint1D(pointToAdd, axValue));

    //qDebug() << "A: relevant parametervector" << data.value1DVector(params[0]);
    //qDebug() << "A: relevant ticks" << dz->axisTicks();

    ASSERT_EQ(data.value1D(params[0], axValue), 12.0);

    QMap<QString, double> pointToAdd2;
    pointToAdd2.insert(params[0], -12.0);
    pointToAdd2.insert(params[1], -2.0);
    pointToAdd2.insert(params[2], -1.0);

    const double axValue2 = 0.5;
    ASSERT_FALSE(data.addDataPoint1D(pointToAdd2, axValue2, false));
    ASSERT_TRUE(data.addDataPoint1D(pointToAdd2, axValue2, true));

    //qDebug() << "B: relevant parametervector" << data.value1DVector(params[0]);
    //qDebug() << "B: relevant ticks" << dz->axisTicks();

    ASSERT_EQ(data.value1D(params[0], axValue2), -12.0);


    QMap<QString, double> pointToAdd3;
    pointToAdd3.insert(params[0], 151.0);
    pointToAdd3.insert(params[1], 123.0);
    pointToAdd3.insert(params[2], 34563.0);

    const double axValue3 = -1.0;
    ASSERT_TRUE(data.addDataPoint1D(pointToAdd3, axValue3));

    //qDebug() << "C: relevant parametervector" << data.value1DVector(params[1]);
    //qDebug() << "C: relevant ticks" << dz->axisTicks();

    ASSERT_EQ(data.value1D(params[1], axValue3), 123.0);

    QMap<QString, double> pointToAdd4;
    pointToAdd4.insert(params[0], -151.0);
    pointToAdd4.insert(params[1], -123.0);
    pointToAdd4.insert(params[2], -34563.0);

    const double axValue4 = 10.0;
    ASSERT_TRUE(data.addDataPoint1D(pointToAdd4, axValue4));

    //qDebug() << "D: relevant parametervector" << data.value1DVector(params[2]);
    //qDebug() << "D: relevant ticks" << dz->axisTicks();

    ASSERT_EQ(data.value1D(params[2], axValue4), -34563.0);

}

