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
#include "gt_icons.h"

#include "gt_startuppage.h"

GtStartupPage::GtStartupPage()
{
    setObjectName("Home");

    QVBoxLayout* lay = new QVBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(5);

    m_frame = new QFrame(widget());
    m_frame->setFrameShape(QFrame::StyledPanel);
    m_frame->setFrameShadow(QFrame::Sunken);

    // must be set explicitly otherwise this widget may sometimes not rerecieve
    // the focus and shortcuts etc. wont be handled by main window
    widget()->setFocusPolicy(Qt::StrongFocus);

    QVBoxLayout* frameLay = new QVBoxLayout;

    m_logoLabel = new QLabel;

    frameLay->addWidget(m_logoLabel);
    frameLay->addStretch(1);

    m_frame->setLayout(frameLay);

    lay->addWidget(m_frame);

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

    widget()->setLayout(lay);

    initializeTheme();

    m_logoLabel->setPixmap(gt::gui::pixmap::logo());
    m_logoLabel->setLayoutDirection(Qt::RightToLeft);
}

QIcon
GtStartupPage::icon() const
{
    return gt::gui::icon::infoBlue16();
}

bool
GtStartupPage::allowsMultipleInstances() const
{
    return false;
}

void
GtStartupPage::onThemeChanged()
{
    initializeTheme();
}

void
GtStartupPage::showIndicatorToggled(bool val)
{
    gtApp->settings()->setShowStartupPage(val);
}

void
GtStartupPage::initializeTheme()
{
    m_frame->setAutoFillBackground(true);
    m_frame->setObjectName(QStringLiteral("frame"));
    m_frame->setAutoFillBackground(true);
    m_frame->setFrameShape(QFrame::Box);
    m_frame->setFrameShadow(QFrame::Raised);

    m_logoLabel->setPixmap(gt::gui::pixmap::logo());
    m_logoLabel->setLayoutDirection(Qt::RightToLeft);

    if (gtApp->inDarkMode())
    {
        m_frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background_dark.png)}");
    }
    else
    {
        m_frame->setStyleSheet("#frame {border-image: "
                             "url(:/pixmaps/startup-background.png)}");
    }
}
