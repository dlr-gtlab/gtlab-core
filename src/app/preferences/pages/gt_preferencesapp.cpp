/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

// Maps verbosity levels to strings
const static QMap<int, QString> s_verbosityLevels{
    {gt::log::Silent    , QStringLiteral("Silent")},
    {gt::log::Medium    , QStringLiteral("Medium")},
    {gt::log::Everything, QStringLiteral("High")  },
};

GtPreferencesApp::GtPreferencesApp() :
    GtPreferencesPage(tr("Application"))
{
    setIcon(gt::gui::icon::application());
    QTabWidget* tabWidget = new QTabWidget;

    tabWidget->setContentsMargins(0, 0, 0, 0);

    auto layout = new QVBoxLayout();
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

//    QWidget* intervalWid = new QWidget;

//    QHBoxLayout* intervalLay = new QHBoxLayout;
//    intervalLay->setContentsMargins(0, 0, 0, 0);

//    intervalWid->setLayout(intervalLay);

//    m_autoSaveModifications = new QCheckBox(tr("Auto-save modified project"));
//    intervalLay->addWidget(m_autoSaveModifications);

//    QLabel* intervalLbl = new QLabel(tr("Interval:"));
//    intervalLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
//    intervalLay->addWidget(intervalLbl);

//    m_autoSaveInterval = new QSpinBox;
//    m_autoSaveInterval->setMaximum(0);
//    m_autoSaveInterval->setMaximum(60);
//    m_autoSaveInterval->setMaximumWidth(70);
//    m_autoSaveInterval->setEnabled(false);
//    m_autoSaveInterval->setSuffix(QStringLiteral(" min"));

//    intervalLay->addWidget(m_autoSaveInterval);

//    generalLayout->addWidget(intervalWid);

    QFormLayout* formLay = new QFormLayout;

    m_maxLogSpin = new QSpinBox;
    m_maxLogSpin->setMinimum(10);
    m_maxLogSpin->setMaximum(100000);

    // order verbosity levels depending on their value
    auto verbosityLevels = s_verbosityLevels.values();
    std::sort(verbosityLevels.begin(), verbosityLevels.end(),
              [&](auto a, auto b){
        return s_verbosityLevels.key(a) < s_verbosityLevels.key(b);
    });
    m_verbositySelection = new QComboBox;
    m_verbositySelection->addItems(verbosityLevels);

    formLay->addRow(tr("Max. logging length:"), m_maxLogSpin);
    formLay->addRow(tr("Logging verbosity:"), m_verbositySelection);

    m_themeSelection = new QComboBox(this);
    m_themeSelection->addItem(tr("System selection"));
    m_themeSelection->addItem(tr("Bright mode"));
    m_themeSelection->addItem(tr("Dark mode"));
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

//    QWidget* notificationPage = new QWidget;
//    tabWidget->addTab(notificationPage, tr("Notifications"));
//    tabWidget->setTabEnabled(1, false);

//    QCheckBox* showHeadsUpNotifications =
//            new QCheckBox(tr("Show heads up notifications"));

//    QVBoxLayout* notificationLayout = new QVBoxLayout(notificationPage);

//    notificationLayout->addWidget(showHeadsUpNotifications);

//    QFormLayout* nFormLayout = new QFormLayout;
//    nFormLayout->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
//    nFormLayout->setFormAlignment(Qt::AlignRight);
//    nFormLayout->setLabelAlignment(Qt::AlignLeft);

//    QLabel* notificationTime = new QLabel(tr("Notification timeout (sec)"));

//    QSpinBox* notifyTimeout = new QSpinBox;

//    notifyTimeout->setFixedWidth(100);
//    notifyTimeout->setRange(1, 10);
//    notifyTimeout->setValue(5);

//    nFormLayout->addRow(notificationTime, notifyTimeout);

//    QLabel* notificationNumber = new QLabel("Max. heads up notifications");

//    QSpinBox* maxNotifications = new QSpinBox;

//    maxNotifications->setFixedWidth(100);
//    maxNotifications->setRange(1, 10);
//    maxNotifications->setValue(10);

//    nFormLayout->addRow(notificationNumber, maxNotifications);

//    notificationLayout->addLayout(nFormLayout);

//    notificationLayout->addStretch(1);

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

    // verbosity: by default use silent
    auto verbosity = gt::log::Silent;
    // check for entry in list
    int  verbosityLevel = settings->loggingVerbosity();
    if (s_verbosityLevels.contains(verbosityLevel))
    {
        verbosity = static_cast<gt::log::Verbosity>(verbosityLevel);
    }
    // set current text
    m_verbositySelection->setCurrentText(s_verbosityLevels.value(verbosity));

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


//    connect(m_autoSaveModifications, SIGNAL(clicked(bool)),
//            SLOT(onAutoSaveTriggered(bool)));
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
    auto verbosity = s_verbosityLevels.key(m_verbositySelection->currentText(),
                                           gt::log::Silent);
    settings.setLoggingVerbosity(verbosity);
    gt::log::Logger::instance().setVerbosity(verbosity);

    settings.setUseExtendedProcessExecutor(
                m_useExtendedProcessExecutor->isChecked());
    settings.setAutostartProcessRunner(
                m_autostartProcessExecutor->isChecked());

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
//    m_autoSaveInterval->setEnabled(val);
}

