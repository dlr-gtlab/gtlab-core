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

struct VersionUpdateRoutine
{
    GtVersionNumber target;
    ConverterFunction f;
};

class GtModuleUpdateHelper
{
public:
    std::vector<VersionUpdateRoutine> m_updateRoutines;

    //logic
    void runUpdateRoutines(const GtVersionNumber& footprintVersion,
                           const QStringList& moduleData) const
    {
        gtDebug() << "(module data updater) running update routines...";

        std::vector<VersionUpdateRoutine> routines = m_updateRoutines;

        std::sort(routines.begin(),
                  routines.end(), [] (VersionUpdateRoutine &x,
                  VersionUpdateRoutine &y) { return x.target < y.target; });

        for (auto const& r : routines)
        {
            if (r.target > footprintVersion)
            {
                gtDebug() << "(module data updater) " << r.target.toString()
                          << " - updating...";

                for (auto const& modData : moduleData)
                {
                    QFile file(modData);

                    if (!file.exists())
                    {
                        qWarning() << "(module data updater) "
                                   << "WARNING: file does not exists!";
                        qWarning() << "(module data updater) |-> " << modData;

                        continue;
                    }


                    QDomDocument document;
                    QString errorStr;
                    int errorLine;
                    int errorColumn;

                    if (!document.setContent(&file, true, &errorStr,
                                             &errorLine, &errorColumn))
                    {
                        gtDebug() << "(module data updater) "
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
                  routines.end(), [] (VersionUpdateRoutine &x,
                  VersionUpdateRoutine &y) { return x.target < y.target; });

        for (auto const& r : routines)
        {
            gtDebug() << "(module data updater) ## ver " << r.target.toString();
        }
    }
}

void
GtModuleUpdater::update(const QMap<QString, GtVersionNumber>& moduleFootprint,
                        const QStringList &moduleData) const
{
    for (auto const& u : m_pimpl->m_updater)
    {
        if (moduleFootprint.contains(QString::fromStdString(u.first)))
        {
            gtDebug() << "(module data updater) found module match - "
                      <<QString::fromStdString(u.first);

            u.second.runUpdateRoutines(
                        moduleFootprint.value(QString::fromStdString(u.first)),
                        moduleData);
        }
    }
}

GtModuleUpdater::GtModuleUpdater() :
    m_pimpl{std::make_unique<GtModuleUpdaterImpl>()}
{

}
