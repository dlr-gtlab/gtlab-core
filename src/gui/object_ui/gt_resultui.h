/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTRESULTUI_H
#define GTRESULTUI_H

#include "gt_gui_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtResultUI class
 */
class GT_GUI_EXPORT GtResultUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtResultUI
     */
    Q_INVOKABLE GtResultUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const override;

};

#endif // GTRESULTUI_H
