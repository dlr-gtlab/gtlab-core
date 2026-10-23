/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_recordingobserver.h"

GtRecordingObserver::~GtRecordingObserver() = default;

void
GtRecordingObserver::recordingStarted(const GtRecordingSession&)
{
    // nothing to do here
}

void
GtRecordingObserver::propertyRead(const GtObject&, const GtAbstractProperty&)
{
    // nothing to do here
}

void
GtRecordingObserver::objectValueRead(const GtObject&)
{
    // nothing to do here
}

void
GtRecordingObserver::propertyWritten(const GtObject&, const GtAbstractProperty&)
{
    // nothing to do here
}

void
GtRecordingObserver::objectAttached(const GtObject&, const GtObject&)
{
    // nothing to do here
}

void
GtRecordingObserver::objectDetached(const GtObject&, const GtObject&)
{
    // nothing to do here
}

void
GtRecordingObserver::recordingFinished(const GtRecordingSession&, bool)
{
    // nothing to do here
}

void
GtRecordingObserver::objectDestroyed(const GtObject&)
{
    // nothing to do here
}
