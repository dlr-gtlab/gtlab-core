/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_coremoduleloader.cpp
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
