#ifndef GTGRAPHICSSPLINEITEM_H
#define GTGRAPHICSSPLINEITEM_H

#include "gt_mdi_exports.h"

#include <QGraphicsPathItem>
#include <QPointer>

#include "gt_spline.h"

class GT_MDI_EXPORT GtGraphicsSplineItem : public QGraphicsPathItem
{
public:
    /**
     * @brief GtGraphicsSplineItem
     * @param spline
     * @param dotThres
     */
    GtGraphicsSplineItem(GtSpline* spline, double dotThres, double valDTmin);

    /**
     * @brief calcDrawPoints
     * @param spline
     * @param t0
     * @param t1
     * @param insIndx
     * @return
     */
    int calcDrawPoints(GtSpline* spline, double t0, double t1, int insIndx);

    /**
     * @brief Curve
     * @return
     */
    QList<QPointF> Curve();

    /**
     * @brief normalise
     * @param p0
     * @param p1
     * @return
     */
    QPointF normalise(QPointF p0, QPointF p1);

    /**
     * @brief vecLength
     * @param p0
     * @param p1
     * @return
     */
    double vecLength(QPointF p0, QPointF p1);

    /**
     * @brief PointsChecked
     * @return
     */
    QList<QPointF> PointsChecked() const;

    /**
     * @brief setPointsChecked
     * @param PointsChecked
     */
    void setPointsChecked(const QList<QPointF>& PointsChecked);

private:
    /// efficient points on curve
    QList<QPointF> m_curve;

    /// points that were looked through during efficient algorithm
    QList<QPointF> m_pointsChecked;

    QPointer<GtSpline> m_spline;

    /// user specified threshold of dot product of two vectors
    /// representing the angle between the two vectors
    /// -0.999 means that angles lower than 177.43 ° entail an interim point
    double m_dotThreshold;

    //// minimum parametric distance to perform the efficient
    /// algorithm on
    double m_dTmin;

    void createPath();
};

#endif // GTGRAPHICSSPLINEITEM_H
