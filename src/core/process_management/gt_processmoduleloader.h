/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    bool check(GtModuleInterface* plugin) const override;

    /**
     * @brief insert
     * @param plugin
     */
    void insert(GtModuleInterface* plugin) override;

};

#endif // GTPROCESSMODULELOADER_H
