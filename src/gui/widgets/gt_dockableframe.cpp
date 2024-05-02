/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 30.4.2024
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#include "gt_dockableframe.h"

#include <QHideEvent>

#include <gt_logging.h>

GtDockableFrame::GtDockableFrame(QWidget* parent, Qt::WindowFlags f) :
    QFrame(parent, f)
{

}

void
GtDockableFrame::hideEvent(QHideEvent* event)
{
    gtWarning() << parent() << event->spontaneous() << event->type();
    if (parent() || !event->spontaneous()) return QFrame::hideEvent(event);

    event->accept();
    emit redockWidget();
}
