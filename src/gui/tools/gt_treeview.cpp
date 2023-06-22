/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QKeyEvent>

#include "gt_treeview.h"
#include "gt_application.h"

GtTreeView::GtTreeView(QWidget* parent) : QTreeView(parent)
{

}

void
GtTreeView::keyPressEvent(QKeyEvent* event)
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

    return QTreeView::keyPressEvent(event);
}
