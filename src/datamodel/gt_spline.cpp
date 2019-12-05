/* GTlab - Gas Turbine laboratory
 * Source File: gt_spline.cpp
 * copyright 2009-2015 by DLR
 *
 *  Created on: Tu Sep 1 2015
 *      Author: Maximilian Vieweg
 *		 Email: maximilian.vieweg@dlr.de
 */

// Datamodel class
// offers function "getCurvePoint" that returns

#include "gt_splinecp.h"

#include "gt_spline.h"
#include <QDebug>
#include <QPointF>

GtSpline::GtSpline() :
    m_degree("degree", "Degree", "B-Spline degree", GtUnit::None,
             GtIntProperty::BoundLow, 3, 3)
{
    registerProperty(m_degree);

    setFlag(GtObject::UserRenamable);
    setFlag(GtObject::UserDeletable);
}

GtSpline::GtSpline(int degree) :
    m_degree("degree", "Degree", "B-Spline degree", GtUnit::None,
             GtIntProperty::BoundLow, 3, 3)
{
    QList<QPointF> controlPoints;
    controlPoints.append(QPointF(0.0, 0.0));
    controlPoints.append(QPointF(0.25, 0.0));
    controlPoints.append(QPointF(0.5, 0.0));
    controlPoints.append(QPointF(0.75, 0.0));
    controlPoints.append(QPointF(1.0, 0.0));

    foreach (const QPointF& cp, controlPoints)
    {
        addControlPoint(cp);
    }

    setDegree(degree);

    QVector<double> knot(degree + controlPoints.size() + 1);
    for (int i = 0; i < knot.size(); ++i)
    {
        if (i < knot.size() / 2)
        {
            knot[i] = 0.0;
        }
        else if (i == knot.size() / 2)
        {
            knot[i] = 0.5;
        }
        else
        {
            knot[i] = 1.0;
        }
    }


    setKnotVec(knot);
}

GtSpline::GtSpline(const QList<QPointF>& controlPoints, int degree,
                   const QVector<double>& knot) :
    m_degree("degree", "Degree", "B-Spline degree", GtUnit::None,
             GtIntProperty::BoundLow, 3, 3)
{
    registerProperty(m_degree);

    setFlag(GtObject::UserRenamable);
    setFlag(GtObject::UserDeletable);

    foreach (const QPointF& cp, controlPoints)
    {
        addControlPoint(cp);
    }

    setDegree(degree);
    setKnotVec(knot);
}

void
GtSpline::setDegree(int degree)
{
    m_degree = degree;
}

int
GtSpline::degree()
{
    return m_degree;
}

//void GtSpline::setControlPoints(QList<QPointF> controlP)
//{
//    m_controlPoints = controlP;
//}

//void
//GtSpline::setControlPoints(int mark, QPointF controlP)
//{
//    m_controlPoints[mark] = controlP;
//}

QList<GtSplineCP*>
GtSpline::controlPoints()
{
    return findDirectChildren<GtSplineCP*>();
}

QList<QPointF>
GtSpline::controlPointsConverted()
{
    QList<QPointF> retval;

    foreach (GtSplineCP* cp, controlPoints())
    {
        retval << cp->point();
    }

    return retval;
}

void
GtSpline::setKnotVec(QVector<double> knotVec)
{
    m_knotVec = knotVec;
}

QVector<double>
GtSpline::KnotVec()
{
    return m_knotVec;
}

void
GtSpline::addControlPoint(const QPointF& cp)
{
    appendChild(new GtSplineCP(cp));
}

QVector<double>
GtSpline::rawPointsY() const
{
    return m_rawPointsY;
}

void
GtSpline::setRawPointsY(const QVector<double> &rawPointsY)
{
    m_rawPointsY = rawPointsY;
}

QVector<double>
GtSpline::rawPointsX() const
{
    return m_rawPointsX;
}

void
GtSpline::setRawPointsX(const QVector<double> &rawPointsX)
{
    m_rawPointsX = rawPointsX;
}

