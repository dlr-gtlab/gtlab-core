/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.03.2017
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 2579
 */
#include "gt_scrollarea.h"
#include "QKeyEvent"
#include <QDebug>

GtScrollArea::GtScrollArea()
{

}

void
GtScrollArea::keyPressEvent(QKeyEvent* e)
{
    QScrollArea::keyPressEvent(e);
}

void
GtScrollArea::wheelEvent(QWheelEvent* e)
{
    if (!e)
    {
        return;
    }

    if (e->modifiers().testFlag(Qt::ControlModifier))
    {
        return;
    }

    e->accept();
    QScrollArea::wheelEvent(e);
}
