/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_projectdependencyanalyzer.h
 */

#ifndef GTPROJECTDEPENDENCYANALYZER_H
#define GTPROJECTDEPENDENCYANALYZER_H

#include "gt_core_exports.h"

#include <QSet>
#include <QString>

#include <memory>

class GtProject;

/**
 * @brief The GtProjectDependencyAnalyzer class determines which modules
 * are used by the data of an open project.
 *
 * The analyzer is based on the current in-memory object tree of the
 * project, so that recently added or deleted objects are reflected
 * immediately. It answers the project usage part of the footprint
 * question:
 *
 * - usedClassNames() returns the classes that currently occur in the
 *   project data. Dummy objects are reported with their original class
 *   names.
 * - directlyRequiredModuleIds() returns the modules that provide the used
 *   classes. The current object/factory module information is preferred;
 *   the persistent class-provider metadata of the project is used as
 *   fallback for classes whose providing module is currently not
 *   available. Classes without a module provider (e.g. core classes) do
 *   not create module dependencies.
 * - unknownUsedClassNames() reports used classes whose providing module
 *   cannot be determined. Such classes must be handled like an unresolved
 *   dependency, because they may require a module that cannot be
 *   determined. Module ownership is never guessed from class names.
 *
 * The transitive resolution of module dependencies is not part of this
 * class. It is provided by the module access of the application,
 * see @ref gt::Modules::requirementsFor.
 *
 * A stale entry in the project-wide class-provider manifest alone does not
 * make a module required; only currently used classes contribute direct
 * module dependencies.
 *
 * @note The analyzer does not modify the project, its module selection or
 * its stored metadata.
 */
class GtProjectDependencyAnalyzer
{
public:
    /**
     * @brief Constructor. Runs the dependency analysis for a given project.
     * @param project Project to analyze. May be null.
     */
    GT_CORE_EXPORT explicit GtProjectDependencyAnalyzer(
        const GtProject* project);

    /**
     * @brief Destructor.
     */
    GT_CORE_EXPORT ~GtProjectDependencyAnalyzer();

    /**
     * @brief Returns the class names that are currently used by the project
     * data.
     * @return Used class names
     */
    GT_CORE_EXPORT const QSet<QString>& usedClassNames() const;

    /**
     * @brief Returns the ids of the modules that directly provide classes
     * used by the project.
     * @return Directly required module ids
     */
    GT_CORE_EXPORT const QSet<QString>& directlyRequiredModuleIds() const;

    /**
     * @brief Returns the used classes that cannot be attributed to a module.
     *
     * These are classes that are neither known by the object factories nor
     * listed in the class-provider metadata of the project, e.g. dummy
     * objects of a legacy project whose providing module is unknown. Such
     * classes may require a module that cannot be determined, therefore the
     * set must be handled like an unresolved dependency.
     *
     * @return Names of used classes without a known provider
     */
    GT_CORE_EXPORT const QSet<QString>& unknownUsedClassNames() const;

private:
    /// Private implementation
    class Impl;

    std::unique_ptr<Impl> m_pimpl;
};

#endif // GTPROJECTDEPENDENCYANALYZER_H
