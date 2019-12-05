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

#include "gt_application.h"
#include "gt_examplesentry.h"
#include "gt_examplegraphicalitem.h"
#include "gt_examplestabwidget.h"
#include "gt_datamodel.h"
#include "gt_projectprovider.h"
#include "gt_project.h"
#include "gt_projectspecwidget.h"
#include "gt_saveprojectmessagebox.h"

#include "gt_examplesmdiwidget.h"

GtExamplesMdiWidget::GtExamplesMdiWidget() :
    m_tabWidget(Q_NULLPTR)
{
    setObjectName(tr("Examples"));

    initializeExamplesPath();

    initializeDirectories();

    initializeWidget();
}

GtExamplesMdiWidget::~GtExamplesMdiWidget()
{
    qDeleteAll(m_examplesEntries);
}

void
GtExamplesMdiWidget::initializeDirectories()
{
    QDir mainDir(m_examplesPath);

    if (!mainDir.exists())
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

    gtDebug() << "AllDirs:" << allDirs;

    foreach (QString dir, allDirs)
    {
        QDir curDir(mainDir.absolutePath() + "/" + dir);

        if (!validateExampleDiretory(&curDir))
        {
            gtWarning() << tr("Invalid Example:")
                        << mainDir.absolutePath() + "/" + dir;
            continue;
        }

        GtExamplesEntry* entry = new GtExamplesEntry;

        if (readDirectoryContentToExampleEntry(&curDir, entry))
        {
            m_examplesEntries.append(entry);
        }

        else
        {
            gtWarning() << tr("Reading directory failed:")
                        << mainDir.absolutePath() + "/" + dir;
            delete entry;
        }
    }
}

void
GtExamplesMdiWidget::setBasicLayoutToTabPage(QScrollArea* tabPage,
                                             QString category)
{
    if (!tabPage)
    {
        return;
    }

    tabPage->setObjectName(category);

    tabPage->setWidgetResizable(true);

    QFrame* frame = new QFrame();

    frame->setAutoFillBackground(true);
    frame->setObjectName(QStringLiteral("frame"));
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setStyleSheet("#frame {border-image:"
                         "url(:/pixmaps/startup-background.png)}");

    QGridLayout* grid = new QGridLayout;
    grid->setObjectName("grid");

    frame->setLayout(grid);

    tabPage->setWidget(frame);
}

