/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTabWidget>
#include <QLayout>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QHeaderView>

#include "gt_objectfactory.h"
#include "gt_processfactory.h"
#include "gt_factorygroup.h"
#include "gt_mdilauncher.h"
#include "gt_application.h"
#include "gt_versionnumber.h"
#include "gt_icons.h"

#include "gt_preferencesplugins.h"

GtPreferencesPlugins::GtPreferencesPlugins()
{
    setTitle(tr("Plugins"));

//    QTabWidget* tabWidget = new QTabWidget;

    // plugins
    QTreeWidget* datamodelPluginList = new QTreeWidget;
    datamodelPluginList->setColumnCount(2);
//    datamodelPluginList->setMaximumHeight(200);
    datamodelPluginList->setHeaderLabels(QStringList()
                                         << tr("Name")
                                         << tr("Version"));
    datamodelPluginList->header()->setStretchLastSection(true);
    datamodelPluginList->setColumnWidth(0, 300);
    foreach (const QString& id, gtApp->moduleIds())
    {
        QStringList nfo;
        nfo << id;
        nfo << gtApp->moduleVersion(id).toString();
        QTreeWidgetItem* item = new QTreeWidgetItem(nfo);
        item->setIcon(0, GtGUI::icon("pluginIcon_16.png"));
        item->setToolTip(0,gtApp->moduleDescription(id));
        datamodelPluginList->addTopLevelItem(item);
    }
    layout()->addWidget(datamodelPluginList);

//    // datamodel
//    QWidget* datamodelPage = new QWidget(this);
//    tabWidget->addTab(datamodelPage, tr("Datamodel"));
//    QVBoxLayout* datamodelLayout = new QVBoxLayout;
////    QLabel* datamodelClassLabel = new QLabel(tr("Classes:"));
//    QListWidget* datamodelClassList = new QListWidget;
//    datamodelClassList->setFrameStyle(QListWidget::NoFrame);
//    datamodelClassList->insertItems(0, gtObjectFactory->knownClasses());
////    datamodelLayout->addWidget(datamodelClassLabel);
//    datamodelLayout->addWidget(datamodelClassList);
//    datamodelPage->setLayout(datamodelLayout);

//    // calculators
//    QWidget* calculatorsPage = new QWidget(this);
//    tabWidget->addTab(calculatorsPage, tr("Calculators"));
//    QVBoxLayout* calculatorLayout = new QVBoxLayout;
//    QListWidget* calculatorClassList = new QListWidget;
//    calculatorClassList->setFrameStyle(QListWidget::NoFrame);
//    calculatorClassList->insertItems(0, gtProcessFactory->knownClasses());
//    calculatorLayout->addWidget(calculatorClassList);
//    calculatorsPage->setLayout(calculatorLayout);

//    // core
////    QWidget* corePage = new QWidget(this);
////    tabWidget->addTab(corePage, tr("Core"));

//    // mdi items
//    QWidget* mdiPage = new QWidget(this);
//    tabWidget->addTab(mdiPage, tr("MDI Items"));
//    QVBoxLayout* mdiLayout = new QVBoxLayout;
//    mdiPage->setLayout(mdiLayout);
//    QListWidget* mdiitemsClassList = new QListWidget;
//    mdiitemsClassList->setFrameStyle(QListWidget::NoFrame);
//    mdiLayout->addWidget(mdiitemsClassList);
//    mdiitemsClassList->insertItems(0, gtMdiLauncher->knownClasses());

//    // ui objects
//    QWidget* uiPage = new QWidget(this);
//    tabWidget->addTab(uiPage, tr("UI Objects"));
//    QVBoxLayout* uiLayout = new QVBoxLayout;
//    uiPage->setLayout(uiLayout);
//    QListWidget* uiitemsClassList = new QListWidget;
//    uiitemsClassList->setFrameStyle(QListWidget::NoFrame);
//    uiLayout->addWidget(uiitemsClassList);
//    uiitemsClassList->insertItems(0, gtApp->knownUIObjects());

//    // dock widgets
//    QWidget* dockPage = new QWidget(this);
//    tabWidget->addTab(dockPage, tr("Dock Widgets"));
//    QVBoxLayout* dockLayout = new QVBoxLayout;
//    dockPage->setLayout(dockLayout);
//    QListWidget* dockClassList = new QListWidget;
//    dockClassList->setFrameStyle(QListWidget::NoFrame);
//    dockLayout->addWidget(dockClassList);
//    dockClassList->insertItems(0, gtMdiLauncher->dockWidgetIds());

//    layout()->addWidget(tabWidget);
}

void
GtPreferencesPlugins::saveSettings()
{

}

void
GtPreferencesPlugins::loadSettings()
{

}

