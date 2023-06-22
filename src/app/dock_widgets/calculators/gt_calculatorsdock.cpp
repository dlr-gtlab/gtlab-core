/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorsdock.cpp
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFrame>
#include <QToolBox>
#include <QVBoxLayout>

#include "gt_calculatorsdock.h"

GtCalculatorsDock::GtCalculatorsDock()
{
    setObjectName(tr("Calculators"));

    QFrame* widget = new QFrame(this);
    setWidget(widget);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    widget->setLayout(layout);

    widget->setFrameShape(QFrame::StyledPanel);
//    widget->setFrameStyle(m_listView->frameStyle());
//    widget->setFrameShadow(m_listView->frameShadow());

    widget->setAutoFillBackground(true);

    QToolBox* toolbox = new QToolBox;
    layout->addWidget(toolbox);
}

Qt::DockWidgetArea
GtCalculatorsDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}
