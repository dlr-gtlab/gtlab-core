/* GTlab - Gas Turbine laboratory
 * Source File: gt_aboutdialog.cpp
 * copyright 2009-2016 by DLR
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


GtAboutDialog::GtAboutDialog(int startPage, QWidget* parent) : GtDialog(parent)
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
        tabs->addTab(sharedFunctionsWidget(),
                     tr("Shared functions"));
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
                                      "copyright 2022 by DLR\n"));

    auto* versLabel =
            new QLabel(gtApp->version().toString() +
                       QStringLiteral("\nHelium"));
    versLabel->setAlignment(Qt::AlignRight);

    aboutHLay->addWidget(copyLabel);
    aboutHLay->addWidget(versLabel);

    auto* iconLabel =
            new QLabel(QStringLiteral("The icon set was created by "
                                      "<a href=\"https://pictogrammers.com/\">"
                                      "Pictogrammers</a>"));
    iconLabel->setTextFormat(Qt::RichText);
    iconLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    iconLabel->setOpenExternalLinks(true);

    layout->addLayout(aboutHLay);
    layout->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::Minimum,
                                          QSizePolicy::Minimum));

    auto* btnLayout = new QHBoxLayout;

    auto exportFootprintBtn = new QPushButton(tr("Export Framework Footprint"));
    exportFootprintBtn->setIcon(gt::gui::icon::export16());
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
        item->setIcon(0, gt::gui::icon::plugin16());
        item->setToolTip(0, gtApp->moduleDescription(id));
        m_modulesTree->addTopLevelItem(item);
    }

    auto* btnLayout = new QHBoxLayout;

    m_detailsButton = new QPushButton(tr("Details"));
    m_detailsButton->setIcon(gt::gui::icon::info16());
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

    QString text = readChangeLog();

    auto* textEdit = new GtChangeLogWidget(text);

    layout->addWidget(textEdit);

    w->setLayout(layout);
    return w;
}

QWidget*
GtAboutDialog::sharedFunctionsWidget()
{
    auto* dw = new GtSharedFunctionsWidget();

    return dw;
}

QString
GtAboutDialog::readChangeLog() const
{
    QDir appDir(QApplication::applicationDirPath());
    appDir.cdUp();

    QFile file(appDir.absoluteFilePath("CHANGELOG.md"));

    if (!file.exists())
    {
        gtDebug() << tr("file not exists!");
        return {};
    }

    if (!file.open(QFile::ReadOnly))
    {
        gtDebug() << tr("could not open file!");
        return {};
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    return content;
}

void
GtAboutDialog::addDevelopmentTabs(QTabWidget *tabs)
{
    // datamodel
    auto* datamodelPage = new QWidget(this);
    tabs->addTab(datamodelPage, tr("Datamodel"));
    auto* datamodelLayout = new QVBoxLayout;
    auto* datamodelClassList = new QListWidget;
    datamodelClassList->setFrameStyle(QListWidget::NoFrame);
    datamodelClassList->insertItems(0, gtObjectFactory->knownClasses());
    datamodelClassList->sortItems();
    datamodelLayout->addWidget(datamodelClassList);
    datamodelPage->setLayout(datamodelLayout);

    // calculators
    auto* calculatorsPage = new QWidget(this);
    tabs->addTab(calculatorsPage, tr("Calculators"));
    auto* calculatorLayout = new QVBoxLayout;
    auto* calculatorClassList = new QListWidget;
    calculatorClassList->setFrameStyle(QListWidget::NoFrame);
    calculatorClassList->insertItems(
                0, gtProcessFactory->calculatorFactory()->knownClasses());
    calculatorClassList->sortItems();
    calculatorLayout->addWidget(calculatorClassList);
    calculatorsPage->setLayout(calculatorLayout);

    // tasks
    auto* tasksPage = new QWidget(this);
    tabs->addTab(tasksPage, tr("Tasks"));
    auto* taskLayout = new QVBoxLayout;
    auto* taskClassList = new QListWidget;
    taskClassList->setFrameStyle(QListWidget::NoFrame);
    QStringList itemNames;
    for (const GtTaskData& taskData : gtTaskFactory->taskDataList())
    {
        itemNames.append(taskData->metaData().className());
    }

    taskClassList->insertItems(0, itemNames);
    taskClassList->sortItems();
    taskLayout->addWidget(taskClassList);
    tasksPage->setLayout(taskLayout);

    // mdi items
    auto* mdiPage = new QWidget(this);
    tabs->addTab(mdiPage, tr("MDI Items"));
    auto* mdiLayout = new QVBoxLayout;
    mdiPage->setLayout(mdiLayout);
    auto* mdiitemsClassList = new QListWidget;
    mdiitemsClassList->setFrameStyle(QListWidget::NoFrame);
    mdiLayout->addWidget(mdiitemsClassList);
    mdiitemsClassList->insertItems(0, gtMdiLauncher->knownClasses());
    mdiitemsClassList->sortItems();

    // ui objects
    auto* uiPage = new QWidget(this);
    tabs->addTab(uiPage, tr("UI Objects"));
    auto* uiLayout = new QVBoxLayout;
    uiPage->setLayout(uiLayout);
    auto* uiitemsClassList = new QListWidget;
    uiitemsClassList->setFrameStyle(QListWidget::NoFrame);
    uiLayout->addWidget(uiitemsClassList);
    uiitemsClassList->insertItems(0, gtApp->knownUIObjects());
    uiitemsClassList->sortItems();

    // dock widgets
    auto* dockPage = new QWidget(this);
    tabs->addTab(dockPage, tr("Dock Widgets"));
    auto* dockLayout = new QVBoxLayout;
    dockPage->setLayout(dockLayout);
    auto* dockClassList = new QListWidget;
    dockClassList->setFrameStyle(QListWidget::NoFrame);
    dockLayout->addWidget(dockClassList);
    dockClassList->insertItems(0, gtMdiLauncher->dockWidgetIds());
    dockClassList->sortItems();
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
