#include "gt_accesstracking.h"
#include "gt_logging.h"
#include "gt_object.h"
#include <qdatetime.h>


QSet<QUuid> GtAccessTracker::getAccessedObjects(const QUuid& contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return{};
    }
    return m_contexts[contextUuid].accessedObjects;
}

QSet<QUuid> GtAccessTracker::getChildContextUuid(const QUuid &contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return{};
    }
    return m_contexts[contextUuid].childContextUuid;
}

void GtAccessTracker::addAccessedProperty(const QUuid& uuid)
{
    if (!m_activeStack.empty())
    {
        const QUuid& contextUuid = m_activeStack.top();

        //gtInfo() << "Object added:" << uuid<<" of Context:"<<contextUuid;
        m_contexts[contextUuid].accessedObjects.insert(uuid);
    }
}

bool GtAccessTracker::clearContext(const QUuid &contextUuid)
{
    if (!m_contexts.contains(contextUuid))
    {
        gtError()<< "Context map does not contain a context with UUID: "<<contextUuid;
        return false;
    }
    m_contexts.remove(contextUuid);
    return true;
}

void GtAccessTracker::startAccessTracking(const QUuid& contextUuid)
{

    AccessContext context;

    QUuid parentContextUuid{};
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

    m_activeStack.pop();
}
