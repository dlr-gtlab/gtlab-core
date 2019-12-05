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

#include "gt_calculators_exports.h"

#include <QHash>

#include "gt_moduleloader.h"

/**
 * @brief The GtProcessModuleLoader class
 */
class GT_CALCULATORS_EXPORT GtProcessModuleLoader : public GtModuleLoader
{
public:
    /**
     * @brief GtProcessModuleLoader
     */
    GtProcessModuleLoader();

    /**
     * @brief ~GtCoreModuleLoader
     */
    virtual ~GtProcessModuleLoader();

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

#endif // GTPROCESSMODULELOADER_H
