/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTPOSTTEMPLATEITEM_H
#define GTPOSTTEMPLATEITEM_H

#include "gt_gui_exports.h"

#include "gt_object.h"
#include "gt_abstractpostwidget.h"

class GT_GUI_EXPORT GtPostTemplateItem : public GtObject
{
    Q_OBJECT

public:

    virtual GtAbstractPostWidget* createUI(GtPostTemplateItem* dm,
                                           QWidget* parent) = 0;


protected:
    GtPostTemplateItem();
};

#endif // GTPOSTTEMPLATEITEM_H
