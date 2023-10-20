/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_templateviewerlistwidget.h"

GtTemplateViewerListWidget::GtTemplateViewerListWidget()
{

}

void
GtTemplateViewerListWidget::adjustWidgetSize()
{
    int noi = count();

    if (noi > 0)
    {
        int w = sizeHintForColumn(0) + 22;
        int h = sizeHintForRow(0);
        setMaximumSize(QSize(w, 25 + (h * (noi - 1))));
    }
    else
    {
        setMaximumSize(QSize(50, 23));
    }
}