void
GtExamplesMdiWidget::initializeWidget()
{
    QWidget* mainWidget = widget();

    QFrame* frame = qobject_cast<QFrame*>(widget());

    if (frame != Q_NULLPTR)
    {
        frame->setFrameStyle(QFrame::NoFrame);
    }

    if (mainWidget == Q_NULLPTR)
    {
        return;
    }

    QVBoxLayout* lay1 = new QVBoxLayout;

    mainWidget->setLayout(lay1);

    m_tabWidget = new GtExamplesTabWidget();

    lay1->addWidget(m_tabWidget);

    QStringList categories = getAllCategories();

    //gtDebug() << "Categories:" << categories;

    connect(m_tabWidget, SIGNAL(resized()), this, SLOT(onResized()));

    foreach (QString cat, categories)
    {
        QScrollArea* tabpage = new QScrollArea(m_tabWidget);

        setBasicLayoutToTabPage(tabpage, cat);

        m_tabWidget->addTab(tabpage, cat);

        m_tabs.insert(cat, tabpage);
    }

    foreach (GtExamplesEntry* entry, m_examplesEntries)
    {
        GtExampleGraphicalItem* item = new
        GtExampleGraphicalItem(entry);

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

    foreach (GtExampleGraphicalItem* item, m_graphicalItems)
    {
        if (item->m_data == Q_NULLPTR)
        {
            continue;
        }

        QString category = item->m_data->category();

        QWidget* page = getCurrentPage(category);

        QGridLayout* lay = page->findChild<QGridLayout*>("grid");

        if (lay == Q_NULLPTR)
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
GtExamplesMdiWidget::onOpenProject(QString exampleName)
{
    gtDebug() << "On Open Project";
    QString dirPath = exampleName.split("#").first();

    QDialog dialog;

    QString newDirPath;

    QString id = gtDataModel->uniqueObjectName(exampleName.split("#").last(),
                 gtApp->session());

    dialog.setWindowTitle(tr("Save Example Project As..."));
    dialog.setWindowIcon(gtApp->icon(QStringLiteral("saveProjectIcon_16.png")));

    Qt::WindowFlags flags = dialog.windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    dialog.setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;
    QLabel* infoLabel = new QLabel;
    layout->addWidget(infoLabel);
    infoLabel->setText(tr("The project you are about to open is located in a "
                          "write-protected location.\nPlease select a folder "
                          "to open a modifable copy of the project."));

    GtProjectSpecWidget* pspecs = new GtProjectSpecWidget;
    layout->addWidget(pspecs);

    QDir projectDir = QDir::home();

    pspecs->setProjectInformation(id, projectDir.absolutePath());

    layout->addStretch(1);

    QHBoxLayout* btnLay = new QHBoxLayout;

    QPushButton* acceptBtn = new QPushButton(tr("Save"));
    acceptBtn->setIcon(gtApp->icon(QStringLiteral("saveProjectIcon_16.png")));

    connect(acceptBtn, SIGNAL(clicked(bool)), &dialog, SLOT(accept()));

    QPushButton* cancleBtn = new QPushButton(tr("Cancel"));
    cancleBtn->setIcon(gtApp->icon(QStringLiteral("closeIcon_16.png")));

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
        newDirPath = pspecs->pathLine()->text(); //dialog.selectedFiles().first();
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

    //QString exampleName2 = projectNameNew; // exampleName.split("#").last();

    if (!parentDir.mkdir(newProjectDir.dirName()))
    {
        gtError() << tr("Could not create new folder")
                  << newProjectDir.dirName()
                  << tr("in the directory") << parentDir.absolutePath();
        return;
    }

    //if (!newProjectDir.cd(exampleName2))
    //{
    //    gtWarning() << tr("New Directory not found:") << exampleName2;
    //    return;
    //}

    QStringList oldDirContent = oldProjectDir.entryList(QDir::NoFilter);
    oldDirContent.removeOne(".");
    oldDirContent.removeOne("..");

    //gtDebug() << "oldDirContent" << oldDirContent;

    foreach (QString path, oldDirContent)
    {
        QFile file(oldProjectDir.absoluteFilePath(path));

        QDir dir(oldProjectDir.absolutePath() +  QDir::separator() + path);

        if (file.exists())
        {
            file.copy(newProjectDir.absoluteFilePath(path));
        }

        if (dir.exists())
        {
            newProjectDir.mkdir(path);

            QDir newSubDir(newProjectDir.absolutePath() + QDir::separator()
                           + path);

            QStringList oldSubDirContent = dir.entryList(QDir::NoFilter);
            oldSubDirContent.removeOne(".");
            oldSubDirContent.removeOne("..");

            foreach (QString subPath, oldSubDirContent)
            {
                QFile file2(dir.absoluteFilePath(subPath));

                if (file2.exists())
                {
                    file2.copy(newSubDir.absoluteFilePath(subPath));
                }
                else
                {
                    gtWarning() << tr("Examples supports only two levels of"
                                      "directories");
                }
            }
        }
    }

    QString projectFile = newProjectDir.absoluteFilePath("project.gtlab");

    GtProjectProvider provider(projectFile);
    GtProject* project = provider.project();

    project->setObjectName(projectNameNew);

    // search for project within session
    if (gtApp->session()->findProject(project->objectName()))
    {
        gtInfo() << tr("There is already a project with name of the example.");
        project->setObjectName(gtDataModel->uniqueObjectName(
                               project->objectName(), gtApp->session()));
    }

    if (!gtDataModel->newProject(project))
    {
        delete project;
    }

    if (gtApp->hasProjectChanges())
    {
        QString text = tr("Found changes in the opened project.\n"
                          "Do you want to save all your changes "
                          "before opening the example project?");

        GtSaveProjectMessageBox mb(text);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                gtDataModel->saveProject(gtApp->currentProject());
                break;
            }

            case QMessageBox::No:
            {
                break;
            }

            case QMessageBox::Cancel:
            {
                return;
                break;
            }

            default:
                break;
        }
    }

    gtDataModel->closeProject(gtApp->currentProject());

    if (!gtDataModel->openProject(project))
    {
        gtDebug() << "Opening the example project failed";
    }
}

QWidget*
GtExamplesMdiWidget::getCurrentPage(QString category)
{
    return m_tabs.value(category);
}

QStringList
GtExamplesMdiWidget::getAllCategories()
{
    QStringList retVal;

    foreach (GtExamplesEntry* entry, m_examplesEntries)
    {
        retVal.append(entry->category());
    }

    retVal.removeDuplicates();

    return retVal;
}

QIcon
GtExamplesMdiWidget::icon() const
{
    return gtApp->icon(QStringLiteral("examplesIcon_16.png"));
}

bool
GtExamplesMdiWidget::allowsMultipleInstances()
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
GtExamplesMdiWidget::validateExampleDiretory(QDir* dir)
{
    if (dir == Q_NULLPTR)
    {
        gtDebug() << tr("Dir is NullPtr");
        return false;
    }

    if (!dir->exists())
    {
        gtDebug() << tr("Dir not found");
        return false;
    }

    QFile indexFile(dir->absoluteFilePath("index.json"));

    if (!indexFile.exists())
    {
        gtWarning() << tr("IndexFile not found");
        return false;
    }

    QDir projectDir(dir->absolutePath() + QDir::separator() + "project");

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
GtExamplesMdiWidget::readDirectoryContentToExampleEntry(QDir* dir,
                                                        GtExamplesEntry* entry)
{
    if (!dir)
    {
        gtDebug() << tr("Dir is Nullptr");
        return false;
    }

    if (!entry)
    {
        gtDebug() << tr("Entry is Nullptr");
        return false;
    }

    QFile indexFile(dir->absoluteFilePath("index.json"));

    entry->setDirPath(dir->absolutePath());

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
        entry->setDescription(obj["Description"].toString());
    }

    entry->setName(obj["Name"].toString());
    //gtDebug() << "Name:" << obj["Name"].toString();
    entry->setCategory(obj["Category"].toString());
    //gtDebug() << "Category:" << obj["Category"].toString();

    QString pixmapFilePath = dir->absoluteFilePath("picture.png");

    entry->setPixmapPath(pixmapFilePath);

    return true;
}
