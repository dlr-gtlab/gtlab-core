/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTDATAZONEUI_H
#define GTABSTRACTDATAZONEUI_H

#include "gt_gui_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtAbstractDataZoneUI class
 */
class GT_GUI_EXPORT GtDataZoneUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractDataZoneUI
     */
    Q_INVOKABLE GtDataZoneUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

    /**
     * @brief openWith
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) Q_DECL_OVERRIDE;

};

#endif // GTABSTRACTDATAZONEUI_H
