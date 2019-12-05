#ifndef GTSPLINEUTILITIES_H
#define GTSPLINEUTILITIES_H

#include "gt_datamodel_exports.h"

#include <QList>
#include <QPointF>
#include <QVector>

class GtSpline;

namespace GtSplineUtilities {

GT_DATAMODEL_EXPORT GtSpline* insertKnot(GtSpline* spline, double t);

GT_DATAMODEL_EXPORT QList<GtSpline*> splitCurve(
        GtSpline* spline, double t, QVector<double> knot);

/**
 * @brief isAxialFunction
 * @param spline
 * @param nLoops number of points that are tested
 * @param movedCp is the number of the current control point being moved
 * If this is set with a valid valid (unlike -1) the function
 * checks only the curve parameter values that are affected by the
 * control point
 * @return
 */
GT_DATAMODEL_EXPORT bool isAxialFunction(
        GtSpline* spline, double nLoops = 100, int movedCp = -1);


GT_DATAMODEL_EXPORT bool isVerticalFunction(
        GtSpline* spline, double nLoops = 100, int movedCp = -1);

GT_DATAMODEL_EXPORT QPointF getYfromX(GtSpline* spline, double xVal,
                                      double tGuess = 0.5);

GT_DATAMODEL_EXPORT QPointF getXfromY(GtSpline* spline, double yVal);

int multiplicity(double t, QVector<double> knot);

double funcXvariation(GtSpline* spline, double xVal, double t);

double funcYvariation(GtSpline* spline, double yVal, double t);

GT_DATAMODEL_EXPORT double pointsDistance(QPointF p0, QPointF p1);

/**
 * @brief halfWay - middle point of the line between two points
 * @param p0
 * @param p1
 * @return
 */
GT_DATAMODEL_EXPORT QPointF halfWay(QPointF p0, QPointF p1);

}


#endif // GTSPLINEUTILITIES_H
