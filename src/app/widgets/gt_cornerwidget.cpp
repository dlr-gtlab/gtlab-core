/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_cornerwidget.cpp
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>

#include "gt_mainwin.h"

#include "gt_cornerwidget.h"

GtCornerWidget::GtCornerWidget(GtMainWin* mainwin) : QWidget(mainwin),
    m_mainwin(mainwin)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_updateLabel = new QLabel;
    m_updateLabel->setToolTip(tr("Searching for updates..."));
    layout->addWidget(m_updateLabel);
}

void
GtCornerWidget::showUpdateWidget(bool val)
{
    if (!m_updateLabel)
    {
        return;
    }

    if (val)
    {
        m_updateLabel->setVisible(true);

        if (!m_updateAnim)
        {
            m_updateAnim = new QMovie(QStringLiteral(":/pixmaps/loading.gif"),
                                      QByteArray(), this);
        }

        m_updateLabel->setMovie(m_updateAnim);
        m_updateAnim->start();
    }
    else
    {
        m_updateLabel->setVisible(false);
        delete m_updateAnim;
    }
}
