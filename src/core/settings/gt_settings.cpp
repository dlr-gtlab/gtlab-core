/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_settings.h"
#include "gt_settingsitem.h"
#include "gt_loglevel.h"

#include <QDir>
#include <QSettings>
#include <QMap>
#include <QString>

struct GtSettings::Impl
{

    ///
    GtSettingsItem* lastSession;

    ///
    GtSettingsItem* openSession;

    ///
    GtSettingsItem* language;

    ///
    GtSettingsItem* firstRun;

    ///
    GtSettingsItem* lastPath;

    ///
    GtSettingsItem* lastPerspective;

    ///
    GtSettingsItem* showStartupPage;

    ///
    GtSettingsItem* searchForUpdate;

    ///
    GtSettingsItem* lastProject;

    ///
    GtSettingsItem* openProject;

    ///
    GtSettingsItem* maxLogLength;

    ///
    GtSettingsItem* loggingVerbosity;

    GtSettingsItem* loggingLevel;

    ///
    GtSettingsItem* lastProcessElements;

    /// Explorer dock widget expand states setting
    GtSettingsItem* explorerExpandStates;

    /// Settings for short cuts in GTlab
    GtSettingsItem* shortcutsTable;

    /// Settings for theme selection
    GtSettingsItem* themeSelection;

    /// whether to use the extended process executor
    GtSettingsItem* m_useExtendedProcessExecutor;

    /// Whether to autostart the process runner
    GtSettingsItem* m_autostartProcessRunner;
};

GtSettings::GtSettings()
    : pimpl(std::make_unique<Impl>())
{
    pimpl->lastSession = registerSetting(
                        QStringLiteral("application/general/lastsession"),
                        QStringLiteral("default"));
    pimpl->openSession = registerSetting(
                        QStringLiteral("application/general/opensession"),
                        false);
    pimpl->language = registerSetting(QStringLiteral("application/general/language"),
                                 QStringLiteral(""));
    pimpl->firstRun = registerSetting(QStringLiteral("application/general/firstRun"),
                                 true);
    pimpl->lastPath = registerSetting(QStringLiteral("application/general/lastPath"),
                                 QStringLiteral(""));

    pimpl->lastPerspective =
            registerSetting(
                QStringLiteral("application/general/lastPerspective"),
                QStringLiteral("default"));
    pimpl->showStartupPage = registerSetting(
                            QStringLiteral("application/general/startuppage"),
                            true);
    pimpl->searchForUpdate =
            registerSetting(
                QStringLiteral("application/general/searchforupdate"),
                true);
    pimpl->lastProject = registerSetting(
                        QStringLiteral("application/general/lastproject"),
                        QStringLiteral(""));
    pimpl->openProject = registerSetting(
                        QStringLiteral("application/general/openproject"),
                        false);
    pimpl->maxLogLength = registerSetting(
                         QStringLiteral("application/general/maxloglength"),
                         (int) 2000);
    pimpl->loggingVerbosity = registerSetting(
                         QStringLiteral("application/general/loggingVerbosity"),
                         (int) 0);
    pimpl->loggingLevel = registerSetting(
                         QStringLiteral("application/general/loggingLevel"),
                         (int)gt::log::InfoLevel);
    pimpl->lastProcessElements =
            registerSetting(
                QStringLiteral("application/process/lastelements"),
                QStringList());
    pimpl->explorerExpandStates =
            registerSetting(
                QStringLiteral("application/explorer/expandstates"),
                QStringList());

    pimpl->shortcutsTable = registerSetting(
                QStringLiteral("application/general/shortCuts"));

    pimpl->themeSelection = registerSettingRestart(
                        QStringLiteral("application/general/themeSelection"),
                        QStringLiteral("system"));

    pimpl->m_useExtendedProcessExecutor = registerSetting(
                QStringLiteral("application/process_runner/enabled"), false);

    pimpl->m_autostartProcessRunner = registerSetting(
                QStringLiteral("application/process_runner/autostart"), false);
}

QList<GtShortCutSettingsData>
GtSettings::shortcutsList() const
{
    QMap<QString, QVariant> helpingMap =
            pimpl->shortcutsTable->getValue().toMap();

    QList<GtShortCutSettingsData> retVal;

    for (QString const & s : helpingMap.keys())
    {
        QVariant v = helpingMap.value(s);

        QStringList list = v.toStringList();

        QString cat = list.at(1);
        QString key = list.at(0);

        bool readOnly = false;
        if (list.size() > 2)
        {
            if (list.at(2) == "true")
            {
                readOnly = true;
            }
        }

        retVal.append({s, cat, key, readOnly});
    }

    return retVal;
}

void
GtSettings::setShortcutsTable(QList<GtShortCutSettingsData> const& list)
{
    QMap<QString, QVariant> helpingMap;

    for (auto const& s : list)
    {
        helpingMap.insert(s.id, s.dataToVariant());
    }

    pimpl->shortcutsTable->setValue(QVariant::fromValue(helpingMap));
}


