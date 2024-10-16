// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
//
// SPDX-License-Identifier: MPL-2.0+

/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QWidget>
#include <QFrame>
#include <QDir>
#include <QScrollArea>
#include <QFileDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QApplication>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "gt_settings.h"
#include "gt_application.h"
#include "gt_projectui.h"
#include "gt_examplesentry.h"
#include "gt_examplegraphicalitem.h"
#include "gt_examplestabwidget.h"
#include "gt_datamodel.h"
#include "gt_projectprovider.h"
#include "gt_examplesmdiwidget.h"

#include "gt_project.h"
#include "gt_projectspecwidget.h"
#include "gt_dialog.h"
#include "gt_icons.h"
#include "gt_stylesheets.h"
#include "gt_filesystem.h"

#include <gt_logging.h>

GtExamplesMdiWidget::GtExamplesMdiWidget() :
    m_tabWidget(nullptr)
{
    setObjectName(tr("Examples"));

    initializeExamplesPath();

    initializeDirectories();

    initializeWidget();
}


void
GtExamplesMdiWidget::initializeDirectories()
{
    QDir mainDir(m_examplesPath);

    if (m_examplesPath.isEmpty() || !mainDir.exists())
    {
        gtWarning() << tr("No Examples found!");
        return;
    }

    QStringList allDirs = mainDir.entryList(QDir::AllDirs);

    allDirs.removeOne(".");
    allDirs.removeOne("..");

    if (allDirs.isEmpty())
    {
        gtWarning() << tr("No Examples found!");
        return;
    }

    gtInfo() << tr("Examples found:") << allDirs.join(", ");

    foreach (QString dir, allDirs)
    {
        QDir curDir(mainDir.absolutePath() + "/" + dir);

        if (!validateExampleDiretory(curDir))
        {
            gtWarning() << tr("Invalid Example:")
                        << mainDir.absolutePath() + "/" + dir;
            continue;
        }

        GtExamplesEntry entry;

        if (readDirectoryContentToExampleEntry(curDir, entry))
        {
            m_examplesEntries.append(entry);
        }

        else
        {
            gtWarning() << tr("Reading directory failed:")
                        << mainDir.absolutePath() + "/" + dir;
        }
    }
}

void
GtExamplesMdiWidget::setBasicLayoutToTabPage(QScrollArea* tabPage,
                                             const QString& category)
{
    if (!tabPage) return;

    tabPage->setObjectName(category);

    tabPage->setWidgetResizable(true);

    auto* frame = new QFrame();

    frame->setAutoFillBackground(true);
    frame->setObjectName(QStringLiteral("frame"));
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setStyleSheet(gt::gui::stylesheet::backgroundFrame());

    auto* grid = new QGridLayout;
    grid->setObjectName(QStringLiteral("grid"));

    frame->setLayout(grid);

    tabPage->setWidget(frame);
}

void
GtExamplesMdiWidget::initializeWidget()
{
    QWidget* mainWidget = widget();

    auto* frame = qobject_cast<QFrame*>(widget());

    if (frame)
    {
        frame->setFrameStyle(QFrame::NoFrame);
    }

    if (!mainWidget)
    {
        return;
    }

    auto* lay1 = new QVBoxLayout;

    mainWidget->setLayout(lay1);

    m_tabWidget = new GtExamplesTabWidget();

    lay1->addWidget(m_tabWidget);

    QStringList categories = getAllCategories();

    connect(m_tabWidget, SIGNAL(resized()), this, SLOT(onResized()));

    foreach (QString cat, categories)
    {
        auto* tabpage = new QScrollArea(m_tabWidget);

        setBasicLayoutToTabPage(tabpage, cat);

        m_tabWidget->addTab(tabpage, cat);

        m_tabs.insert(cat, tabpage);
    }

    foreach (const auto& entry, m_examplesEntries)
    {
        auto* item = new GtExampleGraphicalItem(entry);

        connect(item, SIGNAL(openProject(QString)),
                this, SLOT(onOpenProject(QString)));

        m_graphicalItems.append(item);
    }

    sortItems();
}

