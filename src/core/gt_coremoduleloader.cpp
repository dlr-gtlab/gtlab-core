/* GTlab - Gas Turbine laboratory
 * Source File: gt_coremoduleloader.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QString>

#include "gt_coremoduleloader.h"

#include "gt_moduleinterface.h"

GtCoreModuleLoader::GtCoreModuleLoader()
{

}

GtCoreModuleLoader::~GtCoreModuleLoader()
{

}

bool
GtCoreModuleLoader::check(GtModuleInterface* plugin) const
{
    return GtProcessModuleLoader::check(plugin);
}

void
GtCoreModuleLoader::insert(GtModuleInterface* plugin)
{
    GtProcessModuleLoader::insert(plugin);
}
