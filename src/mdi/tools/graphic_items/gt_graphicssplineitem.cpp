/* GTlab - Gas Turbine laboratory
 * Source File: gt_graphicssplineitem.cpp
 * copyright 2009-2015 by DLR
 *
 *  Created on: Tu Sep 1 2015
 *      Author: Maximilian Vieweg
 *		 Email: maximilian.vieweg@dlr.de
 */

#include <QDebug>
#include "qmath.h"

#include "gt_spline.h"
#include "gt_exceptions.h"

#include "sl_common.h"
#include "sl_point.h"
#include "sl_bsplinecurve.h"

#include "gt_graphicssplineitem.h"

GtGraphicsSplineItem::GtGraphicsSplineItem(GtSpline* spline, double dotThres,
                                           double valDTmin) :
    m_spline(spline),
    m_dotThreshold(dotThres),
    m_dTmin(valDTmin)
{
    createPath();
}

void
GtGraphicsSplineItem::createPath()
{
    if (m_spline)
    {
//        double dTmin = 0.2;
        double t0;
        double t1;
        double tMid;
        int iterations = int(1.0 / m_dTmin);
//        qDebug() << "iterations = " << iterations;
        int drawPointsCount = 0;
        QList<QPointF> controls = m_spline->controlPointsConverted();

        // convert control points
        std::vector<SLPoint> cps;
        foreach (const QPointF& cp, controls)
        {
            cps.push_back(SLPoint(cp.x(), cp.y()));
        }

        // generate spline
        SLBSplineCurve spline_tmp(cps, m_spline->KnotVec().toStdVector(),
                                  m_spline->degree());

        for (int i = 0; i < iterations; i++)
        {
            t0 = i * m_dTmin;
            t1 = (i + 1) * m_dTmin;
            tMid = (t0 + t1) / 2;

            QPointF p0;
            QPointF p1;

            try
            {
                SLPoint p0_temp = spline_tmp.eval2(t0);

                p0.setX(p0_temp.X());
                p0.setY(p0_temp.Y());

//                p0 = GtBSplineAlgorithm::curvePoint(
//                            t0,
//                            controls,
//                            m_spline->KnotVec(),
//                            m_spline->degree());
            }
            catch (GTlabException &e)
            {
                gtError() << "Exception in function" << e.where() << "!";
                gtError() << "Explanation:" << e.what();
                return ;
            }
            try
            {
                SLPoint p1_temp = spline_tmp.eval2(t1);

                p1.setX(p1_temp.X());
                p1.setY(p1_temp.Y());

//                p1 = GtBSplineAlgorithm::curvePoint(
//                            t1,
//                            controls,
//                            m_spline->KnotVec(),
//                            m_spline->degree());
            }
            catch (GTlabException &e)
            {
                gtError() << "Exception in function" << e.where() << "!";
                gtError() << "Explanation:" << e.what();
                return ;
            }

            if (i == 0)
            {
                m_curve.append(p0);
            }

            int tmp_drawPointsCount = drawPointsCount;
            drawPointsCount += calcDrawPoints(m_spline, t0, t1, 1 + drawPointsCount);

            if (tmp_drawPointsCount == drawPointsCount)
            {
                // no further drawing point has been added
                // enforce an addition

                QPointF pMid;

                try
                {
                    SLPoint pMid_temp = spline_tmp.eval2(tMid);

                    pMid.setX(pMid_temp.X());
                    pMid.setY(pMid_temp.Y());

//                    pMid = GtBSplineAlgorithm::curvePoint(
//                                tMid,
//                                controls,
//                                m_spline->KnotVec(),
//                                m_spline->degree());
                }
                catch (GTlabException &e)
                {
                    gtError() << "Exception in function" << e.where() << "!";
                    gtError() << "Explanation:" << e.what();
                    return;
                }

                m_curve.insert(drawPointsCount + 1, pMid);
                drawPointsCount++;
            }

            if (i == (iterations - 1))
            {
                m_curve.append(p1);
            }

        }

        QPainterPath path;
        for (int i = 0; i < m_curve.size(); i++)
        {
            if (i == 0)
            {
                path.moveTo(QPointF(m_curve[0]));
            }
            else
            {
                path.lineTo(QPointF(m_curve[i]));
            }
        }
        setPath(path);

    }
}

int
GtGraphicsSplineItem::calcDrawPoints(GtSpline* spline,
                                     double t0, double t1, int insIndx)
{
    QPointF p0;
    QPointF p1;


    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, m_spline->KnotVec().toStdVector(),
                              m_spline->degree());

    try
    {
        SLPoint p0_temp = spline_tmp.eval2(t0);

        p0.setX(p0_temp.X());
        p0.setY(p0_temp.Y());

//        p0 = GtBSplineAlgorithm::curvePoint(
//                    t0, controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException &e)
    {
        gtError() << "Exception in function" << e.where() << "!";
        gtError() << "Explanation:" << e.what();
        return 0;
    }
    try
    {
        SLPoint p1_temp = spline_tmp.eval2(t1);

        p1.setX(p1_temp.X());
        p1.setY(p1_temp.Y());

//        p1 = GtBSplineAlgorithm::curvePoint(
//                    t1, controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException &e)
    {
        gtError() << "Exception in function" << e.where() << "!";
        gtError() << "Explanation:" << e.what();
        return 0;
    }

    // Minimum distance of two points is 5
    if (vecLength(p0, p1) < 5)
    {
        qDebug() << "########## Miminum distance of points reached. " <<
                    "No further mid points added in efficient algorithm";
        return 0;
    }

    double tMid = (t0 + t1) / 2;

    QPointF pMid;

    try
    {
        SLPoint pMid_temp = spline_tmp.eval2(tMid);

        pMid.setX(pMid_temp.X());
        pMid.setY(pMid_temp.Y());

//        pMid = GtBSplineAlgorithm::curvePoint(
//                    tMid,
//                    controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException &e)
    {
        gtError() << "Exception in function" << e.where() << "!";
        gtError() << "Explanation:" << e.what();
        return 0;
    }

    QPointF leftVector = normalise(p0, pMid);
    QPointF rightVector = normalise(p1, pMid);

    qreal dotProd = QPointF::dotProduct(leftVector, rightVector);

    if (dotProd > m_dotThreshold)
    {
        int pointsAdded = 0;

        m_pointsChecked.append(pMid);
        pointsAdded += calcDrawPoints(spline, t0, tMid, insIndx);

        m_curve.insert(insIndx + pointsAdded, pMid);

        pointsAdded++;

        pointsAdded += calcDrawPoints(spline, tMid, t1, insIndx + pointsAdded);

        return pointsAdded;
    }
    m_pointsChecked.append(pMid);

    return 0;
}

QList<QPointF>
GtGraphicsSplineItem::Curve()
{
    return m_curve;
}

QPointF
GtGraphicsSplineItem::normalise(QPointF p0, QPointF p1)
{
    double length = vecLength(p0, p1);
    return (p0 - p1) / length;
}

double
GtGraphicsSplineItem::vecLength(QPointF p0, QPointF p1)
{
    return qSqrt(pow((p0-p1).x(), 2) + pow((p0-p1).y(), 2));
}
QList<QPointF> GtGraphicsSplineItem::PointsChecked() const
{
    return m_pointsChecked;
}

void GtGraphicsSplineItem::setPointsChecked(const QList<QPointF>& PointsChecked)
{
    m_pointsChecked = PointsChecked;
}


