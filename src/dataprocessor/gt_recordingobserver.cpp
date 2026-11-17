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
GtRecordingObserver::recordingFinished(const GtRecordingSession&, bool)
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
GtRecordingObserver::propertyListRead(const GtObject& owner, const QString&,
                                      int, const QString&)
{
    objectValueRead(owner);
}

void
GtRecordingObserver::propertyWritten(const GtObject&, const GtAbstractProperty&)
{
    // nothing to do here
}

void
GtRecordingObserver::objectValueWritten(const GtObject&)
{
    // nothing to do here
}

void
GtRecordingObserver::propertyListWritten(const GtObject& owner, const QString&,
                                         int, const QString&)
{
    objectValueWritten(owner);
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
GtRecordingObserver::objectDestroyed(const GtObject*, const GtObject&)
{
    // nothing to do here
}
