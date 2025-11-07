/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTableWidget>
#include <QCheckBox>
#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QSettings>

#include "gt_application.h"
#include "gt_settings.h"
#include "gt_logmodel.h"
#include "gt_logging.h"
#include "gt_icons.h"
#include "gt_moduledirectoriestab.h"

#include "gt_preferencesapp.h"
#include <QStandardItemModel>

#include <gt_moduleloader.h>

#include <cassert>

struct LoggingVerbosity
{
    gt::log::Verbosity verbosity;
    QString name;
};

std::array<LoggingVerbosity, 3> const s_verbosityLevels{
    LoggingVerbosity{gt::log::Silent    , QStringLiteral("Silent")},
    LoggingVerbosity{gt::log::Medium    , QStringLiteral("Medium")},
    LoggingVerbosity{gt::log::Everything, QStringLiteral("High")  }
};

namespace
{

/// returns the display text for the current verbosity level
QString const& currentVerbosityName()
{
    // not using verbosity setting here to get actual logging verbosity
    int verbosityLevelValue = gt::log::Logger::instance().verbosity();

    auto iter = std::find_if(std::begin(s_verbosityLevels),
                             std::end(s_verbosityLevels),
                             [=](auto const& entry){
        return verbosityLevelValue <= entry.verbosity;
    });

    if (iter != std::end(s_verbosityLevels))
    {
        return iter->name;
    }

    /// fallback
    return s_verbosityLevels[0].name;
}

/// returns the verbosity level for the given text
gt::log::Verbosity verbosityLevel(QString const& verbosityText)
{
    auto iter = std::find_if(std::begin(s_verbosityLevels),
                             std::end(s_verbosityLevels),
                             [&](auto const& entry){
                                 return verbosityText == entry.name;
                             });

    if (iter != std::end(s_verbosityLevels))
    {
        return iter->verbosity;
    }

    return s_verbosityLevels[0].verbosity;
}

} // namespace

