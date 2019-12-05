#ifndef GTSPLINE_H
#define GTSPLINE_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

#include "gt_intproperty.h"

#include <QVector>

class GtSplineCP;

/**
 * @brief The GtSpline class
 */
class GT_DATAMODEL_EXPORT GtSpline : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> knot MEMBER m_knotVec)

    Q_PROPERTY(QVector<double> rawX MEMBER m_rawPointsX)

    Q_PROPERTY(QVector<double> rawY MEMBER m_rawPointsY)

public:
    /**
     * @brief GtSpline
     */
    Q_INVOKABLE GtSpline();

    GtSpline(int degree);

    /**
     * @brief GtSpline
     * @param Cp
     * @param N
     * @param knot
     */
    GtSpline(const QList<QPointF>& controlPoints, int degree,
             const QVector<double>& knot);

    /**
     * @brief setN
     * @param degree
     */
    void setDegree(int degree);

    /**
     * @brief N
     * @return
     */
    int degree();

//    /**
//     * @brief setCp
//     * @param controlP
//     */
//    void setControlPoints(QList<QPointF> controlP);

//    /**
//     * @brief setCp
//     * @param mark
//     * @param controlP
//     */
//    void setControlPoints(int mark, QPointF controlP);

    /**
     * @brief Cp
     * @return
     */
    QList<GtSplineCP*> controlPoints();

    /**
     * @brief controlPointsConverted
     * @return
     */
    QList<QPointF> controlPointsConverted();

    /**
     * @brief setKnotVec
     * @param knotVec
     */
    void setKnotVec(QVector<double> knotVec);

    /**
     * @brief KnotVec
     * @return
     */
    QVector<double> KnotVec();

    /**
     * @brief addControlPoint
     * @param cp
     */
    void addControlPoint(const QPointF& cp);

    QVector<double> rawPointsX() const;

    void setRawPointsX(const QVector<double> &rawPoints);

    QVector<double> rawPointsY() const;

    void setRawPointsY(const QVector<double> &rawPointsY);

private:
    /// int degree of spline
    GtIntProperty m_degree;

    //    /// list of control points
//    /// dont use pointers, so that u dont have to worry about freeing memory
//    QList<QPointF> m_controlPoints;

    /// knot vector of each control point
    /// determines pull force to each control point
    QVector<double> m_knotVec;

    QVector<double> m_rawPointsX;

    QVector<double> m_rawPointsY;

};

#endif // GTSPLINE_H
