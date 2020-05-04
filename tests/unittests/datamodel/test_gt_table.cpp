/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_table
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.10.2016
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_table.h"

/// This is a test fixture that does a init for each test
class TestGtTable : public ::testing::Test , public GtTable
{
protected:
    virtual void SetUp()
    {
        // m_table: empty Table
        m_table = new GtTable();
        // m_table2: table with 2 axes, without values
        m_table2 = new GtTable();
        // m_table3: table with 2 axes, with values
        m_table3 = new GtTable();
        // m_table4: table with one axis, without values
        m_table4 = new GtTable();
        // m_table5: table with two axis, with values with tableName
        m_table5 = new GtTable();
        // m_table6: table with three axis, with values
        m_table6 = new GtTable();
        // m_table7: table with four axis, with values
        m_table7 = new GtTable();
        // m_table8: table with one axis and values
        m_table8 = new GtTable();



        m_table->setObjectName("Table1");
        m_table2->setObjectName("Table2");
        m_table3->setObjectName("Table3");
        m_table4->setObjectName("Table4");
        m_table5->setObjectName("Table5");
        m_table6->setObjectName("Table6");
        m_table7->setObjectName("Table7");
        m_table8->setObjectName("Table8");


        QVector<double> ticks;
        ticks.append(0);
        ticks.append(1);
        ticks.append(2);

        QString axisID = "axisID";
        QString desc = "descritption";
        QString unit = "Kelvin";
        GtTableAxis::ExtrapMethod loExMethod;
        GtTableAxis::InterpMethod intMethod;
        GtTableAxis::ExtrapMethod hiExtMethod;

        loExMethod = GtTableAxis::ExtrapMethod::E_LINEAR;
        intMethod = GtTableAxis::InterpMethod::I_LINEAR;
        hiExtMethod = GtTableAxis::ExtrapMethod::E_LINEAR;

        m_table2->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table3->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table4->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table5->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table6->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table7->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table8->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);



        QVector<double> ticks2;
        ticks2.append(0);
        ticks2.append(3);
        ticks2.append(5);

        QString axisID2 = "axisID2";
        QString desc2 = "descritption2";
        QString unit2 = "Kelvin2";
        m_table2->addAxis(axisID2, desc2, unit2, loExMethod, intMethod,
                          hiExtMethod, ticks2);
        m_table3->addAxis(axisID2, desc2, unit2, loExMethod, intMethod,
                          hiExtMethod, ticks2);
        m_table5->addAxis(axisID2, desc2, unit2, loExMethod, intMethod,
                          hiExtMethod, ticks2);
        m_table6->addAxis(axisID2, desc2, unit2, loExMethod, intMethod,
                          hiExtMethod, ticks2);
        m_table6->addAxis("axisID3", desc2, unit2, loExMethod, intMethod,
                          hiExtMethod, ticks2);
        m_table7->addAxis(axisID, desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table7->addAxis("axisID3", desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);
        m_table7->addAxis("axisID4", desc, unit, loExMethod, intMethod,
                          hiExtMethod, ticks);

        QString valuesID = "valuesID";
        QString valuesDesc = "valuesDescription";
        QString valuesUnit = "Fahrenheit";

        QVector <double> values;

        for (int i = 0; i < 3; i++)
        {
            values.append(i);
        }
        m_table8->addValues(valuesID, valuesDesc, valuesUnit, values);

        values.clear();

        for (int i = 0; i < 9; i++)
        {
            values.append(i);
        }

        m_table3->addValues(valuesID, valuesDesc, valuesUnit, values);
        m_table5->addValues("Table5", valuesDesc, valuesUnit, values);

        values.clear();
        for (int i = 0; i < 27; i++)
        {
            values.append(i);
        }
        m_table6->addValues("Table6", valuesDesc, valuesUnit, values);

        values.clear();
        for (int i = 0; i < 81; i++)
        {
            values.append(i);
        }
        m_table7->addValues("Table7", valuesDesc, valuesUnit, values);

    }

    virtual void TearDown()
    {
        delete m_table;
        m_table = Q_NULLPTR;
        delete m_table2;
        m_table2 = Q_NULLPTR;
        delete m_table3;
        m_table3 = Q_NULLPTR;
        delete m_table4;
        m_table4 = Q_NULLPTR;
        delete m_table5;
        m_table5 = Q_NULLPTR;
        delete m_table6;
        m_table6 = Q_NULLPTR;
        delete m_table7;
        m_table7 = Q_NULLPTR;
        delete m_table8;
        m_table8 = Q_NULLPTR;
    }

