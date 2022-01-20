/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelui.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELUI_H
#define GTLABELUI_H

#include "gt_objectui.h"

/**
 * @brief The GtLabelUI class
 */
class GtLabelUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtLabelUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

};

#endif // GTLABELUI_H
