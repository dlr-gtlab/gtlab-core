/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSMODULELOADER_H
#define GTPROCESSMODULELOADER_H

#include "gt_core_exports.h"

#include <QHash>

#include "gt_moduleloader.h"

/**
 * @brief The GtProcessModuleLoader class
 */
class GT_CORE_EXPORT GtProcessModuleLoader : public GtModuleLoader
{
public:
    /**
     * @brief GtProcessModuleLoader
     */
    GtProcessModuleLoader();

protected:
    /**
     * @brief check
     * @param plugin
     * @return
     */
    bool check(GtModuleInterface* plugin) override;

    /**
     * @brief insert
     * @param plugin
     */
    void insert(GtModuleInterface* plugin) override;

};

#endif // GTPROCESSMODULELOADER_H
