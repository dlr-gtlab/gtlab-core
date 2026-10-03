/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_modulemetadata.h
 */

#ifndef GT_MODULEMETADATA_H
#define GT_MODULEMETADATA_H

#include "gt_core_exports.h"
#include "gt_versionnumber.h"

#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariantList>

#include <map>
#include <vector>

class QJsonObject;

namespace gt::detail
{

    /**
     * @brief Meta data of a single module as stored in the module plugin file.
     *
     * The meta data is available for all modules that can be found in the module
     * directories, independent of whether the modules are currently loaded.
     */
    class GT_CORE_EXPORT ModuleMetaData
    {
    public:
        explicit ModuleMetaData(const QString& loc = QString());

        /**
         * @brief Loads the meta data from the module json file
         */
        void readFromJson(const QJsonObject& json);

        const QString& location() const noexcept;

        const QString& moduleId() const noexcept;

        struct Dependency
        {
            // avoids faulty cpp-check warning
            bool optional() const
            {
                return isOptional;
            }

            QString name;
            GtVersionNumber version;
            bool isOptional{false};
        };

        /**
         * @return A list of modules (modulename, version) of which
         * this module depends.
         */
        const std::vector<Dependency>& directDependencies() const noexcept;

        /// Returns a list of modules that allow to suppress this module
        const QStringList& suppressorModules() const noexcept;

        const QMap<QString, QString>& environmentVars() const noexcept;

    private:
        static QVariantList metaArray(const QJsonObject& metaData,
                                      const QString& id);

        QString m_id;
        QString m_libraryLocation;

        std::vector<Dependency> m_deps;
        QMap<QString, QString> m_envVars;
        QStringList m_suppression;
    };

    using ModuleMetaMap = std::map<QString, ModuleMetaData>;

    /**
     * @brief Checks if a candidate string matches a dependency pattern.
     * @param dependency Dependency pattern, may use the "regex:" prefix
     * @param candidate Module identification string to match against
     * @return Whether the candidate matches the dependency
     */
    GT_CORE_EXPORT
    bool matchesDependency(const QString& dependency, const QString& candidate);

    /**
     * @brief Returns the ids of all known modules that match a dependency pattern
     * @param dependency Dependency pattern, may use the "regex:" prefix
     * @param allModules Meta data of all known modules
     * @return Matched module identification strings
     */
    GT_CORE_EXPORT
    QStringList getMatchedModuleIds(const QString& dependency,
                                    const ModuleMetaMap& allModules);

    /**
     * @brief Creates a map of all modules that are included in the dependency
     * graph of the given modules, with key=moduleId and value=direct dependencies
     * @param modulesToLoad The list of modules to be included
     * @param allModules    The map of meta data of all modules
     * @return Adjacency matrix of the dependency graph
     */
    GT_CORE_EXPORT
    std::map<QString, QStringList> createAdjacencyMatrix(
        const QStringList& modulesToLoad, const ModuleMetaMap& allModules);

    /**
     * @brief Result of a module dependency closure resolution.
     */
    struct GT_CORE_EXPORT DependencyClosureResult
    {
        /// Given modules plus all modules that they depend on (transitively)
        QStringList moduleIds;

        /**
         * @brief Required dependencies that could not be resolved from the given
         * module meta data.
         *
         * The list contains root module ids with missing metadata and the
         * dependency names of unmatched required
         * dependencies (including unexpanded "regex:" patterns). For those
         * dependencies the dependency graph is unknown, so the closure is
         * incomplete.
         */
        QStringList unresolvedDependencies;

        /// @brief Whether the dependency closure is incomplete
        bool isIncomplete() const noexcept
        {
            return !unresolvedDependencies.isEmpty();
        }
    };

    /**
     * @brief Resolves the transitive dependency closure for a set of module ids.
     *
     * The closure contains the given modules plus all modules that they depend
     * on (transitively), resolved from the given module meta data. Required
     * dependencies whose meta data is not available are still included in the
     * closure, but their own dependencies cannot be resolved and the dependency
     * is reported as unresolved. Optional dependencies keep the semantics of the
     * module loader: they are only part of the closure if the corresponding
     * module meta data is available.
     *
     * @param moduleIds Module identification strings to resolve
     * @param allModules Meta data of all known modules
     * @return Module closure including the information about unresolved
     * dependencies
     */
    GT_CORE_EXPORT
    DependencyClosureResult dependencyClosure(const QStringList& moduleIds,
                                              const ModuleMetaMap& allModules);

} // namespace gt::detail

#endif // GT_MODULEMETADATA_H
