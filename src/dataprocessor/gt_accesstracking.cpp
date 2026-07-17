#include "gt_accesstracking.h"
#include "gt_logging.h"
#include "gt_object.h"


QStringList AccessList::getList()
{
    return m_accessList;
}

void AccessList::clearList()
{
    m_accessList.clear();
}

void AccessList::tracking(bool state)
{
    gtInfo() << "activateTracking set to "<< state;
    m_tracking = state;
}


void AccessList::addAccessedProperty(QString uuid)
{
    if (!m_activeStack.empty())
    {
        const QString& contextUuid = m_activeStack.top();

        gtInfo() << "Object added:" << uuid;
        m_contexts[contextUuid].accessedObjects.insert(uuid);
    }
}

void AccessList::startAccessTracking(GtObject& object)
{
    m_activeStack.push(object.uuid());
    AccessContext context;
    context.uuid=object.uuid();
    context.parentUuid=object.parentObject()->uuid();
    context.startTime={};
    context.endTime={};
    context.isActive=true;

}

void AccessList::endAccessTracking()
{
    QString uuid = m_activeStack.pop();

}
