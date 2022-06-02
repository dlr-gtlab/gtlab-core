/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 20.05.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODULEUPDATER_H
#define GTMODULEUPDATER_H

#include <memory>

#include "gt_moduleinterface.h"

class GtModuleUpdaterImpl;

class GtModuleUpdater
{
public:
    static GtModuleUpdater& instance();

    void registerModuleConverter(const QString& modId, GtVersionNumber target,
                                 ConverterFunction func);

    void debugModuleConverter();

    void update(const QMap<QString,
                GtVersionNumber>& moduleFootprint,
                const QStringList& moduleData) const;

private:
    /// Private implementation
    std::unique_ptr<GtModuleUpdaterImpl> m_pimpl;

    /**
     * @brief Constructor.
     */
    GtModuleUpdater();
};

#endif // GTMODULEUPDATER_H
