/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_lineedit.h"

GtLineEdit::GtLineEdit(QWidget* parent) : QLineEdit(parent)
{

}

void
GtLineEdit::focusOutEvent(QFocusEvent* event)
{
    if (text().isEmpty())
    {
        emit clearFocusOut();
    }
    else
    {
        emit focusOut();
    }

    QLineEdit::focusOutEvent(event);
}

