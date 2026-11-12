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


namespace gt
{
namespace utils
{
namespace process
{


GtPropertyConnection*
makePropertyConnection(GtProcessComponent& sourceComponent,
                       const QString& sourceProperty,
                       GtProcessComponent& targetComponent,
                       const QString& targetProperty)
{

    if (!sourceComponent.findProperty(sourceProperty) ||
        !targetComponent.findProperty(targetProperty))
    {
        QStringList msg;
        if (!sourceComponent.findProperty(sourceProperty))
        {
            msg << "Source property not found!";
        }
        if (!targetComponent.findProperty(targetProperty))
        {
            msg << "Target property not found!";
        }

        gtError() << msg.join("\n") << QStringLiteral("\n") + "sourceComponent:"
                  << &sourceComponent
                  << QStringLiteral("\n") + "sourceProperty:" << sourceProperty
                  << QStringLiteral("\n") + "targetComponent:"
                  << &targetComponent
                  << QStringLiteral("\n") + "targetProperty:" << targetProperty;
        return nullptr;
    }

    auto rootTaskSrc = findRootTask(&sourceComponent);
    auto rootTaskTgt = findRootTask(&targetComponent);

    if (!rootTaskSrc || !rootTaskTgt)
    {
        gtError() << "Highest parent not found";
        return nullptr;
    }

    if (rootTaskSrc != rootTaskTgt)
    {
        gtError()
            << "Selected process components not within the same root task!";
        return nullptr;
    }

    auto targetPropertyObj = targetComponent.findProperty(targetProperty);

    if (targetPropertyObj->isConnected())
    {
        gtError() << "Target property already connected:" << targetProperty << ", target process component:" << &targetComponent;
        return nullptr;
    }

    GtPropertyConnection* c = new GtPropertyConnection;
    c->setSourceUuid(sourceComponent.uuid());
    c->setSourceProp(sourceComponent.findProperty(sourceProperty)->ident());
    c->setTargetUuid(targetComponent.uuid());
    c->setTargetProp(targetPropertyObj->ident());
    rootTaskSrc->appendChild(c);
    c->makeConnection();

    if (!c->isConnected())
    {
        gtError() << "Property connection created, but connection failed";
        delete c;
        return nullptr;
    }

    return c;
}


} // namespace process
} // namespace utils
} // namespace gt