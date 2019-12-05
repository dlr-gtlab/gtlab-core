/* GTlab - Gas Turbine laboratory
 * Source File: gt_aboutdialog.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QIcon>

#include "gt_application.h"

#include "gt_aboutdialog.h"

GtAboutLogo::GtAboutLogo(QWidget* parent) : QWidget(parent), m_clicks(0)
{
    m_pixmap.load(QStringLiteral(":/pixmaps/splash.png"));
}

void GtAboutLogo::paintEvent(QPaintEvent* e)
{
    QPainter paint(this);
    paint.drawPixmap(0, 0, m_pixmap);

    QWidget::paintEvent(e);
}

void GtAboutLogo::mouseDoubleClickEvent(QMouseEvent* event)
{
    QWidget::mouseDoubleClickEvent(event);
}


GtAboutDialog::GtAboutDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("About GTlab"));
    setWindowIcon(gtApp->icon(QStringLiteral("infoIcon_16.png")));

    /// Turn  off the "?"-Button in the header
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;

    GtAboutLogo* logo = new GtAboutLogo;
    logo->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding));

    layout->addWidget(logo);

    QHBoxLayout* hLayout = new QHBoxLayout;

    QLabel* copyLabel =
            new QLabel(QStringLiteral(
                "GTlab - Gas Turbine laboratory\ncopyright 2009-2018 by DLR"));

    hLayout->addWidget(copyLabel);

    QString vers = gtApp->versionToString() + QStringLiteral("\nHydrogen");

    QLabel* versLabel = new QLabel(vers);
    versLabel->setAlignment(Qt::AlignRight);

    hLayout->addWidget(versLabel);

    layout->addLayout(hLayout);

    layout->setSpacing(0);

    setLayout(layout);

    setMinimumSize(465, 350);
    setMaximumSize(465, 350);
}

