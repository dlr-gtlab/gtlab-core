/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_modules.cpp
 */

#include "gt_modules.h"

#include "gt_moduleloader.h"

namespace gt
{

    Modules::Modules(const GtModuleLoader* loader) : m_loader{loader}
    {
    }

    GtVersionNumber Modules::version(const QString& id) const
    {
        if (!m_loader)
        {
            return GtVersionNumber();
        }

        return m_loader->moduleVersion(id);
    }

    ModuleRequirements Modules::requirementsFor(
        const QStringList& moduleIds) const
    {
        if (!m_loader)
        {
            // without module meta data no dependency can be resolved. Every
            // requested module is required but unresolved.
            ModuleRequirements requirements;
            requirements.moduleIds = moduleIds;
            requirements.moduleIds.removeDuplicates();
            requirements.moduleIds.sort();
            requirements.unresolved = requirements.moduleIds;
            return requirements;
        }

        return m_loader->requirementsFor(moduleIds);
    }

} // namespace gt