    GtTable* m_table;
    GtTable* m_table2;
    GtTable* m_table3;
    GtTable* m_table4;
    GtTable* m_table5;
    GtTable* m_table6;
    GtTable* m_table7;
    GtTable* m_table8;

};

TEST_F(TestGtTable, ticks)
{
    GtTableAxis* axis = m_table3->findDirectChild<GtTableAxis*>("axisID");
    ASSERT_TRUE(axis != Q_NULLPTR);

    QVector <double> ticks = axis->ticks();

    ASSERT_EQ(ticks.size(),3);
    //axis->ticks();
}

TEST_F(TestGtTable, addAxis)
{
    QVector<double> ticks;
    ticks.append(0);
    ticks.append(1);
    ticks.append(2);

    QString axisID = "axisID3";
    QString desc = "descritption3";
    QString unit = "Kelvin3";
    GtTableAxis::ExtrapMethod loExMethod = GtTableAxis::E_LINEAR;
    GtTableAxis::InterpMethod intMethod = GtTableAxis::I_LINEAR;
    GtTableAxis::ExtrapMethod hiExtMethod = GtTableAxis::E_LINEAR;

    ASSERT_TRUE(m_table->addAxis(axisID, desc, unit, loExMethod, intMethod,
                                 hiExtMethod, ticks));
    ticks.clear();
    ticks.append(-1);
    ticks.append(-2);

    ASSERT_FALSE(m_table4->addAxis(axisID, desc, unit, loExMethod, intMethod,
                                   hiExtMethod, ticks));
}

// checkAxisName not direct testable (private)

TEST_F(TestGtTable, addValues)
{
    QString valuesID = "valuesID";
    QString valuesDesc = "valuesDescription";
    QString valuesUnit = "Fahrenheit";

    QVector <double> values;

    for (int i = 0; i < 9; i++)
    {
        values.append(i);
    }

    QVector <double> values2;

    for (int i = 0; i < 6; i++)
    {
        values2.append(i - 5);
    }


    ASSERT_TRUE(m_table2->addValues(valuesID, valuesDesc, valuesUnit, values));
    ASSERT_FALSE(m_table2->addValues(valuesID, valuesDesc, valuesUnit,
                                     values2));
}

// approxDimension not direct testable (private)

// calcStart not direct testable (private)

// calcStride not direct testable (private)

TEST_F(TestGtTable, getAxisTicks1)
{
    ASSERT_EQ(m_table3->getAxisTicks(0).size(), 3);
    ASSERT_EQ(m_table3->getAxisTicks(1).size(), 3);

    ASSERT_EQ(m_table3->getAxisTicks(1).at(0), 0);
    ASSERT_EQ(m_table3->getAxisTicks(1).at(1), 3);
    ASSERT_EQ(m_table3->getAxisTicks(1).at(2), 5);

    ASSERT_THROW(m_table->getAxisTicks(4), GTlabException);
}

TEST_F(TestGtTable, getAxisTicks2)
{
    QVector <double> test;
    test.append(0);
    test.append(3);
    test.append(5);

    ASSERT_THROW(m_table->getAxisTicks("Test", test), GTlabException);

    QVector <double> test2;
    m_table2->getAxisTicks("axisID2", test2);

    ASSERT_EQ(test.size(), test2.size());
    ASSERT_EQ(test.at(0), test2.at(0));
    ASSERT_EQ(test.at(1), test2.at(1));
    ASSERT_EQ(test.at(2), test2.at(2));
}

TEST_F(TestGtTable, getAxisList)
{
    ASSERT_EQ(m_table3->getAxesList().size(), 2);
    ASSERT_TRUE(m_table2->getAxesList().first() != Q_NULLPTR);
    ASSERT_TRUE(m_table2->getAxesList().at(1) != Q_NULLPTR);
}

