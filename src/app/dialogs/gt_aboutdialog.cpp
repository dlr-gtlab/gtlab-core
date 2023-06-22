/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_aboutdialog.cpp
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPushButton>
#include <QIcon>
#include <QDateTime>
#include <QFile>
#include <QTabWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QListWidget>
#include <QSpacerItem>
#include <QApplication>
#include <QDir>
#include <QDesktopServices>

#include "gt_application.h"
#include "gt_filedialog.h"
#include "gt_footprint.h"
#include "gt_logging.h"
#include "gt_icons.h"
#include "gt_versionnumber.h"
#include "gt_moduledetailsdialog.h"
#include "gt_processfactory.h"
#include "gt_mdilauncher.h"
#include "gt_objectfactory.h"
#include "gt_changelogwidget.h"
#include "gt_sharedfunctionswidget.h"
#include "gt_taskdata.h"
#include "gt_taskfactory.h"
#include "gt_splashscreen.h"

#include "gt_aboutdialog.h"

GtAboutLogo::GtAboutLogo(QWidget* parent) :
    QWidget(parent),
    m_clicks(0),
    m_pixmap(gt::gui::pixmap::splash())
{
}

void
GtAboutLogo::paintEvent(QPaintEvent* e)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, m_pixmap);

    QWidget::paintEvent(e);
}

void
GtAboutLogo::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
}


GtAboutDialog::GtAboutDialog(GtAboutDialog::Tabs startPage,
                             QWidget* parent) : GtDialog(parent)
{
    setWindowTitle(tr("About GTlab"));
    setWindowIcon(gt::gui::icon::info2());

    auto* mainL = new QVBoxLayout;

    auto* tabs = new QTabWidget;

    tabs->addTab(coreAbout(), tr("Core"));
    tabs->addTab(changeLogWidget(), tr("Changelog"));
    tabs->addTab(modulesWidget(), tr("Modules"));

    if (gtApp->devMode())
    {
        tabs->addTab(sharedFunctionsWidget(), tr("Shared functions"));
        addDevelopmentTabs(tabs);
    }

    mainL->addWidget(tabs);
    setLayout(mainL);

    tabs->setCurrentIndex(startPage);

    setMinimumSize(575, 470);
    setMaximumSize(575, 470);
}

void
GtAboutDialog::exportFootprint()
{
    QString initFileName = QDateTime::currentDateTime().toString("yyyy-MM-dd") +
                           QStringLiteral("_gtlab_env_footprint.xml");

    QString filename = GtFileDialog::getSaveFileName(this,
                       tr("Choose File"),
                       QString(), tr("XML files (*.xml)"),
                       initFileName);

    if (filename.isEmpty())
    {
        // abort has been pressed
        return;
    }

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open file!");
        return;
    }

    QTextStream stream(&file);
    stream << GtFootprint().exportToXML();

    file.close();
}

QWidget*
GtAboutDialog::coreAbout() const
{
    auto* w = new QWidget;

    auto* layout = new QVBoxLayout;

    auto anim = new GtSplashScreen;
    anim->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding));
    layout->addWidget(anim);

    auto* aboutHLay = new QHBoxLayout;

    auto* copyLabel =
            new QLabel(QStringLiteral("GTlab - Gas Turbine laboratory\n"
                                      "copyright 2023 by DLR\n"));

    auto* versLabel =
            new QLabel(gtApp->version().toString() +
                       QStringLiteral("\nHelium"));
    versLabel->setAlignment(Qt::AlignRight);

    aboutHLay->addWidget(copyLabel);
    aboutHLay->addWidget(versLabel);

    auto* iconLabel = new QPushButton(tr("License information"));
    iconLabel->setFlat(true);
    iconLabel->setContentsMargins(0, 0, 0, 0);
    QFont f = iconLabel->font();
    f.setUnderline(true);
    iconLabel->setFont(f);
    iconLabel->setStyleSheet("QPushButton {color : blue; border:none;}");

    connect(iconLabel, SIGNAL(clicked()), SLOT(openLicenseFolder()));

    layout->addLayout(aboutHLay);
    layout->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::Minimum,
                                          QSizePolicy::Minimum));

    auto* btnLayout = new QHBoxLayout;

    auto* exportFootprintBtn =
            new QPushButton(tr("Export Framework Footprint"));
    exportFootprintBtn->setIcon(gt::gui::icon::export_());
    exportFootprintBtn->setFocusPolicy(Qt::NoFocus);
    btnLayout->addWidget(iconLabel);
    btnLayout->addStretch();
    btnLayout->addWidget(exportFootprintBtn);

    layout->addLayout(btnLayout);

    layout->setSpacing(5);

    w->setLayout(layout);

    connect(exportFootprintBtn, SIGNAL(clicked()), SLOT(exportFootprint()));

    return w;
}

