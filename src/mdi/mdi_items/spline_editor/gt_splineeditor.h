/* GTlab - Gas Turbine laboratory
 * Source File: gt_splineeditor.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SPLINEEDITOR_H
#define GT_SPLINEEDITOR_H

#include "gt_mdiitem.h"

class GtSplinePlot;

/**
 * @brief The GtSplineEditor class
 */
class GtSplineEditor : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtSplineEditor();

    /**
     * @brief setData
     * @param obj
     */
    virtual void setData(GtObject* obj) Q_DECL_OVERRIDE;

private:
    /// Spline plot widget
    GtSplinePlot* m_plot;

};

#endif // GT_SPLINEEDITOR_H
