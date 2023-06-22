/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.06.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QTreeWidget>
#include <QString>

#include "gt_projectanalyzer.h"
#include "gt_project.h"
#include "gt_versionnumber.h"

#include "gt_projectupgradeoverviewpage.h"

GtProjectUpgradeOverviewPage::GtProjectUpgradeOverviewPage(GtProject* project,
                                                           QWidget* parent) :
    QWizardPage(parent)
{
    setTitle(tr("Available Upgrades"));

    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* label = new QLabel(tr("The following upgrades are available:"));
    layout->addWidget(label);

    QTreeWidget* treeWid = new QTreeWidget;
    treeWid->setHeaderHidden(true);
    layout->addWidget(treeWid);

    GtProjectAnalyzer analyzer(project);
    GtFootprint footprint = analyzer.footPrint();

    foreach (const QString& modId, project->availableModuleUpgrades())
    {
        const QList<GtVersionNumber> avail = project->availableUpgrades(modId);

        if (avail.isEmpty())
        {
            continue;
        }

        QString newestVer = avail.last().toString();
        QString preVer;
        QString modStr = modId;

        if (modId == GtFootprint::frameworkIdentificationString())
        {
            preVer = footprint.frameworkVersion().toString();
            modStr = "Core Framework";
        }
        else
        {
            preVer =  footprint.modules().value(modId).toString() +
                    tr(" (current)");
        }

        QTreeWidgetItem* rootItem = new QTreeWidgetItem(treeWid, {modStr});
        QTreeWidgetItem* child = new QTreeWidgetItem(rootItem, {preVer +
                                                                " -> " +
                                                                newestVer});

        rootItem->addChild(child);
        treeWid->addTopLevelItem(rootItem);
    }

    treeWid->expandAll();

    layout->addStretch(1);

    setLayout(layout);
}
