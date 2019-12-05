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

    QSize iconSize(110, 110);

    QPushButton* newProjBtn = new QPushButton;
    newProjBtn->setIcon(QIcon(":/pixmaps/openProjectPix.png"));
    newProjBtn->setIconSize(iconSize);
    newProjBtn->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    btnGridLay->addWidget(newProjBtn, 0, 0);

    QPushButton* examplesBtn = new QPushButton;
    examplesBtn->setIcon(QIcon(":/pixmaps/examplesPix.png"));
    examplesBtn->setIconSize(iconSize);
    examplesBtn->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    btnGridLay->addWidget(examplesBtn, 0, 1);

    QPushButton* helpBtn = new QPushButton;
    helpBtn->setIcon(QIcon(":/pixmaps/helpPix.png"));
    helpBtn->setIconSize(iconSize);
    helpBtn->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    btnGridLay->addWidget(helpBtn, 1, 0);

    QPushButton* infoBtn = new QPushButton;
    infoBtn->setIcon(QIcon(":/pixmaps/infoPix.png"));
    infoBtn->setIconSize(iconSize);
    infoBtn->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    btnGridLay->addWidget(infoBtn, 1, 1);

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
