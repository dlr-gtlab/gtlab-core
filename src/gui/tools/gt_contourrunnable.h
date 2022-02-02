/* GTlab - Gas Turbine laboratory
 * Source File: gt_contourrunnable.h
 * copyright 2009-2019 by DLR
 *
 *  Created on: 22.02.2019
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GT_CONTOURRUNNABLE_H
#define GT_CONTOURRUNNABLE_H

#include <QRunnable>
#include <QPolygonF>

#include "gt_table.h"
#include "gt_objectmemento.h"


/**
 * @brief The GtContourRunnable class
 */
class GtContourRunnable : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * @brief Runnable Object for multithreaded contour calculation
     * @param tableMemento memento of table for contour calculation
     * @param isoValue iso value used for contour calculation
     * @param calcIsoPolygons list of polygons representing the contour
     */
    GtContourRunnable(GtObjectMemento tableMemento,
                      QString isoValue, bool calcIsoPolygons,
                      bool considerNextIsoValue = false,
                      QString nextIsoValue = "");

    /**
     * @brief Runnable run method (overloaded from QRunnable)
     */
    void run() override;

    /**
     * @brief returns iso value
     * @return
     */
    QString isoValue();

    /**
     * @brief returns iso lines (polygons with 2 points)
     * @return
     */
    QList<QPolygonF> isoLines();

    /**
     * @brief returns iso polygons (polygons with 3 or more points)
     * @return
     */
    QList<QPolygonF> isoPolygons();

private:
    /// memento of table for contour calculation
    GtObjectMemento m_tableMemento;

    /// iso value
    QString m_isoValue;

    /// flag if polygons with 3 or more points should be calculated
    bool m_calcIsoPolygons;

    /// list of iso lines (2 points)
    QList<QPolygonF> m_isoLines;

    /// list of polygons with 3 or more points for coloured contour plotting
    QList<QPolygonF> m_isoPolygons;

    /// Flag if a higher contour value needs to be considered within
    /// the polygon calculation in order to reduce the total number of polygons
    bool m_considerNextIsoValue;

    /// next higher iso value
    QString m_nextIsoValue;

    /**
     * @brief Contour calculation routine (description at top of header file)
     */
    void calculateContour();

    /**
     * @brief Checks if given table is valid for contour calculation
     * @param t
     * @return
     */
    bool tableValid(GtTable& t);

    /**
     * @brief Checks the intersection of an edge defined by two
     * values z1 and z2 related to certain phi/xi coordinates
     * (axis value1 and axisValue2) and the assessed contour iso value zValIso
     * As well calculates the intersection point and stores it to parametr val
     * @param zVal1 z value of vertice 1
     * @param zVal2 z value of vertice 2
     * @param zValIso iso value
     * @param axisValue1 phi/xi of vertice 1
     * @param axisValue2 phi/xi of vertice 2
     * @param val phi/xi of intersection point
     * @return flag detecting whether there is an intersection or not
     */
    bool intersection(const double& zVal1, const double& zVal2,
                      const double& zValIso, const double& axisValue1,
                      const double& axisValue2, double& val);

    /**
     * @brief Helper function for linear interpolation
     * @param x1 x1
     * @param x2 x2
     * @param y1 f(x1)
     * @param y2 f(x2)
     * @param isoValue iso value
     * @return
     */
    double getTmpValue(const double& x1, const double& x2,
                       const double& y1, const double& y2,
                       const double& isoValue);

    /**
     * @brief Transformation from xi/phi corrdinates back into x/y corrdinates
     * @param polysToTransform polygons to transform
     * @param table phi/xi table including x/y data for transformation
     * @return
     */
    QPolygonF toXY(QPolygonF& polysToTransform, const GtTable& table);

    /**
     * @brief Checks if an edge given by two vertices lies
     * directly on the contour level given by the iso value
     * @param zVal1 z value of vertice 1
     * @param zVal2 z value of vertice 2
     * @param zValIso is value
     * @return
     */
    bool lineOnIsoLevel(const double& zVal1, const double& zVal2,
                        const double& zValIso);

signals:
    /**
     * @brief Signal that contour calculation was finished
     */
    void finished();

};

#endif // GT_CONTOURRUNNABLE_H
