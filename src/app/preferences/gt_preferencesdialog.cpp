/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include "gt_preferencesdialog.h"
#include "gt_preferencesapp.h"
#include "gt_preferencessession.h"
#include "gt_preferenceslanguage.h"
#include "gt_preferencesperspective.h"
#include "gt_preferencesaccess.h"
#include "gt_preferencespathsettings.h"
#include "gt_preferencesshortcuts.h"
#include "gt_preferencescommandlinetemplates.h"
#include "gt_icons.h"
#include "gt_application.h"
#include "gt_settings.h"


GtPreferencesDialog::GtPreferencesDialog(int initItem, QWidget* parent) :
    GtDialog(parent)
{
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
    addPage(new GtPreferencesApp);
    addPage(new GtPreferencesSession);
    addPage(new GtPreferencesPerspective);
    addPage(new GtPreferencesShortCuts);
    addPage(new GtPreferencesLanguage);
    addPage(new GtPreferencesAccess);
    addPage(new GtPreferencesPathSettings);
    addPage(new GtPreferencesCommandlineTemplates);

    // Add pages from modules
    for (const auto& builder : qAsConst(GtApplication::customPreferencePages()))
    {
        addPage(builder());
    }

    QPushButton* saveButton = new QPushButton(tr("Save"));
    saveButton->setIcon(gt::gui::icon::save());
    QPushButton* closeButton = new QPushButton(tr("Cancel"));
    closeButton->setIcon(gt::gui::icon::cancel());

    connect(m_contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));

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
    setWindowIcon(gt::gui::icon::config());
    setFixedHeight(530);
    setFixedWidth(600);

    loadSettings();
}

void
GtPreferencesDialog::setStartingPage(const QString& title)
{
    if (!m_contentsWidget || title.isEmpty())
    {
        return;
    }

    m_contentsWidget->setCurrentRow(pageIndex(title));
}

QString
GtPreferencesDialog::currentPageTitle() const
{
    assert(m_contentsWidget);

    const auto* page = m_pages[m_contentsWidget->currentRow()];
    assert(page);

    return page->title();
}

void
GtPreferencesDialog::addPage(GtPreferencesPage* page)
{
    if (!page)
    {
        return;
    }

    m_pages.append(page);

    auto layout = new QVBoxLayout;
    auto title = new QLabel(page->title());

    QFont font = title->font();
    font.setBold(true);
    title->setFont(font);

    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(page);

    auto container = new QWidget(this);
    container->setLayout(layout);

    m_pagesWidget->addWidget(container);

    // Add a button (icon + text) the the list on the left
    QListWidgetItem* button = new QListWidgetItem(m_contentsWidget);
    button->setIcon(page->icon());
    button->setText(page->titleShort());
    button->setTextAlignment(Qt::AlignHCenter);
    button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    button->setWhatsThis(page->title());
    button->setSizeHint(QSize(100, 56));

    if (page->isEnabled())
    {
        button->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }
    else
    {
        button->setFlags(button->flags() & ~Qt::ItemIsEnabled);
    }
}

int
GtPreferencesDialog::pageIndex(const QString& title)
{
    auto it = std::find_if(m_pages.begin(), m_pages.end(),
                             [&title](GtPreferencesPage* page) {
        return title == page->title();
    });

    return it != m_pages.end() ? std::distance(m_pages.begin(), it) : -1;
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
    for (auto page : qAsConst(m_pages))
    {
        if (page)
        {
            page->saveSettings(*gtApp->settings());
        }
    }

    if (gtApp->settings()->requiresAppRestart())
    {
        QMessageBox::information(this, tr("Restart required"),
            tr("The changes require a restart of GTlab to take effect"));
    }

    accept();
}

void
GtPreferencesDialog::loadSettings()
{

    for (auto page : qAsConst(m_pages))
    {
        if (page)
        {
            page->loadSettings(*gtApp->settings());
        }
    }
}
