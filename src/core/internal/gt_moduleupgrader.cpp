/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 20.05.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFile>
#include <QDomElement>

#include "gt_logging.h"
#include "gt_xmlutilities.h"
#include "gt_coreupgraderoutines.h"
#include "gt_coreapplication.h"
#include "gt_footprint.h"

#include "gt_moduleupgrader.h"

class GtModuleUpgradeHelper
{
public:
    std::vector<gt::VersionUpgradeRoutine> m_upgradeRoutines;

    // returns sorted list of all routines. routines are sorted by
    // version number
    std::vector<gt::VersionUpgradeRoutine> sortedRoutines() const
    {
        std::vector<gt::VersionUpgradeRoutine> routines = m_upgradeRoutines;

        std::sort(routines.begin(),
                  routines.end(), [] (const gt::VersionUpgradeRoutine& x,
                  const gt::VersionUpgradeRoutine& y) { return x.target <
                    y.target; });

        return routines;
    }

    // logic
    void runUpgradeRoutines(const GtVersionNumber& footprintVersion,
                           const QStringList& files) const
    {
        gtDebugId("module data upgrader") << "running upgrade routines...";

        for (auto const& upgradeRoutine : sortedRoutines())
        {
            if (upgradeRoutine.target > footprintVersion)
            {
                gtDebugId("module data upgrader")
                          << upgradeRoutine.target.toString()
                          << " - updating...";

                for (auto const& modData : files)
                {
                    QFile file(modData);

                    if (!file.exists())
                    {
                        gtErrorId("module data upgrader")
                                << "WARNING: file does not exists!";
                        gtErrorId("module data upgrader") << " |-> " << modData;

                        continue;
                    }

                    QDomDocument document;
                    QString errorStr;
                    int errorLine;
                    int errorColumn;

                    if (!document.setContent(&file, true, &errorStr,
                                             &errorLine, &errorColumn))
                    {
                        gtErrorId("module data upgrader")
                                  << "XML ERROR!" << " " << "line" << ": "
                                  << errorLine << " " << "column" << ": "
                                  << errorColumn << " -> " << errorStr;

                        continue;
                    }

                    QDomElement root = document.documentElement();

                    // Call the module upgrade routine
                    upgradeRoutine.f(root, modData);

                    // save file
                    // new ordered attribute stream writer algorithm
                    if (!gt::xml::writeDomDocumentToFile(modData,
                                                         document, true))
                    {
                        gtErrorId("module data upgrader")
                                  << modData << QStringLiteral(": ")
                                  << "Failed to save project data!";
                    }
                }
            }
            else
            {
                // Skipping upgrade routine, since data does not need this
                // upgrade
                gtDebugId("module data upgrader")
                          << upgradeRoutine.target.toString()
                          << " - skipped ( <= " << footprintVersion.toString()
                          << " )";
            }

        }
    }

};