QVariant
GtSettings::initialShortCuts() const
{
    return {};
}

QList<GtShortCutSettingsData>
GtSettings::intialShortCutsList() const
{
    return {};
}


QString
GtSettings::themeMode()
{
    return pimpl->themeSelection->getValue().toString();
}

bool
GtSettings::darkMode()
{
    QString s = themeMode();

    if (s.toStdString() == ("dark"))
    {
        return true;
    }
    else if (s.toStdString() == ("bright"))
    {
        return false;
    }
    else    // system mode
    {
#ifdef Q_OS_WIN
        std::string set = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
        QString set2 = QString::fromStdString(set);

        std::string a = "AppsUseLightTheme";
        QString a2 = QString::fromStdString(a);

        QSettings settings(set2 ,QSettings::NativeFormat);
        if (settings.value(a2) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
#else //Dark mode detection is inactive for linux
        return false;
#endif
    }
}

void
GtSettings::setThemeMode(const QString& theme)
{
    pimpl->themeSelection->setValue(theme);
}

GtSettings::~GtSettings() = default;

QString
GtSettings::lastSession()
{
    return pimpl->lastSession->getValue().toString();
}

void
GtSettings::setLastSession(const QString& session)
{
    pimpl->lastSession->setValue(session);
}

QString
GtSettings::lastProject()
{
    return pimpl->lastProject->getValue().toString();
}

void
GtSettings::setLastProject(const QString& project)
{
    pimpl->lastProject->setValue(project);
}

QString
GtSettings::lastPerspective()
{
    return pimpl->lastPerspective->getValue().toString();
}

void
GtSettings::setLastPerspective(const QString& perspective)
{
    pimpl->lastPerspective->setValue(perspective);
}

QString
GtSettings::lastPath()
{
    // default to home directory
    auto tmp = pimpl->lastPath->getValue().toString();
    if (tmp.isEmpty())
    {
        return QDir::homePath();
    }
    return tmp;
}

void
GtSettings::setLastPath(const QString& path)
{
    pimpl->lastPath->setValue(path);
}

bool
GtSettings::openLastSession()
{
    return pimpl->openSession->getValue().toBool();
}

void
GtSettings::setOpenLastSession(bool value)
{
    pimpl->openSession->setValue(value);
}

bool
GtSettings::openLastProject()
{
    return pimpl->openProject->getValue().toBool();
}

void
GtSettings::setOpenLastProject(bool value)
{
    pimpl->openProject->setValue(value);
}

QString
GtSettings::language()
{
    return pimpl->language->getValue().toString();
}


void
GtSettings::setLanguage(const QString& language)
{
    pimpl->language->setValue(language);
}

bool
GtSettings::firstApplicationRun()
{
    return !pimpl->firstRun->getValue().toBool();
}

void
GtSettings::setShowStartupPage(bool value)
{
    pimpl->showStartupPage->setValue(value);
}

bool
GtSettings::showStartupPage()
{
    return pimpl->showStartupPage->getValue().toBool();
}

void
GtSettings::setSearchForUpdate(bool value)
{
    pimpl->searchForUpdate->setValue(value);
}

bool
GtSettings::searchForUpdate()
{
    return pimpl->searchForUpdate->getValue().toBool();
}

void
GtSettings::setMaxLogLength(int value)
{
    pimpl->maxLogLength->setValue(value);
}

int
GtSettings::maxLogLength()
{
    return pimpl->maxLogLength->getValue().toInt();
}

void
GtSettings::setLoggingVerbosity(int value)
{
    pimpl->loggingVerbosity->setValue(value);
}

int
GtSettings::loggingVerbosity() const
{
    return pimpl->loggingVerbosity->getValue().toInt();
}

void
GtSettings::setLoggingLevel(int value)
{
    pimpl->loggingLevel->setValue(value);
}

int
GtSettings::loggingLevel() const
{
    return pimpl->loggingLevel->getValue().toInt();
}

void
GtSettings::setLastProcessElements(const QStringList& list)
{
    if (list.size() > 10)
    {
        return;
    }

    pimpl->lastProcessElements->setValue(list);
}

QStringList
GtSettings::lastProcessElements()
{
    return pimpl->lastProcessElements->getValue().toStringList();
}

void
GtSettings::setExplorerExpandStates(const QStringList& list)
{
    pimpl->explorerExpandStates->setValue(list);
}

QStringList
GtSettings::explorerExpandStates()
{
    return pimpl->explorerExpandStates->getValue().toStringList();
}

bool
GtSettings::useExtendedProcessExecutor() const
{
    return pimpl->m_useExtendedProcessExecutor->getValue().toBool();
}

void
GtSettings::setUseExtendedProcessExecutor(bool value)
{
    return pimpl->m_useExtendedProcessExecutor->setValue(value);
}

bool
GtSettings::autostartProcessRunner() const
{
    return pimpl->m_autostartProcessRunner->getValue().toBool();
}

void
GtSettings::setAutostartProcessRunner(bool value)
{
    return pimpl->m_autostartProcessRunner->setValue(value);
}
