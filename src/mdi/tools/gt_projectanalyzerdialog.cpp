/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectanalyzerdialog.cpp
 * copyright 2009-2020 by DLR
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

#include "gt_application.h"
#include "gt_footprint.h"

#include "gt_projectanalyzerdialog.h"

GtProjectAnalyzerDialog::GtProjectAnalyzerDialog(GtProjectAnalyzer* analyzer) :
    QDialog(),
    m_tabWid(Q_NULLPTR)
{
    setWindowTitle(tr("Project Analyzer"));
    setWindowIcon(gtApp->icon("componentsIcon_16.png"));

    QVBoxLayout* mLay = new QVBoxLayout;

    // read information
    if (analyzer != Q_NULLPTR)
    {
        // analyse footprint
        GtFootprint footprint = analyzer->footPrint();

        if (!footprint.isValid())
        {
            QHBoxLayout* ftrntInfoLay = new QHBoxLayout;

            QLabel* ftrntIcon = new QLabel;
            ftrntIcon->setPixmap(
                        gtApp->icon("infoBlueIcon.png").pixmap(32, 32));

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
                ftrntIcon->setPixmap(
                            gtApp->icon("errorIcon.png").pixmap(32, 32));

                QSpacerItem* spacer = new QSpacerItem(10, 20,
                                                      QSizePolicy::Minimum,
                                                      QSizePolicy::Minimum);

                QLabel* ftrntInfo =
                        new QLabel(tr("The project was last edited with a newer"
                                      " release than the current used release"
                                      " of the GTlab framework.\n\n") +
                                   "Framework =\t" +
                                   GtFootprint().versionToString() + "\n" +
                                   "Project =\t\t" +
                                   footprint.versionToString() +
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

                QSpacerItem* spacer2 = new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                                      QSizePolicy::Expanding);
                mLay->addSpacerItem(spacer2);
            }
        }

        // analyse unknown modules
        QMap<QString, int> unknownModules = footprint.unknownModules();

        if (!unknownModules.isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            QListWidget* umListWid = new QListWidget;

            foreach (const QString& str, unknownModules.keys())
            {
                QListWidgetItem* lItem =
                        new QListWidgetItem(gtApp->icon("pluginIcon.png"),
                                            str);
                umListWid->addItem(lItem);
            }

            tabWid->addTab(umListWid, gtApp->icon("errorIcon_16.png"),
                           tr("Unknown Modules"));
        }

        // analyse incompatible modules
        QMap<QString, int> incompatibleModules =
                footprint.incompatibleModules();

        if (!incompatibleModules.isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            QTreeWidget* tWid = new QTreeWidget;
            tWid->setColumnCount(3);
            tWid->setHeaderLabels(QStringList() << "" << "Project" <<
                                  "Framework");

            for(auto e : incompatibleModules.keys())
            {
                QTreeWidgetItem* incompatibleModule =
                        new QTreeWidgetItem(QStringList() << e <<
                                            QString::number(
                                                incompatibleModules.value(e)) <<
                                            QString::number(
                                                gtApp->moduleVersion(e)));

                incompatibleModule->setBackgroundColor(1,
                                                       QColor(255, 0, 0, 100));
                incompatibleModule->setBackgroundColor(2,
                                                       QColor(255, 0, 0, 100));
                incompatibleModule->setIcon(0, gtApp->icon("pluginIcon.png"));

                tWid->addTopLevelItem(incompatibleModule);
            }

            tWid->setColumnWidth(0, 200);

            tabWid->addTab(tWid, gtApp->icon("errorIcon_16.png"),
                           tr("Incompatible Modules"));
        }

        // analyse unknown classes
        if (!analyzer->unknownClasses().isEmpty())
        {
            QTabWidget* tabWid = tabWidget();

            QListWidget* ucListWid = new QListWidget;

            foreach (const QString& str, analyzer->unknownClasses())
            {
                QListWidgetItem* lItem =
                        new QListWidgetItem(gtApp->icon("emptyIcon_16.png"),
                                            str);
                ucListWid->addItem(lItem);
            }

            tabWid->addTab(ucListWid, gtApp->icon("errorIcon_16.png"),
                           tr("Unknown Classes"));
        }
    }

    if (m_tabWid != Q_NULLPTR)
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

    QSpacerItem* spacer = new QSpacerItem(10, 20, QSizePolicy::Expanding,
                                          QSizePolicy::Minimum);

    btnLay->addSpacerItem(spacer);

    QPushButton* okBtn = new QPushButton(tr("Ok"));

    connect(okBtn, SIGNAL(clicked()), this, SLOT(accept()));

    btnLay->addWidget(okBtn);

    mLay->addLayout(btnLay);

    setLayout(mLay);
}

QTabWidget*
GtProjectAnalyzerDialog::tabWidget()
{
    if (m_tabWid == Q_NULLPTR)
    {
        m_tabWid = new QTabWidget;
    }

    return m_tabWid;
}
