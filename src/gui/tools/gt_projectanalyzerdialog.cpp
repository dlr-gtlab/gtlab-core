/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectanalyzerdialog.cpp
 *
 *  Created on: 26.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QCheckBox>

#include "gt_application.h"
#include "gt_footprint.h"
#include "gt_versionnumber.h"
#include "gt_algorithms.h"
#include "gt_icons.h"
#include "gt_colors.h"

#include "gt_projectanalyzerdialog.h"

GtProjectAnalyzerDialog::GtProjectAnalyzerDialog(GtProjectAnalyzer* analyzer) :
    GtDialog(),
    m_tabWid(nullptr)
{
    setWindowTitle(tr("Project Analyzer"));
    setWindowIcon(gt::gui::icon::projectOpen());

    QVBoxLayout* mLay = new QVBoxLayout;

    // read information
    if (analyzer)
    {
        // analyse footprint
        GtFootprint footprint = analyzer->footPrint();

        if (!footprint.isValid())
        {
            QHBoxLayout* ftrntInfoLay = new QHBoxLayout;

            QLabel* ftrntIcon = new QLabel;
            ftrntIcon->setPixmap(gt::gui::icon::info2().pixmap(32, 32));

            QSpacerItem* spacer = new QSpacerItem(10, 20,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Minimum);

            QLabel* ftrntInfo =
                    new QLabel(tr("No footprint was found in the project file."
                                  "\nIt seems that the project was created "
                                  "with a GTlab version < 1.6.\n\nSaving the "
                                  "project with a GTlab version >= "
                                  "1.6 allows the recording of a footprint."));

            ftrntInfoLay->addWidget(ftrntIcon);
            ftrntInfoLay->addSpacerItem(spacer);
            ftrntInfoLay->addWidget(ftrntInfo);
            mLay->addLayout(ftrntInfoLay);

            // horizontal line
            QFrame* line = new QFrame;
            line->setObjectName(QString::fromUtf8("line"));
            line->setGeometry(QRect(320, 150, 118, 3));
            line->setFrameShape(QFrame::HLine);
            line->setFrameShadow(QFrame::Sunken);
            mLay->addWidget(line);

            QSpacerItem* spacer2 = new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                                  QSizePolicy::Expanding);
            mLay->addSpacerItem(spacer2);
        }
        else
        {
            if (footprint.isNewerRelease())
            {
                QHBoxLayout* ftrntInfoLay = new QHBoxLayout;

                QLabel* ftrntIcon = new QLabel;
                ftrntIcon->setPixmap(gt::gui::icon::error().pixmap(32, 32));

                QSpacerItem* spacer = new QSpacerItem(10, 20,
                                                      QSizePolicy::Minimum,
                                                      QSizePolicy::Minimum);

                QLabel* ftrntInfo =
                        new QLabel(tr("The project was last edited with a newer"
                                      " release than the current used release"
                                      " of the GTlab framework.\n\n") +
                                   "Framework =\t" +
                                   GtFootprint().frameworkVersion().toString()
                                   + "\n" +
                                   "Project =\t\t" +
                                   footprint.frameworkVersion().toString() +
                                   "\n\nThis can lead to unexpected behavior.");

                ftrntInfoLay->addWidget(ftrntIcon);
                ftrntInfoLay->addSpacerItem(spacer);
                ftrntInfoLay->addWidget(ftrntInfo);
                mLay->addLayout(ftrntInfoLay);

                // horizontal line
                QFrame* line = new QFrame;
                line->setObjectName(QString::fromUtf8("line"));
                line->setGeometry(QRect(320, 150, 118, 3));
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                mLay->addWidget(line);

                QSpacerItem* spacer2 = new QSpacerItem(10, 10,
                                                       QSizePolicy::Minimum,
                                                       QSizePolicy::Expanding);
                mLay->addSpacerItem(spacer2);
            }
        }

        // analyse unknown modules
        QMap<QString, GtVersionNumber> unknownModules = footprint.unknownModules();

        if (!unknownModules.isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            QListWidget* umListWid = new QListWidget;
            umListWid->setFrameStyle(QFrame::NoFrame);

            gt::for_each_key(unknownModules, [&](const QString& str)
            {
                QListWidgetItem* lItem =
                        new QListWidgetItem(gt::gui::icon::plugin(), str);
                umListWid->addItem(lItem);
            });

            tabWid->addTab(umListWid, gt::gui::icon::error(),
                           tr("Unknown Modules"));
        }

        // analyse incompatible modules
        QMap<QString, GtVersionNumber> incompatibleModules =
                footprint.incompatibleModules();

        if (!incompatibleModules.isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            auto* tWid = new QTreeWidget;
            tWid->setFrameStyle(QFrame::NoFrame);
            tWid->setColumnCount(3);
            tWid->setHeaderLabels(QStringList() << "" << "Project" <<
                                  "Framework");
            gt::for_each_key(incompatibleModules, [&](const QString& e)
            {
                auto* incompatibleModule =
                        new QTreeWidgetItem(QStringList() << e <<
                                     incompatibleModules.value(e).toString() <<
                                     gtApp->moduleVersion(e).toString());

                incompatibleModule->setData(
                            1, Qt::BackgroundRole,
                            gt::gui::color::footprintError());
                incompatibleModule->setData(
                            2, Qt::BackgroundRole,
                            gt::gui::color::footprintError());

                incompatibleModule->setIcon(0, gt::gui::icon::plugin());

                tWid->addTopLevelItem(incompatibleModule);
            });

            tWid->setColumnWidth(0, 200);

            tabWid->addTab(tWid, gt::gui::icon::error(),
                           tr("Incompatible Modules"));
        }

        // analyse unknown classes
        if (!analyzer->unknownClasses().isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            QListWidget* ucListWid = new QListWidget;
            ucListWid->setFrameStyle(QFrame::NoFrame);

            for (const auto& str : analyzer->unknownClasses())
            {
                auto lItem = new QListWidgetItem(gt::gui::icon::objectUnknown(),
                                                 str);
                ucListWid->addItem(lItem);
            }

            tabWid->addTab(ucListWid, gt::gui::icon::error(),
                           tr("Unknown Classes"));
        }
    }

    if (m_tabWid)
    {
        QLabel* irregulInfo =
                new QLabel(tr("Irregularities found in the project file:"));
        mLay->addWidget(irregulInfo);

        QSpacerItem* spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding,
                                              QSizePolicy::Minimum);
        mLay->addSpacerItem(spacer);

        mLay->addWidget(m_tabWid);
    }

    QHBoxLayout* btnLay = new QHBoxLayout;

    m_ignoreCheck = new QCheckBox(
        tr("Hide irregularity alerts for this project"));
    btnLay->addWidget(m_ignoreCheck);

    QSpacerItem* spacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum);

    btnLay->addSpacerItem(spacer);

    QPushButton* okBtn = new QPushButton(tr("Ok"));

    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));

    btnLay->addWidget(okBtn);

    mLay->addLayout(btnLay);

    setLayout(mLay);
}

bool
GtProjectAnalyzerDialog::ignoreIrregularities() const
{
    return m_ignoreCheck->isChecked();
}

QTabWidget*
GtProjectAnalyzerDialog::tabWidget()
{
    if (!m_tabWid)
    {
        m_tabWid = new QTabWidget;
    }

    return m_tabWid;
}
