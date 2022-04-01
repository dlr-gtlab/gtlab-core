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
    ~GtCoreModuleLoader() override;

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

#endif // GTCOREMODULELOADER_H
