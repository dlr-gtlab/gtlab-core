/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_projectdependencyanalyzer.cpp
 */

#include "gt_projectdependencyanalyzer.h"

#include "gt_project.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_processfactory.h"

#include <QMap>
#include <QStringList>

#include <algorithm>

class GtProjectDependencyAnalyzer::Impl
{
public:
    /// Class names currently used by the project data
    QSet<QString> m_usedClassNames;

    /// Modules that directly provide used classes
    QSet<QString> m_directlyRequiredModuleIds;

    /// Used classes without a known providing module
    QSet<QString> m_unknownUsedClassNames;

    /// Class names that are known by the object and process factories
    QSet<QString> m_knownClassNames;

    /// Module ids of used classes resolved from current object/factory state
    QMap<QString, QString> m_currentClassModules;

    /// Project that has been analyzed
    const GtProject* m_project{nullptr};

    void analyze(const GtProject* project);

    /// Collects the classes of the current in-memory object tree
    void readUsedClassNames(const GtProject* project);

    /// Takes the module information provided by the current environment
    void readClassModuleInfo(const QString& className,
                             const GtAbstractObjectFactory* factory);

    /// Returns the module that provides the given class
    QString resolveClassModule(const QString& className) const;
};

GtProjectDependencyAnalyzer::GtProjectDependencyAnalyzer(
    const GtProject* project) : m_pimpl{std::make_unique<Impl>()}
{
    m_pimpl->analyze(project);
}

GtProjectDependencyAnalyzer::~GtProjectDependencyAnalyzer() = default;

const QSet<QString>&
GtProjectDependencyAnalyzer::usedClassNames() const
{
    return m_pimpl->m_usedClassNames;
}

const QSet<QString>&
GtProjectDependencyAnalyzer::directlyRequiredModuleIds() const
{
    return m_pimpl->m_directlyRequiredModuleIds;
}

const QSet<QString>&
GtProjectDependencyAnalyzer::unknownUsedClassNames() const
{
    return m_pimpl->m_unknownUsedClassNames;
}

void
GtProjectDependencyAnalyzer::Impl::analyze(const GtProject* project)
{
    if (!project)
    {
        return;
    }

    m_project = project;

    // the current in-memory object state is the source of truth
    readUsedClassNames(project);

    for (const QString& className : qAsConst(m_usedClassNames))
    {
        const QString moduleId = resolveClassModule(className);
        if (!moduleId.isEmpty())
        {
            m_directlyRequiredModuleIds.insert(moduleId);
            continue;
        }

        // Classes without a known module provider (e.g. core classes) do not
        // create module dependencies. Classes that are not known at all
        // cannot be classified; they are reported like unresolved
        // dependencies so that existing information is kept. Module
        // ownership is never guessed from class names.
        if (!m_knownClassNames.contains(className))
        {
            m_unknownUsedClassNames.insert(className);
        }
    }
}

void
GtProjectDependencyAnalyzer::Impl::readUsedClassNames(const GtProject* project)
{
    for (const GtObject* object : project->findChildren<GtObject*>())
    {
        Q_ASSERT(object);

        const QString className = object->isDummy()
                                      ? object->toMemento().className()
                                      : object->metaObject()->className();
        if (className.isEmpty()) continue;

        m_usedClassNames.insert(className);

        // prefer the object's/factory's current module information
        readClassModuleInfo(className, object->factory());
    }
}

void
GtProjectDependencyAnalyzer::Impl::readClassModuleInfo(
    const QString& className, const GtAbstractObjectFactory* factory)
{
    const bool known = (factory && factory->knownClass(className)) ||
                       gtObjectFactory->knownClass(className) ||
                       gtProcessFactory->knownClass(className);

    if (known)
    {
        m_knownClassNames.insert(className);
    }

    if (!m_currentClassModules.value(className).isEmpty())
    {
        return;
    }

    QStringList moduleIds;
    if (factory)
    {
        moduleIds << factory->moduleId(className);
    }
    moduleIds << gtObjectFactory->moduleId(className)
              << gtProcessFactory->moduleId(className);

    const auto firstModule = std::find_if(
        moduleIds.cbegin(), moduleIds.cend(),
        [](const QString& moduleId) { return !moduleId.isEmpty(); });

    if (firstModule != moduleIds.cend())
    {
        m_currentClassModules.insert(className, *firstModule);
    }
}

QString
GtProjectDependencyAnalyzer::Impl::resolveClassModule(
    const QString& className) const
{
    // prefer the current module information of objects/factories
    QString moduleId = m_currentClassModules.value(className);

    // fall back to the persistent project class-provider metadata. This
    // also allows resolving classes of currently unavailable modules.
    if (moduleId.isEmpty() && m_project)
    {
        moduleId = m_project->classModuleId(className);
    }

    return moduleId;
}
