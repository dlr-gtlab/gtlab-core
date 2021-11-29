/* GTlab - Gas Turbine laboratory
 * Source File: gt_startuppage.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 19.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QFrame>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QToolButton>

#include "gt_application.h"
#include "gt_settings.h"
#include "gt_logging.h"
#include "gt_stylesheets.h"

#include "gt_startuppage.h"

GtStartupPage::GtStartupPage()
{
    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    QFrame* frame = new QFrame(this);

    QVBoxLayout* frameLay = new QVBoxLayout;

    QLabel* label = new QLabel;

    frameLay->addWidget(label);

    QGridLayout* gridLay = new QGridLayout;

    gridLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                     QSizePolicy::Expanding), 0, 1);
    gridLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Minimum,
                                     QSizePolicy::Expanding), 2, 1);
    gridLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding,
                                     QSizePolicy::Minimum), 1, 0);
    gridLay->addItem(new QSpacerItem(10, 10, QSizePolicy::Expanding,
                                     QSizePolicy::Minimum), 1, 2);

    QGridLayout* btnGridLay = new QGridLayout;

    QString welcomebuttonStyleSheet;

    QString border = "border: 1px solid gray;"
                     "border-radius: 4px;";

    QString size = "min-width: 110px;"
                   "min-height:110px;";

    if (!gtApp->inDarkMode())
    {
        welcomebuttonStyleSheet =  "QAbstractButton { "
                + border +
               "background-color:white;"
                + size +
                "font: bold;}"
               "QAbstractButton:hover{background-color:"
               "rgb(220,235,250)}"
               "QAbstractButton:disabled {background-color:"
               "rgb(240,240,240)}"
               "QAbstractButton:pressed{background-color:"
               "rgb(180,213,246)}";
    }
    else
    {
        welcomebuttonStyleSheet =  "QAbstractButton { "
                + border +
                "background-color:#2d2d2d;" /// the same as 45,45,45 RGB
                + size +
                "font: bold;}"
               "QAbstractButton:hover{background-color:"
               "rgb(180,200,200)}"
               "QAbstractButton:disabled {background-color:"
               "rgb(180,180,180)}"
               "QAbstractButton:pressed{background-color:"
               "rgb(180,213,213)}";
    }

    QIcon newP = QIcon(":/icons/addProjectIcon.png");
    QIcon examples = QIcon(":/icons/examplesIcon.png");
    QIcon helpIcon = QIcon(":/icons/questionIcon.png");
    QIcon infoIcon = QIcon(":/icons/infoIcon.png");

    QSize iconSize(80, 80);

    QToolButton* newProjBtn = new QToolButton;
    newProjBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    newProjBtn->setIcon(newP);
    newProjBtn->setIconSize(iconSize);
    newProjBtn->setStyleSheet(welcomebuttonStyleSheet);
    newProjBtn->setText(tr("New Project"));
    btnGridLay->addWidget(newProjBtn, 0, 0);

    QToolButton* examplesBtn = new QToolButton;
    examplesBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    examplesBtn->setIcon(examples);
    examplesBtn->setIconSize(iconSize);
    examplesBtn->setStyleSheet(welcomebuttonStyleSheet);
    examplesBtn->setText(tr("Examples"));
    btnGridLay->addWidget(examplesBtn, 0, 1);

    QToolButton* helpBtn = new QToolButton;
    helpBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    helpBtn->setIcon(helpIcon);
    helpBtn->setIconSize(iconSize);
    helpBtn->setStyleSheet(welcomebuttonStyleSheet);
    helpBtn->setText(tr("Help Contents"));
    btnGridLay->addWidget(helpBtn, 1, 0);

    QToolButton* infoBtn = new QToolButton(this);
    infoBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    infoBtn->setIcon(infoIcon);
    infoBtn->setIconSize(iconSize);
    infoBtn->setText("Info");
    infoBtn->setStyleSheet(welcomebuttonStyleSheet);


                //QSize(110, 120));
    btnGridLay->addWidget(infoBtn, 1, 1);

    //QPushButton* infoBtn = new QPushButton;
    //infoBtn->setIcon(QIcon(":/pixmaps/infoPix.png"));
    //infoBtn->setIconSize(iconSize);
    //infoBtn->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    //btnGridLay->addWidget(infoBtn, 1, 1);

    connect(newProjBtn, SIGNAL(clicked(bool)), this, SIGNAL(newProject()));
    connect(examplesBtn, SIGNAL(clicked(bool)), this,
            SIGNAL(openExamplesWidget()));
    connect(helpBtn, SIGNAL(clicked(bool)), this, SIGNAL(helpContents()));
    connect(infoBtn, SIGNAL(clicked(bool)), this, SIGNAL(showInfo()));

    gridLay->addLayout(btnGridLay, 1, 1);

    frameLay->addLayout(gridLay);

    frame->setLayout(frameLay);

    lay->addWidget(frame);

    QHBoxLayout* checkLay = new QHBoxLayout;

    QCheckBox* check = new QCheckBox(tr("Show at Startup"));

    if (gtApp->settings()->showStartupPage())
    {
        check->setChecked(true);
    }

    connect(check, SIGNAL(clicked(bool)), SLOT(showIndicatorToggled(bool)));

    checkLay->addWidget(check);
    checkLay->setContentsMargins(5, 0, 0, 0);

    lay->addLayout(checkLay);

    setLayout(lay);

    frame->setAutoFillBackground(true);
    frame->setObjectName(QStringLiteral("frame"));
    frame->setAutoFillBackground(true);
    frame->setFrameShape(QFrame::Box);
    frame->setFrameShadow(QFrame::Raised);
    frame->setStyleSheet(
                "#frame {border-image: url(:/pixmaps/startup-background.png)}");

    label->setPixmap(QPixmap(":/pixmaps/gt-logo.png"));
    label->setLayoutDirection(Qt::RightToLeft);

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Sunken);
}

GtStartupPage::~GtStartupPage()
{
    gtDebug() << "startup page deleted!";
}

QIcon
GtStartupPage::icon() const
{
    return gtApp->icon("infoBlueIcon_16.png");
}

void
GtStartupPage::showIndicatorToggled(bool val)
{
    gtApp->settings()->setShowStartupPage(val);
}
