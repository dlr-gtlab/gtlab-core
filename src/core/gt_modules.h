/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_modules.h
 */

#ifndef GT_MODULES_H
#define GT_MODULES_H

#include "gt_core_exports.h"
#include "gt_versionnumber.h"

#include <QString>
#include <QStringList>

class GtModuleLoader;
class GtCoreApplication;

namespace gt
{

    /**
 * @brief Result of a module requirements query (@ref gt::Modules)
 *
 * The result contains the module ids that are required for a set of
 * modules, including transitive dependencies, and the dependencies that
 * could not be resolved with the module meta data of the current
 * environment. Unresolved dependencies must not be treated as unused,
 * because their own dependencies are unknown.
 */
    struct GT_CORE_EXPORT ModuleRequirements
    {
        /// Required module ids, including transitive dependencies
        QStringList moduleIds;

        /// Required dependencies that cannot be resolved with the current
        /// module meta data
        QStringList unresolved;

        /// @brief Whether all requirements could be resolved
        bool isComplete() const
        {
            return unresolved.isEmpty();
        }
    };

    /**
 * @brief Provides coherent access to the modules of the GTlab application
 *
 * The class is a lightweight handle to the module subsystem of the
 * application, exposed by @ref GtCoreApplication::modules. It provides the
 * module information that is independent of an individual module, e.g. the
 * versions of loaded modules and the queries which modules are required by
 * a set of modules.
 *
 * All methods return empty or conservative results if the application
 * does not have a module subsystem.
 */
    class GT_CORE_EXPORT Modules
    {
    public:
        /// Constructor. Creates a module access without module subsystem.
        Modules() = default;

        /**
     * @brief Returns the version of a module
     * @param id Module identification string
     * @return Version of the module, null version if the module is not
     * currently loaded
     */
        GtVersionNumber version(const QString& id) const;

        /**
     * @brief Returns the modules that are required by the given modules,
     * including transitive dependencies
     *
     * The requirements are resolved from the effective module meta data of
     * the environment, without changing the module loading state. Required
     * dependencies whose meta data is not available are still contained in
     * the result, but they are reported as unresolved, see
     * @ref gt::ModuleRequirements.
     *
     * @param moduleIds Module identification strings to resolve
     * @return Required module ids including the unresolved dependencies
     */
        ModuleRequirements requirementsFor(const QStringList& moduleIds) const;

    private:
        friend class ::GtCoreApplication;

        /**
     * @brief Constructor. Used by the application to wrap its module
     * subsystem.
     * @param loader Module subsystem, may be null
     */
        explicit Modules(const GtModuleLoader* loader);

        /// Module subsystem of the application, may be null
        const GtModuleLoader* m_loader{nullptr};
    };

} // namespace gt

#endif // GT_MODULES_H
