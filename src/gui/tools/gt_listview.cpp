/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QKeyEvent>

#include "gt_listview.h"

GtListView::GtListView(QWidget* parent) : QListView(parent)
{

}

void
GtListView::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_F)
        {
            emit searchRequest();
            return;
        }

        if (event->key() == Qt::Key_C)
        {
            emit copyRequest();
            return;
        }

    }
    else if (event->key() == Qt::Key_Delete)
    {
        emit deleteRequest();
        return;
    }

    return QListView::keyPressEvent(event);
}

