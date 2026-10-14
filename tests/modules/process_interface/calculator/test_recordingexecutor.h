/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_RECORDINGEXECUTOR_H
#define TEST_RECORDINGEXECUTOR_H

#include "gt_abstractcalculatorexecutor.h"

#include <QString>

namespace test
{
/// Appends a line to the file named by GTLAB_TEST_RECORDING_LOG, if set
void recordingLog(const QString& line);
} // namespace test

/**
 * @brief Test executor that records every execution with a logging observer.
 *
 * Calculators record their run(), tasks their iteration. Each component
 * observes its own linked objects.
 */
class TestRecordingExecutor : public GtAbstractCalculatorExecutor
{
    Q_OBJECT

public:
    Q_INVOKABLE TestRecordingExecutor();

    bool exec(GtCalculator* calculator) override;
    bool exec(GtTask* task) override;
};

#endif // TEST_RECORDINGEXECUTOR_H
