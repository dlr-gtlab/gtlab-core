/* GTlab - Gas Turbine laboratory
 * Source File: test_process_interface
 * copyright 2009-2022 by DLR
 *
 * Created on: 20.05.2022
 * Author: J. Schmeink
 */

#include "test_calculator.h"

#include "test_process_interface.h"

GtVersionNumber
TestProcessInterface::version()
{
    return GtVersionNumber(0, 2, 0);
}

QString
TestProcessInterface::ident() const
{
    return "Test Process Interface";
}

QString
TestProcessInterface::description() const
{
    return "Test Process Interface Description";
}

QList<GtCalculatorData>
TestProcessInterface::calculators()
{
    QList<GtCalculatorData> metaData;
    GtCalculatorData test = GT_CALC_DATA(TestCalculator);
    test->id = QStringLiteral("Simple Test Calculator");
    test->version = GtVersionNumber(0, 1);
    test->author = QStringLiteral("Jens Schmeink");
    test->category = QStringLiteral("Testing");
    test->description = QStringLiteral("Calculates the square "
                                       "of a given value");
    test->status = GtCalculatorDataImpl::RELEASE;
    metaData << test;


    return metaData;
}

QList<GtTaskData>
TestProcessInterface::tasks()
{
    QList<GtTaskData> metaData;

    return metaData;
}