void
GtExamplesMdiWidget::sortItems()
{
    QStringList categories = getAllCategories();

    QMap<QString, int> rowMap;
    QMap<QString, int> columnMap;

    foreach (QString cat, categories)
    {
        rowMap.insert(cat, 0);
        columnMap.insert(cat, 0);
    }

    int columnsPerRow = widget()->width() / 290;

    if (columnsPerRow < 1)
    {
        columnsPerRow = 1;
    }

    foreach (auto* item, m_graphicalItems)
    {

        QString category = item->m_data.category();

        QWidget* page = getCurrentPage(category);

        auto* lay = page->findChild<QGridLayout*>("grid");

        if (!lay)
        {
            gtDebug() << tr("Layout not found");
            continue;
        }

        if (columnMap.value(category) == columnsPerRow)
        {
            rowMap[category]++;
            columnMap[category] = 0;
        }

        lay->addWidget(item, rowMap.value(category),
                       columnMap.value(category), 1, 1,
                       Qt::AlignTop);

        lay->setVerticalSpacing(30);

        columnMap[category]++;
    }
}

void
GtExamplesMdiWidget::onResized()
{
    sortItems();
}

void
GtExamplesMdiWidget::onOpenProject(const QString& exampleName)
{
    QString dirPath = exampleName.split("#").first();

    GtDialog dialog;

    QString newDirPath;

    QString id = gt::makeUniqueName(exampleName.split("#").last(),
                 gtApp->session());

    dialog.setWindowTitle(tr("Save Example Project As..."));
    dialog.setWindowIcon(gt::gui::icon::save());

    auto* layout = new QVBoxLayout;
    auto* infoLabel = new QLabel;
    layout->addWidget(infoLabel);
    infoLabel->setText(tr("The project you are about to open is located in a "
                          "write-protected location.\nPlease select a folder "
                          "to open a modifable copy of the project."));

    auto* pspecs = new GtProjectSpecWidget;
    layout->addWidget(pspecs);

    QDir projectDir = gtApp->settings()->lastPath();

    pspecs->setProjectInformation(id, projectDir.absolutePath());

    layout->addStretch(1);

    auto* btnLay = new QHBoxLayout;

    auto* acceptBtn = new QPushButton(tr("Save"));
    acceptBtn->setIcon(gt::gui::icon::save());

    connect(acceptBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    auto* cancleBtn = new QPushButton(tr("Cancel"));
    cancleBtn->setIcon(gt::gui::icon::cancel());

    connect(cancleBtn, SIGNAL(clicked(bool)), &dialog, SLOT(reject()));

    btnLay->addStretch(1);
    btnLay->addWidget(acceptBtn);
    btnLay->addWidget(cancleBtn);

    layout->addLayout(btnLay);

    dialog.setLayout(layout);

    dialog.resize(400, 170);

    QString projectNameNew;

    if (dialog.exec())
    {
        newDirPath = pspecs->pathLine()->text();
        projectNameNew = pspecs->nameLine()->text();
    }
    else
    {
        return;
    }

    ////////////////////////////////////////////////////

    QDir oldProjectDir(dirPath + QDir::separator() + "project");

    if (!oldProjectDir.exists())
    {
        gtWarning() << tr("Directory of Example not found");
        return;
    }

    QDir newProjectDir(newDirPath);

    QDir parentDir = newProjectDir;
    parentDir.cdUp();

    if (!parentDir.mkdir(newProjectDir.dirName()))
    {
        gtError() << tr("Could not create new folder")
                  << newProjectDir.dirName()
                  << tr("in the directory") << parentDir.absolutePath();
        return;
    }

    gt::filesystem::CopyStatus res =
            gt::filesystem::copyDir(oldProjectDir, newProjectDir);

    if (res != gt::filesystem::CopyStatus::Success)
    {
        gtError() << tr("Copy the example to a new project failed");
        return;
    }

    QString projectFile = newProjectDir.absoluteFilePath("project.gtlab");

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    project->setObjectName(projectNameNew);

    // search for project within session
    if (gtApp->session()->findProject(project->objectName()))
    {
        gtInfo() << tr("There is already a project with name of the example.");
        project->setObjectName(gt::makeUniqueName(
                               project->objectName(), gtApp->session()));
    }

    if (!gtDataModel->newProject(project, true))
    {
        delete project;
        gtError() << tr("Invalid example project");
        return;
    }

    if (!GtProjectUI::saveAndCloseCurrentProject()) return;

    if (!gtDataModel->openProject(project))
    {
        gtDebug() << "Opening the example project failed";
    }
}

QWidget*
GtExamplesMdiWidget::getCurrentPage(const QString& category) const
{
    return m_tabs.value(category);
}

QStringList
GtExamplesMdiWidget::getAllCategories()
{
    QStringList retVal;

    foreach (const auto& entry, m_examplesEntries)
    {
        retVal.append(entry.category());
    }

    retVal.removeDuplicates();

    return retVal;
}

QIcon
GtExamplesMdiWidget::icon() const
{
    return gt::gui::icon::examples();
}

bool
GtExamplesMdiWidget::allowsMultipleInstances() const
{
    return false;
}

void
GtExamplesMdiWidget::showEvent()
{
    sortItems();
}

void
GtExamplesMdiWidget::initializeExamplesPath()
{
    QDir appDir(qApp->applicationDirPath());

    if (!appDir.cdUp())
    {
        return;
    }

    if (!appDir.cd(QStringLiteral("examples")))
    {
        return;
    }

    m_examplesPath = appDir.absolutePath();
}

bool
GtExamplesMdiWidget::validateExampleDiretory(const QDir &dir) const
{

    if (!dir.exists())
    {
        gtDebug() << tr("Dir not found");
        return false;
    }

    QFile indexFile(dir.absoluteFilePath("index.json"));

    if (!indexFile.exists())
    {
        gtWarning() << tr("IndexFile not found");
        return false;
    }

    QDir projectDir(dir.absolutePath() + QDir::separator() + "project");

    if (!projectDir.exists())
    {
        gtWarning() << tr("Project Directory 'project' is missing in example "
                          "directory");
        return false;
    }

    QFile projectFile(projectDir.absoluteFilePath("project.gtlab"));

    if (!projectFile.exists())
    {
        gtWarning() << tr("The file 'project.gtlab' is missing in the "
                          "'project' directory");
        return false;
    }

    return true;
}

bool
GtExamplesMdiWidget::readDirectoryContentToExampleEntry(const QDir& dir,
                                                        GtExamplesEntry& entry)
{
    QFile indexFile(dir.absoluteFilePath("index.json"));

    entry.setDirPath(dir.absolutePath());

    if (!indexFile.exists())
    {
        gtWarning() << tr("IndexFile not found");
        return false;
    }

    if (!indexFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        gtWarning() << tr("Cannot open IndexFile");
        return false;
    }

    QJsonDocument jsonResponse = QJsonDocument::fromJson(indexFile.readAll());

    indexFile.close();

    QJsonObject obj = jsonResponse.object();

    if (!obj.contains("Name"))
    {
        gtWarning() << tr("invalid Jason-File: Entry 'Name' is missing");

        return false;
    }

    if (!obj.contains("Category"))
    {
        gtWarning() << tr("invalid Jason-File: Entry 'Category' is missing");
        return false;
    }

    if (obj.contains("Description"))
    {
        entry.setDescription(obj["Description"].toString());
    }

    entry.setName(obj["Name"].toString());
    entry.setCategory(obj["Category"].toString());

    QString pixmapFilePath = dir.absoluteFilePath("picture.png");

    entry.setPixmapPath(pixmapFilePath);

    return true;
}
