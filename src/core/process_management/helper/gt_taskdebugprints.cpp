/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#include "gt_taskdebugprints.h"

#include "gt_processcomponent.h"


void gt::debug::helper::printTaskStructure(GtProcessComponent* pcomp, int lvl, const QString msg)
{
    if(!msg.isEmpty())
    {
        gtInfo() << "------------------------------------------";
        gtInfo() << msg;
    }


    gtInfo() << QString(" ").repeated(lvl*2) << "-" << pcomp << pcomp->uuid();

    for(auto* c: pcomp->findDirectChildren<GtProcessComponent*>())
    {
        printTaskStructure(c, lvl+1, "");
    }

    if(!msg.isEmpty())
    {
        gtInfo() << "------------------------------------------";
    }



}
