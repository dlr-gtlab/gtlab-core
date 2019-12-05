/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineui.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SPLINEUI_H
#define GT_SPLINEUI_H

#include "gt_mdi_exports.h"

#include <QIcon>

#include "gt_objectui.h"

/**
 * @brief The GtSplineUI class
 */
class GT_MDI_EXPORT GtSplineUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtSplineUI();

    /**
     * @brief openWith
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) Q_DECL_OVERRIDE;

    /**
     * @brief icon
     * @return
     */
    QIcon icon(GtObject *) const;

};

#endif // GT_SPLINEUI_H