TEST_F(TestGtTable, getAxesNames)
{
    QStringList correctList;
    correctList.append("axisID");
    correctList.append("axisID2");

    QStringList wrongList;
    wrongList.append("axisIsdg");
    wrongList.append("asghasdhg");
    wrongList.append("gsgseeg");

    ASSERT_TRUE(correctList.size() == m_table2->getAxesNames().size());
    ASSERT_FALSE(wrongList.size() == m_table2->getAxesNames().size());

    QString realName = m_table2->getAxesNames().first();
    ASSERT_STREQ(realName.toStdString().c_str(),
                 correctList.first().toStdString().c_str());

    ASSERT_STRNE(realName.toStdString().c_str(),
                 wrongList.first().toStdString().c_str());


    realName = m_table2->getAxesNames().at(1);
    ASSERT_STREQ(realName.toStdString().c_str(),
                 correctList.at(1).toStdString().c_str());

    ASSERT_STRNE(realName.toStdString().c_str(),
                 wrongList.at(1).toStdString().c_str());

}

TEST_F(TestGtTable, getDescription)
{
    m_table->setDescription("description");

    QString desc = m_table->getDescription();


    ASSERT_STREQ(desc.toStdString().c_str(), "description");
    ASSERT_STRNE(desc.toStdString().c_str(), "bla");
}

// getSlice not direct testable (private)

TEST_F(TestGtTable, getTableSize)
{
    ASSERT_EQ(m_table3->getTableSize(), 1);

    ASSERT_NE(m_table->getTableSize(), 1);
}

TEST_F(TestGtTable, getUnit)
{
    m_table->setUnit("meter");

    QString unit = m_table->getUnit();

    ASSERT_STREQ(unit.toStdString().c_str(), "meter");
    ASSERT_STRNE(unit.toStdString().c_str(), "Pa");
}

TEST_F(TestGtTable, getVals)
{
    ASSERT_TRUE(m_table3->getVals("valuesID") != Q_NULLPTR);

    ASSERT_THROW(m_table3->getVals("sdgasdg"), GTlabException);
}

TEST_F(TestGtTable, getValue)
{
    QVector <double> coords;
    coords.append(0);

    QString correctId = "valuesID";
    QString wrongId = "vaasdgashgID";

    // coords size not equal to dimension of table
    ASSERT_THROW(m_table3->getValue(correctId, coords), GTlabException);

    coords.append(3);
    ASSERT_EQ(m_table3->nDims(), coords.size());

    // values-Id not valid
    ASSERT_THROW(m_table3->getValue(wrongId, coords), GTlabException);

    // USING TABLE ID TO FIND VALUE
    ASSERT_THROW(m_table3->getValue("Table3", coords), GTlabException);
    ASSERT_EQ(m_table5->getValue("Table5", coords), 3);

}

TEST_F(TestGtTable, getValue1D)
{
    ASSERT_EQ(m_table8->getValue1D("valuesID",2),2);
    ASSERT_NE(m_table8->getValue1D("valuesID",2),1);
}

TEST_F(TestGtTable, getValue2D)
{
    ASSERT_EQ(m_table3->nDims(), 2);
    ASSERT_EQ(m_table3->getValue2D("valuesID", 1, 3), 4);
    ASSERT_EQ(m_table3->getValue2D("valuesID", 0, 0), 0);
    ASSERT_THROW(m_table->getValue2D("Test", 0, 0), GTlabException);
}

TEST_F(TestGtTable, getValue3D)
{
    ASSERT_EQ(m_table6->getValue3D("Table6", 0, 0, 0), 0);
    ASSERT_THROW(m_table->getValue3D("Test", 0, 0, 0), GTlabException);
}

TEST_F(TestGtTable, getValue4D)
{
    ASSERT_THROW(m_table7->getValue4D("Test", 0, 0, 0, 0), GTlabException);
    ASSERT_THROW(m_table->getValue4D("Test", 0, 0, 0, 0), GTlabException);
}

TEST_F(TestGtTable, getValue5D)
{
    qDebug() << "Funcion getValue5D of GtTable is an empty function";

    ASSERT_EQ(m_table->getValue5D("Test", 1,2,3,4,5),0);
}

// isAscending not direct testable (private)

TEST_F(TestGtTable, scaleAxis)
{
    double factor = 2;
    QString name = "axisID2";

    ASSERT_THROW(m_table->scaleAxis("bla", factor), GTlabException);
    QVector<double> compareValues;

    compareValues.append(0);
    compareValues.append(6);
    compareValues.append(10);

    m_table3->scaleAxis(name,factor);

    QStringList nameList = m_table3->getAxesNames();

    int axisIndex = nameList.indexOf(name);

    ASSERT_NE(axisIndex, -1);

    GtTableAxis* currentAxis = m_table3->getAxesList().at(axisIndex);

    QVector <double> values = currentAxis->ticks();

    ASSERT_EQ(values.at(0), compareValues.at(0));
    ASSERT_EQ(values.at(1), compareValues.at(1));
    ASSERT_EQ(values.at(2), compareValues.at(2));
    currentAxis = Q_NULLPTR;
}

