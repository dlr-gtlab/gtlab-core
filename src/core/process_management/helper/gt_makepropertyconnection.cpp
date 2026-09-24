/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */
#include "gt_makepropertyconnection.h"

#include "gt_propertyconnection.h"
#include "gt_processcomponent.h"
#include "gt_task.h"
#include "gt_taskanalysis.h"



namespace gt {
namespace utils {
namespace process {


GtPropertyConnection*
makePropertyConnection(GtProcessComponent* sourceComponent, const QString &sourceProperty, GtProcessComponent* targetComponent, const QString &targetProperty, bool *ok)
{
    if(ok)
    {
        *ok = false;
    }

    if(!sourceComponent || !targetComponent)
    {
        gtFatal() << "Source or target not set!"
                  << "\nSource:" << sourceComponent
                  << "\nTarget:" << targetComponent;
        return nullptr;
    }


    if(!sourceComponent->findProperty(sourceProperty) || !targetComponent->findProperty(targetProperty))
    {
        QStringList msg;
        if(!sourceComponent->findProperty(sourceProperty))
        {
            msg << "Source property not found!";
        }
        if(!targetComponent->findProperty(targetProperty))
        {
            msg << "Target property not found!";
        }

        gtFatal() << msg.join("\n")
                  << QStringLiteral("\n")+"sourceComponent:" << sourceComponent
                  << QStringLiteral("\n")+"sourceProperty:"  << sourceProperty
                  << QStringLiteral("\n")+"targetComponent:" << targetComponent
                  << QStringLiteral("\n")+"targetProperty:"  << targetProperty;
        return nullptr;
    }

    auto rootTask = findRootTask(sourceComponent);

    if(!rootTask)
    {
        gtFatal() << "Highest parent not found";
        return nullptr;
    }

    GtPropertyConnection* c = new GtPropertyConnection;
    c->setSourceUuid(sourceComponent->uuid());
    c->setSourceProp(sourceComponent->findProperty(sourceProperty)->ident());
    c->setTargetUuid(targetComponent->uuid());
    c->setTargetProp(targetComponent->findProperty(targetProperty)->ident());
    rootTask->appendChild(c);
    c->makeConnection();

    if (!c->isConnected())
    {
        gtFatal() << "Property connection created, but connection failed";
        delete c;
        return nullptr;
    }

    if(ok)
    {
        *ok = true;
    }

    return c;
}




} // namespace process
} // namespace utils
} // namespace gt