#include "gt_recording.h"
#include <quuid.h>

GtRecording::GtRecording()
{
m_actvityUuid = QUuid::createUuid().toString();
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

QString GtRecording::contextUuid() const
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

QSet<QString> GtRecording::childContextUuids() const
{
    return m_childContextUuids;
}
