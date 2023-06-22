/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTTEMPLATEVIEWERLISTWIGET_H
#define GTTEMPLATEVIEWERLISTWIGET_H

#include "gt_gui_exports.h"

#include "QListWidget"

/**
 * @brief The GtTemplateViewerListWidget class
 */
class GT_GUI_EXPORT GtTemplateViewerListWidget : public QListWidget
{
    Q_OBJECT

public:

    GtTemplateViewerListWidget();

    void adjustWidgetSize();
};

#endif // GTTEMPLATEVIEWERLISTWIGET_H
