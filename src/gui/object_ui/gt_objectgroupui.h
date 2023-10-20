/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 15.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTGROUPUI_H
#define GTOBJECTGROUPUI_H

#include "gt_gui_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtObjectGroupUI class
 */
class GT_GUI_EXPORT GtObjectGroupUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtObjectGroupUI
     */
    Q_INVOKABLE GtObjectGroupUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

};

#endif // GTOBJECTGROUPUI_H
