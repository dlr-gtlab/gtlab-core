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

#include "gt_preferencesapp.h"
#include <QStandardItemModel>

#include <cassert>

struct LoggingVerbosity
{
    gt::log::Verbosity verbosity;
    QString name;
};

struct LoggingLevel
{
    gt::log::Level level;
    QString name;
};

std::array<LoggingVerbosity, 3> const s_verbosityLevels{
    LoggingVerbosity{gt::log::Silent    , QStringLiteral("Silent")},
    LoggingVerbosity{gt::log::Medium    , QStringLiteral("Medium")},
    LoggingVerbosity{gt::log::Everything, QStringLiteral("High")  }
};

std::array<LoggingLevel, 4> const s_loggingLevels{
    LoggingLevel{gt::log::TraceLevel,   QStringLiteral("Trace-Level")},
    LoggingLevel{gt::log::DebugLevel,   QStringLiteral("Debug-Level")},
    LoggingLevel{gt::log::InfoLevel,    QStringLiteral("User-Level")},
    LoggingLevel{gt::log::WarningLevel, QStringLiteral("Warnings only")}
};

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
    formLay->addRow(tr("Logging verbosity:"), m_verbositySelection);

    // order verbosity levels depending on their value
    QStringList loggingLevels;
    std::transform(std::begin(s_loggingLevels), std::end(s_loggingLevels),
                   std::back_inserter(loggingLevels), [](auto const& entry){
        return entry.name;
    });
    m_loggingLevelSelection = new QComboBox;
    m_loggingLevelSelection->addItems(loggingLevels);
    formLay->addRow(tr("Logging level:"), m_loggingLevelSelection);

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

    // verbosity
    {
        // not using verbosity setting here to get actual logging verbosity
        int  verbosityLevel = gt::log::Logger::instance().verbosity();

        // set verbosity text
        auto iter = std::find_if(std::begin(s_verbosityLevels),
                                 std::end(s_verbosityLevels),
                                 [=](auto const& entry){
            return verbosityLevel <= entry.verbosity;
        });

        if (iter != std::end(s_verbosityLevels))
        {
            m_verbositySelection->setCurrentText(iter->name);
        }
    }

    // logging level
    {
        // not using logging level setting here to get actual logging level
        gt::log::Level loggingLevel = gt::log::Logger::instance().loggingLevel();

        // set verbosity text
        auto iter = std::find_if(std::begin(s_loggingLevels),
                                 std::end(s_loggingLevels),
                                 [=](auto const& entry){
            return loggingLevel <= entry.level;
        });

        if (iter != std::end(s_loggingLevels))
        {
            m_loggingLevelSelection->setCurrentText(iter->name);
        }
    }

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
    {
        auto verbosityText = m_verbositySelection->currentText();
        auto verbosity = gt::log::Silent;

        auto iter = std::find_if(std::begin(s_verbosityLevels),
                                 std::end(s_verbosityLevels),
                                 [&](auto const& entry){
                                     return verbosityText == entry.name;
                                 });

        if (iter != std::end(s_verbosityLevels))
        {
            verbosity = iter->verbosity;
        }

        settings.setLoggingVerbosity(verbosity);
        gt::log::Logger::instance().setVerbosity(verbosity);
    }

    // logging level
    {
        auto loggingLevelText = m_loggingLevelSelection->currentText();
        auto loggingLevel = gt::log::Logger::instance().loggingLevel();

        auto iter = std::find_if(std::begin(s_loggingLevels),
                                 std::end(s_loggingLevels),
                                 [&](auto const& entry){
            return loggingLevelText == entry.name;
        });

        if (iter != std::end(s_loggingLevels))
        {
            loggingLevel = iter->level;
        }

        settings.setLoggingLevel(loggingLevel);
        gt::log::Logger::instance().setLoggingLevel(loggingLevel);
    }

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
}

void
GtPreferencesApp::loadSettings(const GtSettings&)
{
}

void
GtPreferencesApp::onAutoSaveTriggered(bool /*val*/)
{
    // TODO: remove me
}

