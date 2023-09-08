/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_simpleloadingwidget.cpp
 *
 *  Created on: 29.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QThread>

#include "gt_simpleloadingwidget.h"

GtSimpleLoadingWidget::GtSimpleLoadingWidget(GtAbstractLoadingHelper* helper,
                                             QWidget* parent) :
    GtDialog(parent),
    m_forceClose(false),
    m_helper(helper)
{
    setWindowTitle(tr("loading..."));

    setAutoFillBackground(true);

    QVBoxLayout* lay = new QVBoxLayout;

    int loaderSize = 48;

    m_animation = new QMovie(QStringLiteral(":/pixmaps/loading2.gif"),
                             QByteArray(), this);

    m_animation->setScaledSize(QSize(loaderSize, loaderSize));
    QLabel* animLabel = new QLabel;
    animLabel->setMovie(m_animation);
    animLabel->setFixedSize(loaderSize, loaderSize);

    lay->addWidget(animLabel);

    setLayout(lay);

    m_animation->start();

#if defined(WIN32)
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#else
    setMaximumWidth(60);
    setMaximumHeight(30);
#endif
}

void
GtSimpleLoadingWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (!m_helper)
    {
        return;
    }

    QThread* thread = new QThread;

    m_helper->moveToThread(thread);

    connect(thread, SIGNAL(started()), m_helper, SLOT(exec()));
    connect(m_helper, SIGNAL(finished()), SLOT(onFinished()));
    connect(m_helper, SIGNAL(finished()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void
GtSimpleLoadingWidget::closeEvent(QCloseEvent* event)
{
    if (m_forceClose)
    {
        event->accept();
    }

    event->ignore();
}

void
GtSimpleLoadingWidget::keyPressEvent(QKeyEvent* event)
{
    event->ignore();
}

void
GtSimpleLoadingWidget::onFinished()
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_forceClose = true;
    accept();
}
