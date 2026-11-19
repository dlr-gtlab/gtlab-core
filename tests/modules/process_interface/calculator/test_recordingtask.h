/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_RECORDINGTASK_H
#define TEST_RECORDINGTASK_H

#include "gt_task.h"
#include "gt_objectlinkproperty.h"

/**
 * @brief Test task that links a TestDmiPackage and logs each setUp() call,
 * including whether a recording was active.
 */
class TestRecordingTask : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE TestRecordingTask();

    bool setUp() override;

private:
    GtObjectLinkProperty m_target;
};

#endif // TEST_RECORDINGTASK_H
