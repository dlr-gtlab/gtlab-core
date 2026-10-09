/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_runnable.cpp
 *
 *  Created on: 19.08.2026
 *  Author: Robert Marcenko (AT-TWK)
 */


#include "gt_recording.h"
#include "gt_abstractrunnable.h"
#include "gt_accesstracking.h"

#include <quuid.h>
#include <QDateTime>

GtRecording::GtRecording():m_actvityUuid{QUuid::createUuid().toString()}
{
}

void GtRecording::setActivityObject(QPointer<GtObject> activityObject)
{
    m_activityObject = activityObject;
}

void GtRecording::setLinkedObjects(QList<QPointer<GtObject>> linkedObjects)
{
    m_linkedObjects = linkedObjects;
}

void GtRecording::setStartAtTime(const QString& startAtTime)
{
    m_startAtTime = startAtTime;
}

void GtRecording::setEndedAtTime(const QString& endedAtTime)
{
    m_endedAtTime = endedAtTime;
}


QList<QPointer<GtObject> > GtRecording::linkedObjects() const
{
    return m_linkedObjects;
}

QString GtRecording::toolName() const
{
    return m_toolName;
}

void GtRecording::setToolName(const QString &newToolName)
{
    m_toolName = newToolName;
}

QString GtRecording::Version() const
{
    return m_Version;
}

void GtRecording::setVersion(const QString &newVersion)
{
    m_Version = newVersion;
}

QString GtRecording::State() const
{
    return m_State;
}

void GtRecording::setState(const QString &newState)
{
    m_State = newState;
}

QPointer<GtObject> GtRecording::activityObject() const
{
    return m_activityObject;
}

QUuid GtRecording::contextUuid() const
{
    return m_actvityUuid;
}

QString GtRecording::startAtTime() const
{
    return m_startAtTime;
}

QString GtRecording::endedAtTime() const
{
    return m_endedAtTime;
}

QSet<QUuid> GtRecording::childContextUuids() const
{
    return m_childContextUuids;
}

namespace gt
{
GtRecording
startAccessRecording(GtAbstractRecorder* recorder,
                     QPointer<GtObject> activityObject,
                     QList<QPointer<GtObject> > linkedObjects,
                     const GtAbstractRunnable& runnable)
{
    Q_UNUSED(runnable);

    GtRecording recording;
    recording.setActivityObject(activityObject);
    recording.setLinkedObjects(linkedObjects);

    if (recorder)
    {
        recorder->initLinkedObjects(linkedObjects);
    }

    recording.setStartAtTime(QDateTime::currentDateTimeUtc()
                             .toString("yyyy-MM-ddThh:mm:ssZ"));
    return recording;
}

void
endAccessRecording(GtAbstractRecorder* recorder,
                   GtRecording& recording,
                   const GtAbstractRunnable& runnable)
{
    recording.setEndedAtTime(QDateTime::currentDateTimeUtc()
                             .toString("yyyy-MM-ddThh:mm:ssZ"));

    if (!recorder)
    {
        return;
    }

    recorder->createActivity(recording);

    // record accessed objects of the execution context
    const QSet<QUuid> accessedObjects = runnable.accessTracker()->accessedObjects();
    recorder->recordAccessObjects(accessedObjects, recording.linkedObjects());

    // execute "diff"
    recorder->recordChanges(recording.linkedObjects());
}
}
