/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOPERATINGPOINTUI_H
#define GTOPERATINGPOINTUI_H

#include "gt_mdi_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtOperatingPointUI class
 */
class GT_MDI_EXPORT GtOperatingPointUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtOperatingPointUI
     */
    Q_INVOKABLE GtOperatingPointUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

};

#endif // GTOPERATINGPOINTUI_H
