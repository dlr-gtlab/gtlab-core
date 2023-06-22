/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QKeyEvent>

#include "gt_tableview.h"
#include "gt_application.h"

GtTableView::GtTableView(QWidget* parent) : QTableView(parent)
{ }

void
GtTableView::keyPressEvent(QKeyEvent* event)
{
    if (gtApp->compareKeyEvent(event, "search"))
    {
        emit searchRequest();
    }
    else if (gtApp->compareKeyEvent(event, "copy"))
    {
        emit copyRequest();
    }
    else if (gtApp->compareKeyEvent(event, "delete"))
    {
        emit deleteRequest();
    }

    return QTableView::keyPressEvent(event);
}
