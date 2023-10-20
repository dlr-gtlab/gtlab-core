/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_process_interface
 *
 * Created on: 20.05.2022
 * Author: J. Schmeink
 */

#include "test_calculator.h"
#include "test_processstatescalculator.h"
#include "test_crashonexecutecalculator.h"
#include "test_sleepcalculator.h"
#include "test_progresscalculator.h"

#include "test_process_interface.h"

GtVersionNumber
TestProcessInterface::version()
{
    return GtVersionNumber(0, 2, 0);
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

    GtCalculatorData states = GT_CALC_DATA(TestProcessStatesCalculator);
    states->id = QStringLiteral("Process State Changer");
    states->version = GtVersionNumber(0, 1);
    states->author = QStringLiteral("Marius Bröcker");
    states->category = QStringLiteral("Testing");
    states->description = QStringLiteral("Changes calculator states in a "
                                         "fixed interval");
    states->status = GtCalculatorDataImpl::RELEASE;
    metaData << states;

    GtCalculatorData sleeper = GT_CALC_DATA(TestSleepCalculator);
    sleeper->id = QStringLiteral("Sleep Calculator");
    sleeper->version = GtVersionNumber(0, 1);
    sleeper->author = QStringLiteral("Marius Bröcker");
    sleeper->category = QStringLiteral("Testing");
    sleeper->description = QStringLiteral("Sleeps for a fixed time");
    sleeper->status = GtCalculatorDataImpl::RELEASE;
    metaData << sleeper;

    GtCalculatorData crash = GT_CALC_DATA(TestCrashOnExecuteCalculator);
    crash->id = QStringLiteral("Crash On Execution");
    crash->version = GtVersionNumber(0, 1);
    crash->author = QStringLiteral("Marius Bröcker");
    crash->category = QStringLiteral("Testing");
    crash->description = QStringLiteral("Crashes GTlab on execution :(");
    crash->status = GtCalculatorDataImpl::PROTOTYPE;
    metaData << crash;

    GtCalculatorData progressTest = GT_CALC_DATA(TestProgressCalculator);
    progressTest->id = QStringLiteral("Progress Test");
    progressTest->version = GtVersionNumber(0, 1);
    progressTest->author = QStringLiteral("Jens Schmeink");
    progressTest->category = QStringLiteral("Testing");
    progressTest->description = QStringLiteral("Test progress animation");
    progressTest->status = GtCalculatorDataImpl::RELEASE;
    metaData << progressTest;

    return metaData;
}
