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

#include "gt_preferencesapp.h"

GtPreferencesApp::GtPreferencesApp() :
    m_autoSaveModifications(nullptr),
    m_autoSaveInterval(nullptr)
{
    setTitle(tr("Application"));

    QTabWidget* tabWidget = new QTabWidget;

    tabWidget->setContentsMargins(0, 0, 0, 0);

    layout()->addWidget(tabWidget);

    QWidget* generalPage = new QWidget;
    tabWidget->addTab(generalPage, tr("General"));

    m_lastOpenedProject =
            new QCheckBox(tr("Open last opened project on startup"));
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

    formLay->addRow(tr("Max. logging length:"), m_maxLogSpin);

    m_themeSelection = new QComboBox(this);
    m_themeSelection->addItem(tr("System selection"));
    m_themeSelection->addItem(tr("Bright mode"));
    m_themeSelection->addItem(tr("Dark mode"));
    formLay->addRow(tr("Theme section:"), m_themeSelection);

    generalLayout->addLayout(formLay);

    generalLayout->addStretch(1);

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

    m_lastOpenedProject->setChecked(gtApp->settings()->openLastProject());
    m_openWelcomePage->setChecked(gtApp->settings()->showStartupPage());
    m_updateAtStartup->setChecked(gtApp->settings()->searchForUpdate());

    m_maxLogSpin->setValue(gtApp->settings()->maxLogLength());

    QString themeMode = gtApp->settings()->themeMode();
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
    if (m_lastOpenedProject->isChecked())
    {
        settings.setOpenLastProject(true);
    }
    else
    {
        settings.setOpenLastProject(false);
    }

    if (m_openWelcomePage->isChecked())
    {
        settings.setShowStartupPage(true);
    }
    else
    {
        settings.setShowStartupPage(false);
    }

    if (m_updateAtStartup->isChecked())
    {
        settings.setSearchForUpdate(true);
    }
    else
    {
        settings.setSearchForUpdate(false);
    }

    settings.setMaxLogLength(m_maxLogSpin->value());

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