namespace gt
{
namespace detail
{

class GtModuleUpgrader::Impl
{
public:
    std::map<std::string, GtModuleUpgradeHelper> m_upgrader;

};

GtModuleUpgrader&
GtModuleUpgrader::instance()
{
    static GtModuleUpgrader upgrader;
    return upgrader;
}

void
GtModuleUpgrader::registerModuleConverter(const QString &modId,
                                          GtVersionNumber target,
                                          ConverterFunction func)
{
    auto moduleUpgrader = m_pimpl->m_upgrader.find(modId.toStdString());

    if (moduleUpgrader != m_pimpl->m_upgrader.end())
    {
        // found target module
        moduleUpgrader->second.m_upgradeRoutines.push_back({target, func});
    }
    else
    {
        // target module not found. create new entry
        GtModuleUpgradeHelper helper;
        helper.m_upgradeRoutines.push_back({target, func});
        m_pimpl->m_upgrader.insert({modId.toStdString(), helper});
    }
}

void
GtModuleUpgrader::debugModuleConverter()
{
    gtDebug() << "Module Converter: ";

    for (auto const& upgrader : m_pimpl->m_upgrader)
    {
        const auto& moduleId = upgrader.first;
        auto& upgradeHelper = upgrader.second;

        gtDebugId("module data upgrader") << " # "
                  << QString::fromStdString(moduleId);

        for (auto const& upgradeRoutine : upgradeHelper.sortedRoutines())
        {
            gtDebugId("module data upgrader") << " ## ver "
                      << upgradeRoutine.target.toString();
        }
    }
}

void
GtModuleUpgrader::upgrade(const QMap<QString, GtVersionNumber>& moduleFootprint,
                          const QStringList& files) const
{
    for (auto const& upgrader : m_pimpl->m_upgrader)
    {
        const auto& moduleId = upgrader.first;

        if (moduleFootprint.contains(QString::fromStdString(moduleId)))
        {
            gtDebugId("module data upgrader") << " found module match - "
                      << QString::fromStdString(moduleId);

            auto& upgradeHelper = upgrader.second;

            upgradeHelper.runUpgradeRoutines(
                        moduleFootprint.value(QString::fromStdString(moduleId)),
                        files);
        }
    }
}

bool
GtModuleUpgrader::upgradesAvailable(
        const QMap<QString, GtVersionNumber>& moduleFootprint) const
{
    bool retval = false;

    for (auto const& upgrader : m_pimpl->m_upgrader)
    {
        const auto& moduleId = upgrader.first;

        if (moduleFootprint.contains(QString::fromStdString(moduleId)))
        {
            GtVersionNumber savedVer =
                    moduleFootprint.value(QString::fromStdString(moduleId));
            auto& upgradeHelper = upgrader.second;

            for (auto const& upgradeRoutine : upgradeHelper.m_upgradeRoutines)
            {
                if (savedVer < upgradeRoutine.target)
                {
                    gtDebugId("module data upgrader") << QObject::tr(
                        "Upgrade available for '%1 (%2)' to version %3")
                            .arg(moduleId.c_str(),
                                 savedVer.toString(),
                                 upgradeRoutine.target.toString());
                    retval = true;
                }
            }
        }
    }

    return retval;
}

QStringList
GtModuleUpgrader::availableModuleUpgrades(const QMap<QString,
                                        GtVersionNumber>& moduleFootprint) const
{
    QStringList retval;

    for (auto const& upgrader : m_pimpl->m_upgrader)
    {
        const auto& moduleId = upgrader.first;

        if (moduleFootprint.contains(QString::fromStdString(moduleId)))
        {
            GtVersionNumber savedVer =
                    moduleFootprint.value(QString::fromStdString(moduleId));
            auto& upgradeHelper = upgrader.second;

            if (std::any_of(upgradeHelper.m_upgradeRoutines.begin(),
                            upgradeHelper.m_upgradeRoutines.end(),
                            [savedVer]
                            (const VersionUpgradeRoutine& ugradeRoutine)
            {return savedVer < ugradeRoutine.target;}))
            {
                retval << QString::fromStdString(moduleId);
            }
        }
    }

    return retval;
}

QList<GtVersionNumber>
GtModuleUpgrader::availableUpgrades(const QString& moduleIdent,
                                    const GtVersionNumber &savedVer) const
{
    QList<GtVersionNumber> retval;

    for (auto const& upgrader : m_pimpl->m_upgrader)
    {
        const auto& moduleId = upgrader.first;

        if (moduleIdent == QString::fromStdString(moduleId))
        {
            auto& upgradeHelper = upgrader.second;

            for (auto const& upgradeRoutine : upgradeHelper.sortedRoutines())
            {
                if (savedVer < upgradeRoutine.target)
                {
                    retval << upgradeRoutine.target;
                }
            }

            break;
        }
    }

    return retval;
}

GtModuleUpgrader::GtModuleUpgrader() :
    m_pimpl{std::make_unique<GtModuleUpgrader::Impl>()}
{
    // register core upgrade routines
    foreach (const auto& r, gt::detail::coreUpgradeRoutines())
    {
        registerModuleConverter(GtFootprint::frameworkIdentificationString(),
                                r.target, r.f);
    }

}

} // namespace detail
} // namespace gt
