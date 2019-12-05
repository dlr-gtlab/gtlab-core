/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorsdock.cpp
 * copyright 2009-2016 by DLR
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

    QPalette palette = widget->palette();
    palette.setColor(backgroundRole(), QColor(255, 255, 255));
    widget->setPalette(palette);

    widget->setAutoFillBackground(true);

    QToolBox* toolbox = new QToolBox;
    layout->addWidget(toolbox);
}

Qt::DockWidgetArea
GtCalculatorsDock::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}
