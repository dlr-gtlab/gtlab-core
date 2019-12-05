/* GTlab - Gas Turbine laboratory
 * Source File: gt_coremoduleloader.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 22.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTCOREMODULELOADER_H
#define GTCOREMODULELOADER_H

#include "gt_core_exports.h"

#include <QHash>

#include "gt_processmoduleloader.h"

/**
 * @brief The GtCoreModuleLoader class
 */
class GT_CORE_EXPORT GtCoreModuleLoader : public GtProcessModuleLoader
{
public:
    /**
     * @brief GtCoreModuleLoader
     */
    GtCoreModuleLoader();

    /**
     * @brief ~GtCoreModuleLoader
     */
    virtual ~GtCoreModuleLoader();

protected:
    /**
     * @brief check
     * @param plugin
     * @return
     */
    virtual bool check(GtModuleInterface* plugin);

    /**
     * @brief insert
     * @param plugin
     */
    virtual void insert(GtModuleInterface* plugin);

};

#endif // GTCOREMODULELOADER_H
