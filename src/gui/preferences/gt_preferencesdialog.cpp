/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>

#include "gt_preferencesdialog.h"
#include "gt_preferencesapp.h"
#include "gt_preferencessession.h"
#include "gt_preferencesplugins.h"
#include "gt_preferenceslanguage.h"
#include "gt_preferencesperspective.h"
#include "gt_preferencesaccess.h"
#include "gt_preferencespathsettings.h"
#include "gt_application.h"
#include "gt_accessmanager.h"

GtPreferencesDialog::GtPreferencesDialog(int initItem)
{
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    m_contentsWidget = new QListWidget;
    m_contentsWidget->setViewMode(QListView::IconMode);
    m_contentsWidget->setIconSize(QSize(32, 32));
    m_contentsWidget->setMovement(QListView::Static);
    m_contentsWidget->setMaximumWidth(135);
    m_contentsWidget->setSizePolicy(QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Expanding);
    m_contentsWidget->setSpacing(12);
    m_contentsWidget->setFrameStyle(QFrame::NoFrame);

    m_pagesWidget = new QStackedWidget;
    m_pagesWidget->setMinimumWidth(400);
    m_pagesWidget->addWidget(new GtPreferencesApp);
    m_pagesWidget->addWidget(new GtPreferencesSession);
    m_pagesWidget->addWidget(new GtPreferencesPerspective);
    m_pagesWidget->addWidget(new GtPreferencesPlugins);
    m_pagesWidget->addWidget(new GtPreferencesLanguage);
    m_pagesWidget->addWidget(new GtPreferencesAccess);
    m_pagesWidget->addWidget(new GtPreferencesPathSettings);

    QPushButton* saveButton = new QPushButton(tr("Save"));
    saveButton->setIcon(gtApp->icon("saveProjectIcon_16.png"));
    QPushButton* closeButton = new QPushButton(tr("Cancel"));
    closeButton->setIcon(gtApp->icon("closeIcon_16.png"));

    createIcons();
    m_contentsWidget->setCurrentRow(initItem);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->setContentsMargins(4, 4, 4, 4);
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(closeButton);

    QVBoxLayout* verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(m_pagesWidget);
    verticalLayout->addLayout(buttonsLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_contentsWidget);
    mainLayout->addLayout(verticalLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);

    setWindowTitle(tr("Preferences"));
    setWindowIcon(gtApp->icon("configIcon_16.png"));
    setFixedHeight(470);
    setFixedWidth(600);
}

void
GtPreferencesDialog::changePage(QListWidgetItem* current,
                                QListWidgetItem* previous)
{
    if (!current)
    {
        current = previous;
    }

    m_pagesWidget->setCurrentIndex(m_contentsWidget->row(current));
}

void
GtPreferencesDialog::saveChanges()
{
    QObjectList pageList = m_pagesWidget->children();

    for (int i = 0; i < pageList.size(); i++)
    {
        GtPreferencesPage* page = dynamic_cast<GtPreferencesPage*>(pageList[i]);

        if (page != NULL)
        {
            page->saveSettings();
        }
    }

    accept();
}

void
GtPreferencesDialog::createIcons()
{
    QListWidgetItem* configButton = new QListWidgetItem(m_contentsWidget);
    configButton->setIcon(gtApp->icon("applicationIcon.png"));
    configButton->setText(tr("Application"));
    configButton->setTextAlignment(Qt::AlignHCenter);
    configButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    configButton->setWhatsThis(tr("Application Settings"));
    configButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* sessionButton = new QListWidgetItem(m_contentsWidget);
    sessionButton->setIcon(gtApp->icon("sessionIcon.png"));
    sessionButton->setText(tr("Session"));
    sessionButton->setTextAlignment(Qt::AlignHCenter);
    sessionButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    sessionButton->setWhatsThis(tr("Session Configurations"));
    sessionButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* perspectivesButton = new QListWidgetItem(m_contentsWidget);
    perspectivesButton->setIcon(gtApp->icon("perspectivesIcon.png"));
    perspectivesButton->setText(tr("Perspectives"));
    perspectivesButton->setTextAlignment(Qt::AlignHCenter);
    perspectivesButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    perspectivesButton->setWhatsThis(tr("Perspectives Configurations"));
    perspectivesButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* pluginsButton = new QListWidgetItem(m_contentsWidget);
    pluginsButton->setIcon(gtApp->icon("pluginSettingsIcon.png"));
    pluginsButton->setText(tr("Plugins"));
    pluginsButton->setTextAlignment(Qt::AlignHCenter);
    pluginsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pluginsButton->setWhatsThis(tr("Plugins"));
    pluginsButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* languageButton = new QListWidgetItem(m_contentsWidget);
    languageButton->setIcon(gtApp->icon("languageIcon.png"));
    languageButton->setText(tr("Language"));
    languageButton->setTextAlignment(Qt::AlignHCenter);
    languageButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    languageButton->setWhatsThis(tr("Language settings"));
    languageButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* accessButton = new QListWidgetItem(m_contentsWidget);
    accessButton->setIcon(gtApp->icon("loginIcon.png"));
    accessButton->setText(tr("Access"));
    accessButton->setTextAlignment(Qt::AlignHCenter);

    if (!gtAccessManager->isEmpty())
    {
        accessButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    else
    {
        accessButton->setFlags(accessButton->flags() & ~Qt::ItemIsEnabled);
    }

    accessButton->setWhatsThis(tr("Access settings"));
    accessButton->setSizeHint(QSize(100, 50));

    QListWidgetItem* pathButton = new QListWidgetItem(m_contentsWidget);
    pathButton->setIcon(gtApp->icon("pathSettingsIcon.png"));
    pathButton->setText(tr("Environment"));
    pathButton->setTextAlignment(Qt::AlignHCenter);
    pathButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    pathButton->setWhatsThis(tr("Environment variables"));
    pathButton->setSizeHint(QSize(100, 50));

    connect(m_contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            SLOT(changePage(QListWidgetItem*, QListWidgetItem*)));
}