QWidget*
GtAboutDialog::modulesWidget()
{
    m_modulesTree = new QTreeWidget;

    auto* w = new QWidget;
    auto* l = new QVBoxLayout;

    w->setLayout(l);

    m_modulesTree->setColumnCount(2);
    m_modulesTree->setHeaderLabels(QStringList()
                                         << tr("Name")
                                         << tr("Version"));
    m_modulesTree->header()->setStretchLastSection(true);
    m_modulesTree->setColumnWidth(0, 300);
    foreach (const QString& id, gtApp->moduleIds())
    {
        QStringList info;
        info << id;
        info << gtApp->moduleVersion(id).toString();
        auto* item = new QTreeWidgetItem(info);
        item->setIcon(0, gt::gui::icon::plugin());
        item->setToolTip(0, gtApp->moduleDescription(id));
        m_modulesTree->addTopLevelItem(item);
    }

    auto* btnLayout = new QHBoxLayout;

    m_detailsButton = new QPushButton(tr("Details"));
    m_detailsButton->setIcon(gt::gui::icon::info2());
    m_detailsButton->setDisabled(true);

    connect(m_detailsButton, SIGNAL(clicked()),
            this, SLOT(onDetailsButtonClicked()));

    l->addWidget(m_modulesTree);
    btnLayout->addWidget(m_detailsButton);

    btnLayout->addSpacerItem(new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                             QSizePolicy::Minimum));

    l->addLayout(btnLayout);
    connect(m_modulesTree, SIGNAL(clicked(QModelIndex)),
            this, SLOT(onModuleClicked(QModelIndex)));

    connect(m_modulesTree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(openModuleDetails(QModelIndex)));

    return w;
}

QWidget*
GtAboutDialog::changeLogWidget() const
{
    auto* w = new QWidget;
    auto* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* textEdit = new GtChangeLogWidget(readChangeLog());

    layout->addWidget(textEdit);

    w->setLayout(layout);
    return w;
}

QWidget*
GtAboutDialog::sharedFunctionsWidget()
{
    return new GtSharedFunctionsWidget();
}

QString
GtAboutDialog::readChangeLog() const
{
    return readApplicationDirFile("CHANGELOG.md");
}

void
GtAboutDialog::addDevelopmentTabs(QTabWidget* tabs)
{
    auto entry = [this, tabs](QStringList const& entries, QString const& title)
    {
        auto* page = new QWidget(this);
        tabs->addTab(page, title);
        auto* layout = new QVBoxLayout;
        auto* listWidget = new QListWidget;
        listWidget->setFrameStyle(QListWidget::NoFrame);
        listWidget->insertItems(0, entries);
        listWidget->sortItems();
        layout->addWidget(listWidget);
        page->setLayout(layout);
    };

    // datamodel
    entry(gtObjectFactory->knownClasses(), tr("Datamodel"));

    // calculators
    entry(gtProcessFactory->calculatorFactory()->knownClasses(),
          tr("Calculators"));

    // tasks
    QStringList itemNames;
    for (const GtTaskData& taskData : gtTaskFactory->taskDataList())
    {
        itemNames.append(taskData->metaData().className());
    }
    entry(itemNames, tr("Tasks"));

    // mdi items
    entry(gtMdiLauncher->knownClasses(), tr("MDI Items"));

    // ui objects
    entry(gtApp->knownUIObjects(), tr("UI Objects"));

    // dock widgets
    entry(gtMdiLauncher->dockWidgetIds(), tr("Dock Widgets"));
}

QString
GtAboutDialog::readApplicationDirFile(const QString& fileName) const
{
    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();

    QFile file(appDir.absoluteFilePath(fileName));

    if (!file.exists())
    {
        gtDebug() << tr("file '%1' not exists!").arg(file.fileName());
        return {};
    }

    if (!file.open(QFile::ReadOnly))
    {
        gtDebug() << tr("could not open file '%1'!").arg(file.fileName());
        return {};
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

void
GtAboutDialog::onModuleClicked(const QModelIndex& index)
{
    m_detailsButton->setEnabled(index.isValid());
}

void
GtAboutDialog::onDetailsButtonClicked() const
{
    QModelIndex i = m_modulesTree->currentIndex();

    if (i.isValid())
    {
        openModuleDetails(i);
    }
}

void
GtAboutDialog::openModuleDetails(const QModelIndex& indexOfModule) const
{
    if (indexOfModule.isValid())
    {
        /// take the id from the element of the first column
        /// in the selected row
        QModelIndex effIndex = indexOfModule.siblingAtColumn(0);
        GtModuleDetailsDialog d(effIndex.data().toString());

        d.exec();
    }
}

void
GtAboutDialog::openLicenseFolder()
{
    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();

    QDir appLicenseDir(appDir.absolutePath() + QDir::separator() + "Licenses");

    if (appLicenseDir.exists())
    {
        QDesktopServices::openUrl(appLicenseDir.absolutePath());
        return;
    }
    /// Search for Licenses in the Dev-Tools-Structure
    appDir.cdUp(); /// folder for selection of number of stable/unstable version
    appDir.cdUp(); /// main folder of devtools

    QDir devToolsLicenseDir(appDir.absolutePath()
                            + QDir::separator() + "Licenses");

    if (devToolsLicenseDir.exists())
    {
        QDesktopServices::openUrl(devToolsLicenseDir.absolutePath());
        return;
    }

    gtError() << tr("No licenses information could be found.");
    gtError() << tr("Your installation might be broken.");
    gtError() << tr("Please contact the developers for further information.");
}
