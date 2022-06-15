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

#include "gt_moduleupdater.h"

class GtModuleUpdateHelper
{
public:
    std::vector<VersionUpdateRoutine> m_updateRoutines;

    //logic
    void runUpdateRoutines(const GtVersionNumber& footprintVersion,
                           const QStringList& files) const
    {
        gtDebug() << "(module data updater) running update routines...";

        std::vector<VersionUpdateRoutine> routines = m_updateRoutines;

        std::sort(routines.begin(),
                  routines.end(), [] (const VersionUpdateRoutine& x,
                  const VersionUpdateRoutine& y) { return x.target <
                    y.target; });

        for (auto const& r : routines)
        {
            if (r.target > footprintVersion)
            {
                gtDebug() << "(module data updater) " << r.target.toString()
                          << " - updating...";

                for (auto const& modData : files)
                {
                    QFile file(modData);

                    if (!file.exists())
                    {
                        gtError() << "(module data updater) "
                                  << "WARNING: file does not exists!";
                        gtError() << "(module data updater) |-> " << modData;

                        continue;
                    }

                    QDomDocument document;
                    QString errorStr;
                    int errorLine;
                    int errorColumn;

                    if (!document.setContent(&file, true, &errorStr,
                                             &errorLine, &errorColumn))
                    {
                        gtError() << "(module data updater) "
                                  << "XML ERROR!" << " " << "line" << ": "
                                  << errorLine << " " << "column" << ": "
                                  << errorColumn << " -> " << errorStr;

                        continue;
                    }

                    QDomElement root = document.documentElement();

                    r.f(root, modData);

                    // save file
                    // TODO
                }
            }
            else
            {
                gtDebug() << "(module data updater) " << r.target.toString()
                          << " - skipped ( <= " << footprintVersion.toString()
                          << " )";
            }

        }
    }

};

class GtModuleUpdaterImpl
{
public:
    std::map<std::string, GtModuleUpdateHelper> m_updater;

};

GtModuleUpdater&
GtModuleUpdater::instance()
{
    static GtModuleUpdater updater;
    return updater;
}

void
GtModuleUpdater::registerModuleConverter(const QString &modId,
                                         GtVersionNumber target,
                                         ConverterFunction func)
{
    auto moduleUpdater = m_pimpl->m_updater.find(modId.toStdString());

    if (moduleUpdater != m_pimpl->m_updater.end())
    {
        // found target module
        moduleUpdater->second.m_updateRoutines.push_back({target, func});
    }
    else
    {
        // target module not found. create new entry
        GtModuleUpdateHelper helper;
        helper.m_updateRoutines.push_back({target, func});
        m_pimpl->m_updater.insert({modId.toStdString(), helper});
    }
}

void
GtModuleUpdater::debugModuleConverter()
{
    gtDebug() << "Module Converter: ";

    for (auto const& u : m_pimpl->m_updater)
    {
        gtDebug() << "(module data updater) # "
                  << QString::fromStdString(u.first);

        std::vector<VersionUpdateRoutine> routines = u.second.m_updateRoutines;

        std::sort(routines.begin(),
                  routines.end(), [] (const VersionUpdateRoutine& x,
                  const VersionUpdateRoutine& y)
        { return x.target < y.target; });

        for (auto const& r : routines)
        {
            gtDebug() << "(module data updater) ## ver " << r.target.toString();
        }
    }
}

void
GtModuleUpdater::update(const QMap<QString, GtVersionNumber>& moduleFootprint,
                        const QStringList &files) const
{
    for (auto const& u : m_pimpl->m_updater)
    {
        if (moduleFootprint.contains(QString::fromStdString(u.first)))
        {
            gtDebug() << "(module data updater) found module match - "
                      <<QString::fromStdString(u.first);

            u.second.runUpdateRoutines(
                        moduleFootprint.value(QString::fromStdString(u.first)),
                        files);
        }
    }
}

bool
GtModuleUpdater::upgradesAvailable(
        const QMap<QString, GtVersionNumber>& moduleFootprint) const
{
    gtDebug() << "(module data updater) checking for upgrades...";

    bool retval = false;

    for (auto const& u : m_pimpl->m_updater)
    {
        gtDebug() << "(module data updater) module "
                  << QString::fromStdString(u.first);

        if (moduleFootprint.contains(QString::fromStdString(u.first)))
        {
            GtVersionNumber savedVer =
                    moduleFootprint.value(QString::fromStdString(u.first));

            gtDebug() << "(module data updater) saved version "
                      << savedVer.toString();

            for (auto const& r : u.second.m_updateRoutines)
            {
                if (savedVer < r.target)
                {
                    gtDebug() << "(module data updater) ## update available "
                              << r.target.toString();

                    retval = true;
                }
            }

        }
    }

    return retval;
}

QStringList
GtModuleUpdater::availableModuleUpgrades(const QMap<QString,
                                        GtVersionNumber>& moduleFootprint) const
{
    QStringList retval;

    for (auto const& u : m_pimpl->m_updater)
    {
        if (moduleFootprint.contains(QString::fromStdString(u.first)))
        {
            GtVersionNumber savedVer =
                    moduleFootprint.value(QString::fromStdString(u.first));

            if (std::any_of(u.second.m_updateRoutines.begin(),
                            u.second.m_updateRoutines.end(),
                            [savedVer](const VersionUpdateRoutine& r)
            {return savedVer < r.target;}))
            {
                retval << QString::fromStdString(u.first);
            }
        }
    }

    return retval;
}

QList<GtVersionNumber>
GtModuleUpdater::availableUpgrades(const QString& moduleId,
                                   const GtVersionNumber &savedVer) const
{
    QList<GtVersionNumber> retval;

    for (auto const& u : m_pimpl->m_updater)
    {
        if (moduleId == QString::fromStdString(u.first))
        {

            std::vector<VersionUpdateRoutine> routines =
                    u.second.m_updateRoutines;

            std::sort(routines.begin(),
                      routines.end(), [] (const VersionUpdateRoutine& x,
                      const VersionUpdateRoutine& y) { return x.target <
                        y.target; });

            for (auto const& r : routines)
            {
                if (savedVer < r.target)
                {
                    retval << r.target;
                }
            }

            break;
        }
    }

    return retval;
}

GtModuleUpdater::GtModuleUpdater() :
    m_pimpl{std::make_unique<GtModuleUpdaterImpl>()}
{

}
