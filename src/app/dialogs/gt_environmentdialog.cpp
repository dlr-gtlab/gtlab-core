/* GTlab - Gas Turbine laboratory
 * Source File: gt_environmentdialog.cpp
 * copyright 2009-2019 by DLR
 *
 *  Created on: 26.07.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTableView>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "gt_environmentmodel.h"

#include "gt_environmentdialog.h"

GtEnvironmentDialog::GtEnvironmentDialog(const QStringList& envVars,
                                         const QString& winTitle,
                                         const QString& infoText,
                                         QWidget* parent) : GtDialog(parent)
{
    setWindowTitle(winTitle);

    QTableView* envView = new QTableView;

    m_envModel = new GtEnvironmentModel(envVars, envView);
    envView->setModel(m_envModel);

    envView->horizontalHeader()->setStretchLastSection(true);
    envView->resizeColumnsToContents();
    envView->verticalHeader()->setVisible(false);
    envView->verticalHeader()->setDefaultSectionSize(16);

    QVBoxLayout* envLay = new QVBoxLayout;

    QLabel* envLbl = new QLabel(infoText);

    QHBoxLayout* envBtnLay = new QHBoxLayout;
    QPushButton* envSaveBtn = new QPushButton(tr("Save"));
    QPushButton* envExitBtn = new QPushButton(tr("Exit"));
    QSpacerItem* envSpacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                             QSizePolicy::Minimum);
    envBtnLay->addSpacerItem(envSpacer);
    envBtnLay->addWidget(envSaveBtn);
    envBtnLay->addWidget(envExitBtn);

    envLay->addWidget(envLbl);
    envLay->addWidget(envView);
    envLay->addLayout(envBtnLay);

    setLayout(envLay);

    connect(envSaveBtn, SIGNAL(clicked(bool)), SLOT(save()));
    connect(envExitBtn, SIGNAL(clicked(bool)), SLOT(reject()));
}

void
GtEnvironmentDialog::save()
{
    m_envModel->saveVariables();
    accept();
}
