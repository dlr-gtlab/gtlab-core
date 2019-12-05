/* GTlab - Gas Turbine laboratory
 * Source File: gt_splinecp.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 17.12.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SPLINECP_H
#define GT_SPLINECP_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

class QPointF;

/**
 * @brief The GtSplineCP class
 */
class GT_DATAMODEL_EXPORT GtSplineCP : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(double x MEMBER m_x NOTIFY pointChanged)
    Q_PROPERTY(double y MEMBER m_y NOTIFY pointChanged)

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtSplineCP();

    /**
     * @brief Constructor.
     * @param cp Control point initialization.
     */
    GtSplineCP(const QPointF& cp);

    /**
     * @brief point
     * @return QPointF with coordinates of GtdPointF member values for x and r
     */
    QPointF point();

    /**
     * @brief Returns x-coordinate og spline control point.
     * @return X-coordinate.
     */
    double x();

    /**
     * @brief Returns y-coordinate og spline control point.
     * @return Y-coordinate.
     */
    double y();

    /**
     * @brief setValue
     * @param val
     */
    void setValue(const QPointF& val);

signals:
    void pointChanged();

private:
    /// X-Coordinate.
    double m_x;

    /// Y-Coordinate.
    double m_y;
};

#endif // GT_SPLINECP_H
