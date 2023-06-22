/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_lineedit.h"

GtLineEdit::GtLineEdit(QWidget* parent) : QLineEdit(parent)
{

}

void
GtLineEdit::focusInEvent(QFocusEvent* event)
{
    emit focusIn();

    QLineEdit::focusInEvent(event);
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

