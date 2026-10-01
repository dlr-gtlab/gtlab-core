/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_legacyprojectaccess.h"

#include "gt_abstractobjectfactory.h"
#include "gt_coreapplication.h"
#include "gt_logging.h"
#include "gt_processcomponent.h"

#include <QByteArray>
#include <QMetaObject>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QString>

namespace {

/// Component executed on the current thread, borrowed and never owned
thread_local GtProcessComponent* executingComponent = nullptr;

/// Environment variable overriding the developer mode condition
const char* const kWarningEnvVar = "GTLAB_LEGACY_PROJECT_ACCESS_WARNING";

/// Tri-state result of the environment override
enum class EnvironmentOverride {
    /// No valid override, fall back to the developer mode condition
    NOT_SET,
    /// Override to off, e.g. "0"
    OFF,
    /// Override to on, e.g. "1"
    ON,
};

/// Registry mutex
QMutex& registryMutex()
{
    static QMutex mutex;
    return mutex;
}

/// Meta-object class names, optionally prefixed by the providing module, that
/// have already been reported during this application run
QSet<QString>& warnedComponents()
{
    static QSet<QString> warned;
    return warned;
}

/// Warning policy mutex
QMutex& policyMutex()
{
    static QMutex mutex;
    return mutex;
}

/// Currently installed warning policy
GtLegacyProjectAccess::DeveloperModePolicy& developerModePolicy()
{
    static GtLegacyProjectAccess::DeveloperModePolicy policy;
    return policy;
}

/// Returns the meta-object class name of a component
QString className(GtProcessComponent const& component)
{
    return QString::fromLatin1(component.metaObject()->className());
}

/// Returns the identity of the module providing a component, if known
QString moduleId(GtProcessComponent const& component)
{
    GtAbstractObjectFactory* factory = component.factory();

    if (!factory)
    {
        return QString();
    }

    return factory->moduleId(className(component));
}

/// Returns the override of the developer mode condition
///
/// The value is read on every call because it is only consulted for component
/// classes that have not been reported yet. Values that are not recognised are
/// treated as if the variable was not set.
EnvironmentOverride environmentOverride()
{
    const QString value = QString::fromLocal8Bit(qgetenv(kWarningEnvVar))
                                  .trimmed().toLower();

    if (value.isEmpty())
    {
        return EnvironmentOverride::NOT_SET;
    }

    if (value == QLatin1String("1") || value == QLatin1String("true") ||
        value == QLatin1String("on") || value == QLatin1String("yes"))
    {
        return EnvironmentOverride::ON;
    }

    if (value == QLatin1String("0") || value == QLatin1String("false") ||
        value == QLatin1String("off") || value == QLatin1String("no"))
    {
        return EnvironmentOverride::OFF;
    }

    return EnvironmentOverride::NOT_SET;
}

} // namespace

GtProcessComponentExecutionScope::GtProcessComponentExecutionScope(
    GtProcessComponent* component) :
    m_previous(executingComponent)
{
    executingComponent = component;
}

GtProcessComponentExecutionScope::~GtProcessComponentExecutionScope()
{
    executingComponent = m_previous;
}

GtProcessComponent*
GtProcessComponentExecutionScope::current() noexcept
{
    return executingComponent;
}

void
GtLegacyProjectAccess::reportLegacyAccess()
{
    GtProcessComponent* component = GtProcessComponentExecutionScope::current();

    // Access outside of a component execution, e.g. from GUI code that
    // intentionally targets the project selected in the desktop application,
    // is supported and therefore not reported.
    if (!component)
    {
        return;
    }

    if (!warningsEnabled())
    {
        return;
    }

    QString const key = warningKey(*component);

    bool report = false;
    {
        QMutexLocker locker(&registryMutex());

        report = !warnedComponents().contains(key);

        if (report)
        {
            warnedComponents().insert(key);
        }
    }

    if (!report)
    {
        return;
    }

    gtWarning() << warningMessage(*component);
}

void
GtLegacyProjectAccess::setDeveloperModePolicy(DeveloperModePolicy policy)
{
    QMutexLocker locker(&policyMutex());
    developerModePolicy() = std::move(policy);
}

void
GtLegacyProjectAccess::clearRegistry()
{
    QMutexLocker locker(&registryMutex());
    warnedComponents().clear();
}

bool
GtLegacyProjectAccess::warningsEnabled()
{
    DeveloperModePolicy policy;
    {
        QMutexLocker locker(&policyMutex());
        policy = developerModePolicy();
    }

    if (policy)
    {
        return policy();
    }

    // An explicit environment override replaces the developer mode condition.
    // It makes the diagnostic usable for headless batch runs and CI jobs,
    // where the GUI developer mode is never enabled, and allows silencing it
    // without code changes.
    switch (environmentOverride())
    {
        case EnvironmentOverride::ON:
            return true;
        case EnvironmentOverride::OFF:
            return false;
        case EnvironmentOverride::NOT_SET:
            break;
    }

    // The application instance is optional, e.g. in batch tools or unit tests,
    // and its developer mode flag is plain state without side effects.
    GtCoreApplication* app = GtCoreApplication::instance();

    return app && app->devMode();
}

QString
GtLegacyProjectAccess::warningKey(GtProcessComponent const& component)
{
    QString const id = className(component);
    QString const module = moduleId(component);

    return module.isEmpty() ? id : module + QStringLiteral("/") + id;
}

QString
GtLegacyProjectAccess::warningMessage(GtProcessComponent const& component)
{
    QString const name = component.objectName().isEmpty()
            ? className(component)
            : component.objectName();

    QString location = QStringLiteral("'%1'\n(class: %2")
            .arg(name, className(component));

    QString const module = moduleId(component);

    if (!module.isEmpty())
    {
        location += QStringLiteral(", module: %1").arg(module);
    }

    location += QLatin1Char(')');

    return QStringLiteral(
        "Legacy project access detected in process component %1.\n"
        "\n"
        "gtApp->currentProject() resolved the execution project through the "
        "compatibility fallback of the active GtExecutionContext. The call "
        "succeeded, but new calculator code should not rely on it:\n"
        "  - use already available or linked execution objects,\n"
        "  - pass the required project data explicitly into helpers and "
        "services, or\n"
        "  - use GtExecutionContext::current() for genuinely project-scoped "
        "execution information.\n"
        "\n"
        "currentProject() is not deprecated globally. Code that intentionally "
        "targets the project selected in the GUI can keep using it.\n"
        "\n"
        "Migration guide: GTlab developer documentation, "
        "\"Project context and currentProject() compatibility\".")
            .arg(location);
}
