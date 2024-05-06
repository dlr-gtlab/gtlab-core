/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 30.4.2024
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#include "gt_mdiwidget.h"

#include <QHideEvent>

GtMdiWidget::GtMdiWidget(QWidget* parent, Qt::WindowFlags f) :
    QFrame(parent, f)
{

    setFrameStyle(QFrame::NoFrame);
}

void
GtMdiWidget::hideEvent(QHideEvent* event)
{
    if (parent() || !event->spontaneous()) return QFrame::hideEvent(event);

    event->accept();
    emit redockWidget();
}
