/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_modulemetadata.cpp
 */

#include "gt_modulemetadata.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpression>

#include <algorithm>
#include <iterator>

namespace gt::detail
{

    ModuleMetaData::ModuleMetaData(const QString& loc) : m_libraryLocation(loc)
    {
    }

    void ModuleMetaData::readFromJson(const QJsonObject& pluginMetaData)
    {
        auto json = pluginMetaData.value(QStringLiteral("MetaData")).toObject();

        // read plugin/module id
        m_id = pluginMetaData.value("IID").toString();

        // read dependencies
        QVariantList deps = metaArray(json, QStringLiteral("dependencies"));

        m_deps.clear();
        for (const auto& d : qAsConst(deps))
        {
            QVariantMap mitem = d.toMap();

            auto name = mitem.value(QStringLiteral("name")).toString();
            GtVersionNumber version(
                mitem.value(QStringLiteral("version")).toString());

            auto isOptionalVar = mitem.value(QStringLiteral("optional"));
            bool isOptional =
                isOptionalVar.isValid() ? isOptionalVar.toBool() : false;

            m_deps.push_back({name, version, isOptional});
        }

        // get sys_env_vars list
        QVariantList sys_vars = metaArray(json, QStringLiteral("sys_env_vars"));

        m_envVars.clear();
        for (const QVariant& var : qAsConst(sys_vars))
        {
            QVariantMap mitem = var.toMap();

            const QString name = mitem.value(QStringLiteral("name")).toString();
            const QString initVar =
                mitem.value(QStringLiteral("init")).toString();

            if (!m_envVars.contains(name))
            {
                m_envVars.insert(name, initVar);
            }
        }

        // get suppressor list
        QVariantList supprs =
            metaArray(json, QStringLiteral("allowSuppressionBy"));

        m_suppression.clear();
        std::transform(supprs.cbegin(), supprs.cend(),
                       std::back_inserter(m_suppression),
                       [](const QVariant& var) { return var.toString(); });
    }

    const QString& ModuleMetaData::location() const noexcept
    {
        return m_libraryLocation;
    }

    const QString& ModuleMetaData::moduleId() const noexcept
    {
        return m_id;
    }

    const std::vector<ModuleMetaData::Dependency>& ModuleMetaData::
        directDependencies() const noexcept
    {
        return m_deps;
    }

    const QStringList& ModuleMetaData::suppressorModules() const noexcept
    {
        return m_suppression;
    }

    const QMap<QString, QString>& ModuleMetaData::environmentVars()
        const noexcept
    {
        return m_envVars;
    }

    QVariantList ModuleMetaData::metaArray(const QJsonObject& metaData,
                                           const QString& id)
    {
        return metaData.value(id).toArray().toVariantList();
    }

    bool matchesDependency(const QString& dependency, const QString& candidate)
    {
        const QString regexPrefix = "regex:";

        // If the dependency starts with the explicit "regex" prefix,
        // use regex matching.
        if (dependency.startsWith(regexPrefix))
        {
            // Remove the prefix to get the actual regex pattern.
            QString pattern = dependency.mid(regexPrefix.length());
            QRegularExpression rx(pattern);

            // Check if the candidate matches the regex pattern.
            return rx.match(candidate).hasMatch();
        }
        else
        {
            // Otherwise, perform an exact, literal match.
            return dependency == candidate;
        }
    }

    QStringList getMatchedModuleIds(const QString& dependency,
                                    const ModuleMetaMap& allModules)
    {
        QStringList result;

        for (auto&& module : allModules)
        {
            auto moduleId = module.second.moduleId();
            if (matchesDependency(dependency, moduleId))
                result.push_back(moduleId);
        }

        return result;
    }

    void createAdjacencyMatrixImpl(const QStringList& modulesToLoad,
                                   const ModuleMetaMap& allModules,
                                   std::map<QString, QStringList>& matrix)
    {
        for (const auto& moduleId : modulesToLoad)
        {
            // If the module is already in the matrix,
            // it will not overwrite the current module
            auto insertResult =
                matrix.insert(std::make_pair(moduleId, QStringList{}));

            if (!insertResult.second)
            {
                // continue, module is already in matrix, stop recursion
                continue;
            }

            auto moduleIt = allModules.find(moduleId);
            if (moduleIt == allModules.end())
            {
                // dependency not found, skip it
                continue;
            }

            // Add dependencies to matrix
            auto& moduleDeps = insertResult.first->second;
            for (const auto& dep : moduleIt->second.directDependencies())
            {
                auto matchedModulIds =
                    getMatchedModuleIds(dep.name, allModules);
                moduleDeps.append(matchedModulIds);
            }

            // recurse into dependencies
            createAdjacencyMatrixImpl(moduleDeps, allModules, matrix);
        }
    }

    std::map<QString, QStringList> createAdjacencyMatrix(
        const QStringList& modulesToLoad, const ModuleMetaMap& allModules)
    {
        std::map<QString, QStringList> adjMatrix;
        createAdjacencyMatrixImpl(modulesToLoad, allModules, adjMatrix);
        return adjMatrix;
    }

    DependencyClosureResult dependencyClosure(const QStringList& moduleIds,
                                              const ModuleMetaMap& allModules)
    {
        const auto moduleMatrix = createAdjacencyMatrix(moduleIds, allModules);

        DependencyClosureResult result;

        std::transform(moduleMatrix.cbegin(), moduleMatrix.cend(),
                       std::back_inserter(result.moduleIds),
                       [](const std::pair<QString, QStringList>& entry) {
                           return entry.first;
                       });

        // Conservative mode: a required dependency that is named by available
        // module meta data but whose own meta data is not available cannot be
        // traversed. Literal dependency names are kept in the closure so that
        // their dependencies are not silently dropped. Regex dependencies cannot
        // be materialized to a concrete module id and are therefore not added to
        // the closure. Both cases are reported as unresolved dependencies,
        // because their own dependency graph is unknown.
        for (const auto& entry : moduleMatrix)
        {
            auto moduleIt = allModules.find(entry.first);
            if (moduleIt == allModules.end())
            {
                result.unresolvedDependencies.append(entry.first);
                continue;
            }

            for (const auto& dep : moduleIt->second.directDependencies())
            {
                // Optional dependencies keep the module loader semantics:
                // they only count if the corresponding module is available.
                if (dep.optional()) continue;

                if (!getMatchedModuleIds(dep.name, allModules).isEmpty())
                {
                    continue;
                }

                if (!dep.name.startsWith(QStringLiteral("regex:")))
                {
                    result.moduleIds.append(dep.name);
                }

                result.unresolvedDependencies.append(dep.name);
            }
        }

        result.moduleIds.removeDuplicates();
        result.moduleIds.sort();

        result.unresolvedDependencies.removeDuplicates();
        result.unresolvedDependencies.sort();

        return result;
    }

} // namespace gt::detail
