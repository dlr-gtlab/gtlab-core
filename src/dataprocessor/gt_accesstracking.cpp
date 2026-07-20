#include "gt_accesstracking.h"
#include "gt_logging.h"
#include "gt_object.h"
#include <qdatetime.h>


QSet<QString> GtAccessTracker::getAccessedObjects(const QString& contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return{};
    }
    return m_contexts[contextUuid].accessedObjects;
}

QSet<QString> GtAccessTracker::getChildContextUuid(const QString &contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return{};
    }
    return m_contexts[contextUuid].childContextUuid;
}

void GtAccessTracker::addAccessedProperty(const QString& uuid)
{
    if (!m_activeStack.empty())
    {
        const QString& contextUuid = m_activeStack.top();

        gtInfo() << "Object added:" << uuid;
        m_contexts[contextUuid].accessedObjects.insert(uuid);
    }
}

bool GtAccessTracker::clearContext(const QString &contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return false;
    }
    m_contexts.remove(contextUuid);
    return true;
}

void GtAccessTracker::startAccessTracking(const QString& contextUuid)
{

    AccessContext context;
    context.uuid=contextUuid;

    QString parentContextUuid{};
    if (!m_activeStack.empty())
    {
        parentContextUuid=m_activeStack.top();
        m_contexts[parentContextUuid].childContextUuid.insert(contextUuid);
    }

    m_activeStack.push(contextUuid);
    m_contexts.insert(contextUuid,context);

}

void GtAccessTracker::endAccessTracking()
{
    if (m_activeStack.empty()) {
        gtError()<<"Tried to endAccessTracking with no active context!";
        return;
    }

    QString contextUuid = m_activeStack.pop();
}
