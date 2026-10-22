/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_recordingtask.h"

#include "test_dmi_package.h"
#include "test_recordingexecutor.h"

#include "gt_recordingsession.h"

TestRecordingTask::TestRecordingTask() :
    m_target("target", tr("Target"), tr("Package read in setUp()"), this,
             {GT_CLASSNAME(TestDmiPackage)})
{
    setObjectName(QStringLiteral("Recording Task"));
    registerProperty(m_target);
}

bool
TestRecordingTask::setUp()
{
    test::recordingLog(
        QStringLiteral("setUp recording=%1")
            .arg(GtRecordingSession::current() ? 1 : 0));

    // a read that is recorded because setUp() runs inside the recording
    if (auto* target = data<TestDmiPackage*>(m_target))
    {
        (void)target->mementoTestValue();
    }

    return GtTask::setUp();
}
