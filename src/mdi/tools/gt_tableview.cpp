/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QKeyEvent>

#include "gt_tableview.h"

GtTableView::GtTableView(QWidget* parent) : QTableView(parent)
{
}


void GtTableView::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_F)
        {
            emit searchRequest();
            return;
        }
        else if (event->key() == Qt::Key_C)
        {
            emit copyRequest();
            return;
        }
    }

    return QTableView::keyPressEvent(event);
}