GtPreferencesApp::GtPreferencesApp() :
    GtPreferencesPage(tr("Application"))
{
    setIcon(gt::gui::icon::applicationSettings());
    QTabWidget* tabWidget = new QTabWidget;

    tabWidget->setContentsMargins(0, 0, 0, 0);

    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    layout->addWidget(tabWidget);

    QWidget* generalPage = new QWidget;
    tabWidget->addTab(generalPage, tr("General"));

    m_lastOpenedProject = new QCheckBox(tr("Open last opened "
                                           "project on startup"));
    m_openWelcomePage = new QCheckBox(tr("Show welcome page"));
    m_updateAtStartup = new QCheckBox(tr("Search for Updates on startup"));

    QVBoxLayout* generalLayout = new QVBoxLayout(generalPage);

    generalLayout->addWidget(m_lastOpenedProject);
    generalLayout->addWidget(m_openWelcomePage);
    generalLayout->addWidget(m_updateAtStartup);

    QFormLayout* formLay = new QFormLayout;

    m_maxLogSpin = new QSpinBox;
    m_maxLogSpin->setMinimum(10);
    m_maxLogSpin->setMaximum(100000);
    formLay->addRow(tr("Max. logging length:"), m_maxLogSpin);

    // order verbosity levels depending on their value
    QStringList verbosityLevels;
    std::transform(std::begin(s_verbosityLevels), std::end(s_verbosityLevels),
                   std::back_inserter(verbosityLevels), [](auto const& entry){
        return entry.name;
    });
    m_verbositySelection = new QComboBox;
    m_verbositySelection->addItems(verbosityLevels);
    m_verbositySelection->setCurrentText(currentVerbosityName());
    formLay->addRow(tr("Logging verbosity:"), m_verbositySelection);

    m_themeSelection = new QComboBox(this);
    m_themeSelection->addItem(tr("System selection"));
    m_themeSelection->addItem(tr("Bright mode"));
    m_themeSelection->addItem(tr("Dark mode"));

    // disable system selection on linux
#if !defined(WIN32)
    auto* model = qobject_cast<QStandardItemModel*>(m_themeSelection->model());
    assert(model != nullptr && model->item(0));
    auto* item = model->item(0);
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
#endif

    formLay->addRow(tr("Theme section:"), m_themeSelection);

    generalLayout->addLayout(formLay);

    generalLayout->addStretch(1);

    m_moduleDirsTab = new GtModuleDirectoriesTab;
    tabWidget->addTab(m_moduleDirsTab, tr("Module Directories"));

    QWidget* processRunnerPage = new QWidget;
    auto idx = tabWidget->addTab(processRunnerPage, tr("Process Runner"));

    m_useExtendedProcessExecutor =
            new QCheckBox(tr("Enable the experimental Process Runner"));
    m_autostartProcessExecutor =
            new QCheckBox(tr("Autostart Process Runner on task execution"));

    QVBoxLayout* processRunnerLayout = new QVBoxLayout(processRunnerPage);

    processRunnerLayout->addWidget(m_useExtendedProcessExecutor);
    processRunnerLayout->addWidget(m_autostartProcessExecutor);

    processRunnerLayout->addStretch(1);

    tabWidget->setTabVisible(idx, gtApp->devMode());

    auto* settings = gtApp->settings();
    assert(settings);

    m_lastOpenedProject->setChecked(settings->openLastProject());
    m_openWelcomePage->setChecked(settings->showStartupPage());
    m_updateAtStartup->setChecked(settings->searchForUpdate());

    // process runner
    m_useExtendedProcessExecutor->setChecked(
                settings->useExtendedProcessExecutor());
    m_autostartProcessExecutor->setChecked(
                settings->autostartProcessRunner());

    // log length
    m_maxLogSpin->setValue(settings->maxLogLength());

    // theme selection
    QString themeMode = settings->themeMode();
    if (themeMode == "bright")
    {
        m_themeSelection->setCurrentIndex(1);
    }
    else if (themeMode == "dark")
    {
        m_themeSelection->setCurrentIndex(2);
    }
    else
    {
        m_themeSelection->setCurrentIndex(0);
    }
}

void
GtPreferencesApp::saveSettings(GtSettings& settings) const
{
    settings.setOpenLastProject(m_lastOpenedProject->isChecked());
    settings.setShowStartupPage(m_openWelcomePage->isChecked());
    settings.setSearchForUpdate(m_updateAtStartup->isChecked());

    // log length
    settings.setMaxLogLength(m_maxLogSpin->value());
    GtLogModel::instance().setMaxLogLength(m_maxLogSpin->value());

    // verbosity
    auto verbosity = verbosityLevel(m_verbositySelection->currentText());
    settings.setLoggingVerbosity(verbosity);
    gt::log::Logger::instance().setVerbosity(verbosity);

    // process executor
    settings.setUseExtendedProcessExecutor(
                m_useExtendedProcessExecutor->isChecked());
    settings.setAutostartProcessRunner(
                m_autostartProcessExecutor->isChecked());

    // theme selection
    int index = m_themeSelection->currentIndex();

    if (index == 1)
    {
        settings.setThemeMode("bright");
        gtApp->setDarkMode(false);
    }
    else if (index == 2)
    {
        settings.setThemeMode("dark");
        gtApp->setDarkMode(true);
    }
    else
    {
        settings.setThemeMode("system");
        gtApp->setDarkMode(settings.darkMode());
    }

    settings.setUserModuleDirs(m_moduleDirsTab->userPaths());
}

void
GtPreferencesApp::loadSettings(const GtSettings& settings)
{
    assert(m_moduleDirsTab);

    m_moduleDirsTab->setDefaultInstallPath(GtModuleLoader::applicationModuleDir());
    m_moduleDirsTab->setDefaultUserPath(GtModuleLoader::defaultUserModuleDir());
    m_moduleDirsTab->setUserPaths(settings.userModuleDirs());
}

void
GtPreferencesApp::onAutoSaveTriggered(bool /*val*/)
{
    // TODO: remove me
}