TEST_F(TestGtTable, scaleValues)
{
    QVector<double> compareValues;
    double factor = 2;

    for (int i = 0; i < 9; i++)
    {
        compareValues.append(i * factor);
    }

    ASSERT_THROW(m_table->scaleValues("bla", factor), GTlabException);

    m_table3->scaleValues("valuesID", factor);
    QVector <double> tableValues = m_table3->getVals("valuesID")->values();

    ASSERT_EQ(tableValues.at(0), compareValues.at(0));
    ASSERT_EQ(tableValues.at(2), compareValues.at(2));
    ASSERT_EQ(tableValues.at(4), compareValues.at(4));
    ASSERT_EQ(tableValues.at(6), compareValues.at(6));
}

TEST_F(TestGtTable, setDescription)
{
    m_table->setDescription("description");

    QString desc = m_table->getDescription();


    ASSERT_STREQ(desc.toStdString().c_str(), "description");
    ASSERT_STRNE(desc.toStdString().c_str(), "bla");
}

TEST_F(TestGtTable, setName)
{
    m_table->setName("TestName");

    ASSERT_STREQ(m_table->objectName().toStdString().c_str(), "TestName");
    ASSERT_STRNE(m_table->objectName().toStdString().c_str(), "testname");
}

TEST_F(TestGtTable, setUnit)
{
    m_table->setUnit("meter");

    QString unit = m_table->getUnit();


    ASSERT_STREQ(unit.toStdString().c_str(), "meter");
    ASSERT_STRNE(unit.toStdString().c_str(), "Pa");
}

TEST_F(TestGtTable, tabVals)
{
    ASSERT_EQ(m_table3->tabVals("valuesID").at(4), 4);

    ASSERT_EQ(m_table5->tabVals("Table5").at(4), 4);

    ASSERT_NE(m_table3->tabVals("valuesID").at(7), 3);

    ASSERT_THROW(m_table->tabVals("bla"), GTlabException);
}

TEST_F(TestGtTable, valDescription)
{
    ASSERT_STREQ(m_table3->valDescription("valuesID").toStdString().c_str(),
                 "valuesDescription");
    ASSERT_THROW(m_table->valDescription("bla"), GTlabException);
}

TEST_F(TestGtTable, valUnit)
{
    ASSERT_STREQ(m_table3->valUnit("valuesID").toStdString().c_str(),
                 "Fahrenheit");
    ASSERT_THROW(m_table->valUnit("bla"), GTlabException);
}

TEST_F(TestGtTable, tabValsKeys)
{
    ASSERT_STREQ(m_table3->tabValsKeys().first().toStdString().c_str(),
                 "valuesID");

    ASSERT_EQ(m_table->tabValsKeys().size(), 0);
}

TEST_F(TestGtTable, toDomElement)
{
    QDomDocument dom;

    ASSERT_THROW(m_table->toDomElement(dom), GTlabException);

    ASSERT_THROW(m_table3->toDomElement(dom), GTlabException);

    ASSERT_NO_THROW(m_table3->toDomElement(dom, "valuesID"));

    ASSERT_NO_THROW(m_table5->toDomElement(dom));
}

// getValSlices not direct testable (private)

// slice not direct testable (private)

// axisIndex not direct testable (private)

// onObjectDataMerged not direct testable (private)

// deleteTabVals (protected)
//TEST_F(TestGtTable, deleteTabVals)
//{
//    ASSERT_NE(m_table2->tabValsKeys().size(),0);
//    m_table3->deleteTabVals();
//    ASSERT_EQ(m_table2->tabValsKeys().size(),0);
//}

TEST_F(TestGtTable, nDims)
{
    ASSERT_EQ(m_table3->nDims(), 2);
    ASSERT_NE(m_table3->nDims(), 1);
    ASSERT_EQ(m_table->nDims(), 0);
}

TEST_F(TestGtTable, getAxisSize)
{
    ASSERT_EQ(m_table2->getAxisSize(0), 3);
    ASSERT_EQ(m_table3->getAxisSize(0), 3);
    // EXPECT FAILURE AND RETURN-Value 0:
    ASSERT_EQ(m_table2->getAxisSize(3), 0);
    ASSERT_EQ(m_table->getAxisSize(0), 0);
}




