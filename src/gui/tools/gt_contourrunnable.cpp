/* GTlab - Gas Turbine laboratory
 * Source File: gt_contourrunnable.cpp
 * copyright 2009-2019 by DLR
 *
 *  Created on: 22.02.2019
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
// Contour Calculation for a structured Grid (i x j) based on an adaption of the
// CONREC contour algorithm (http://paulbourke.net/papers/conrec/)
// Every node of the grid is assigned to a data set of three
// values (x,y,z). Hence input requires a table with two axis and one variable
// containing contour values:
// X: Contains the x-coordinate values for every node
// Y: Contains the y-coordinate values for every node
// Z: Contains the z-coordinate values for every node
//
// Input is required as GtTable which allows to define two axes (the grid: X, Y)
// and the contour values on top of it, in this class following syntax is used:
// i: line
// j: coloumn
//
// X and Y span the plain in which the contour lines need to be pictured
// Z contains the values of the desired Contour variable
//
// Additional input is the desired iso-value of the contour variable
// For the interpolation on the different tables, the GTLab table class is
// applied which enables easy handling of irregular grids.
// For detailed Information see numerics/tables.h and numerics/table.cc
//
//            ---> x-values    __
//                    ________/  \_____
//  |                /__\__/_/__/__\__/
//  |               /__/__/_/__\__/___/         z-values on every node
//  |              /_/__/__\__/__/___/
//  y-values      /__\__/__/  /__/__/
//                         \_/
// Output is given as a list of polygons of different sizes:
// 2 Points: Contour Line for considered iso-value
// 3 or more Points: Polygon which can be closed, plotted and filled by color
// for coloured contour plot

//  PHI/XI Meshes used for the Interpolation are regular grids
//     ------> PHI (j)
//  |   __ __ __ __ __ __
//  |  |__|__|__|__|__|__|
//  |  |__|__|__|__|__|__|
//  XI |__|__|__|__|__|__|
// (i) |__|__|__|__|__|__|
//     |__|__|__|__|__|__|
//
#include "gt_contourrunnable.h"

#include "gt_objectfactory.h"

GtContourRunnable::GtContourRunnable(GtObjectMemento tableMemento,
                                     QString isoValue,
                                     bool calcIsoPolygons,
                                     bool considerNextIsoValue,
                                     QString nextIsoValue) :
    m_tableMemento(tableMemento),
    m_isoValue(isoValue),
    m_calcIsoPolygons(calcIsoPolygons),
    m_considerNextIsoValue(considerNextIsoValue),
    m_nextIsoValue(nextIsoValue)
{
    setAutoDelete(false);
}

void
GtContourRunnable::run()
{
    if (!m_tableMemento.isRestorable(gtObjectFactory))
    {
        gtError() << "Could not restore table from memento!";
        return;
    }

    calculateContour();

    emit finished();
}

QString
GtContourRunnable::isoValue()
{
    return m_isoValue;
}

QList<QPolygonF>
GtContourRunnable::isoLines()
{
    return m_isoLines;
}

QList<QPolygonF>
GtContourRunnable::isoPolygons()
{
    return m_isoPolygons;
}

//#############################################################################
// CALCULATE CONTOUR LINES / POLYGONS
//#############################################################################
void
GtContourRunnable::calculateContour()
{
    // FIND CONTOUR LINES FOR GIVEN CONTOUR LEVEL FOR EVERY CELL
    // WITHIN THE GIVEN GRID
    //
    //               ---->X (PHI) (j)
    //      |    1 x------------x 4
    //      |      |\         /|
    //      |      |  \     /  |
    //      Y      |    \ /    |
    //     (XI)    |    / \    |
    //      (i)    |  /     \  |
    //             |/         \|
    //           2 x-----------x 3

    m_isoLines.clear();
    m_isoPolygons.clear();

    GtTable table;
    table.fromMemento(m_tableMemento);

    if(!tableValid(table))
    {
        gtError() << "Table is invalid!";
        return;
    }

    bool check = true;
    double isoVal = m_isoValue.toDouble(&check);

    if (!check)
    {
        gtError() << "ISO level conversion failed!";
        return;
    }

    double nextIsoVal = m_nextIsoValue.toDouble(&check);

    if (!check && m_considerNextIsoValue)
    {
        gtInfo() << m_nextIsoValue;
        gtError() << "Next ISO level conversion failed!";
        return;
    }

    double val1;
    double val2;
    double val3;
    double val4;

    QVector<double> phiValues;
    table.getAxisTicks("phi", phiValues);

    QVector<double> xiValues;
    table.getAxisTicks("xi", xiValues);

    int iDim = xiValues.size();
    int jDim = phiValues.size();

    //CHECK EVERY RECTANGLE IN GIVEN MESH REGARDING GIVEN CONTOUR LEVEL
    for (int i = 0; i < iDim - 1; i++)
    {
        for (int j = 0; j < jDim - 1; j++)
        {

            QPolygonF iso_line;
            QVector<QPointF> conPoints;

            //SET VALUES FOR CURRENT RECTANGLE
            val1 = table.getValue2DatIndices("z", j, i);
            val2 = table.getValue2DatIndices("z", j, i+1);
            val3 = table.getValue2DatIndices("z", j+1, i+1);
            val4 = table.getValue2DatIndices("z", j+1, i);

            //ALL VALUES SMALLER THAN CONTOUR LEVEL --> NEXT RECTANGLE
            if (val1 < isoVal && val2 < isoVal && val3 < isoVal &&
                val4 < isoVal)
            {
                continue;
            }

            //DEFINE BOOL OPERATORS TO DEFINE POINT POSITIONS ON RECTANGLE
            bool left = false;
            bool right = false;
            bool up = false;
            bool down = false;
            double valTmp = 0;

            //LOCATE INTERSECTION POINTS ON ALL RECTANGLE SIDES (LIN. INTERP.)
            // VERTICAL SIDES
            if (intersection(val1, val2, isoVal,
                             xiValues[i], xiValues[i + 1], valTmp))
            {
                left = true;
                conPoints.append(QPointF(phiValues[j], valTmp));
            }

            if (lineOnIsoLevel(val1, val2, isoVal))
            {
                conPoints.append(QPointF(phiValues[j], xiValues[i]));
                conPoints.append(QPointF(phiValues[j], xiValues[i + 1]));
            }

            if (intersection(val3, val4, isoVal,
                             xiValues[i + 1], xiValues[i], valTmp))
            {
                right = true;
                conPoints.append(QPointF(phiValues[j + 1], valTmp));
            }

            if (lineOnIsoLevel(val3, val4, isoVal))
            {
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i]));
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i + 1]));
            }

            // HORIZONTAL SIDES
            if (intersection(val1, val4, isoVal,
                             phiValues[j], phiValues[j + 1], valTmp))
            {
                up = true;
                conPoints.append(QPointF(valTmp, xiValues[i]));
            }

            if (lineOnIsoLevel(val1, val4, isoVal))
            {
                conPoints.append(QPointF(phiValues[j], xiValues[i]));
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i]));
            }

            if (intersection(val2, val3, isoVal,
                             phiValues[j], phiValues[j + 1], valTmp))
            {
                down = true;
                conPoints.append(QPointF(valTmp, xiValues[i + 1]));
            }

            if (lineOnIsoLevel(val2, val3, isoVal))
            {
                conPoints.append(QPointF(phiValues[j], xiValues[i + 1]));
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i + 1]));
            }

            // DIAGONAL INTERSECTION
            if (val1 == isoVal && val3 == isoVal &&
                val2 != isoVal && val4 != isoVal)
            {
                conPoints.append(QPointF(phiValues[j], xiValues[i]));
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i + 1]));
            }

            if (val2 == isoVal && val4 == isoVal &&
                val1 != isoVal && val3 != isoVal)
            {
                conPoints.append(QPointF(phiValues[j + 1], xiValues[i]));
                conPoints.append(QPointF(phiValues[j], xiValues[i + 1]));
            }

            //ADD POLYGON TO LIST (ONLY IF 2 POINTS WERE FOUND FOR CONTOUR LINE)
            if (conPoints.size() > 1)
            {
                for (int k = 0; k < conPoints.size(); k++)
                {
                    iso_line << conPoints[k];
                }

                QPolygonF iso_line_trans;
                iso_line_trans = toXY(iso_line, table);
                m_isoLines << iso_line_trans;
            }

            //------------------------------------------------------------------
            //           ISO LINE CALCULATION FINISHED
            //------------------------------------------------------------------
            if (m_calcIsoPolygons)
            {
                QPolygonF corners;
                QPolygonF corners_trans;

                //ALL VALUES HIGHER THAN CONTOUR LEVEL
                //--> ADD CURRENT RECTANGLE AS POLYGON TO LIST
                if (val1 >= isoVal && val2 >= isoVal && val3 >= isoVal &&
                    val4 >= isoVal)
                {
                    if (m_considerNextIsoValue)
                    {
                        if (val1 >= nextIsoVal && val2 >= nextIsoVal
                                && val3 >= nextIsoVal &&  val4 >= nextIsoVal)
                        {
                            continue;
                        }
                    }

//                    QPolygonF corners;
//                    QPolygonF corners_trans;

                    //ADD CORNERS TO POLYGON
                    corners << QPointF(phiValues[j],
                                       xiValues[i]);
                    corners << QPointF(phiValues[j + 1],
                                       xiValues[i]);
                    corners << QPointF(phiValues[j + 1],
                                       xiValues[i + 1]);
                    corners << QPointF(phiValues[j],
                                       xiValues[i + 1]);

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;
                    continue;
                }

                //FIND POLYGONS TO FILL FOR COLOUR PLOT DEPENDENT ON
                // POSITION OF INTERSECTION POINTS

                //INTERSECTION POINTS ON LEFT AND RIGHT SIDE OF RECTANGLE
                if (left && right && conPoints.size() == 2)
                {
                    if (val1 < isoVal && val4 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    else
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //INTERSECTION POINTS ON UP- AND UNDERSIDE OF RECTANGLE
                if (up && down && conPoints.size() == 2)
                {
                    if (val1 < isoVal && val2 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    else
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //INTERSECTION POINTS ON LEFT AND UNDERSIDE OF RECTANGLE
                if (left && down && conPoints.size() == 2)
                {
                    if (val2 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    else
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //INTERSECTION POINTS ON RIGHT AND UNDERSIDE OF RECTANGLE
                if (right && down && conPoints.size() == 2)
                {
                    if (val3 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    else
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //INTERSECTION POINTS ON RIGHT AND UPSIDE OF RECTANGLE
                if (right && up && conPoints.size() == 2)
                {
                    if (val4 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << conPoints.last();
                    }

                    else
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //INTERSECTION POINTS ON LEFT AND UPSIDE OF RECTANGLE
                if (left && up && conPoints.size() == 2)
                {
                    if (val1 < isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << conPoints.last();
                    }

                    if (val1 > isoVal)
                    {
                        corners << conPoints.first();
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << conPoints.last();
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //RECTANGLE SIDES
                if (val1 == isoVal && val2 == isoVal)
                {
                    if (val3 > isoVal && val4 > isoVal)
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                if (val2 == isoVal && val3 == isoVal)
                {
                    if (val1 > isoVal && val4 > isoVal)
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                if (val3 == isoVal && val4 == isoVal)
                {
                    if (val2 > isoVal && val3 > isoVal)
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                if (val1 == isoVal && val4 == isoVal)
                {
                    if (val3 > isoVal && val2 > isoVal)
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                //DIAGONAL INTERSECTION
                if (val1 == isoVal && val3 == isoVal)
                {
                    if (val4 > isoVal && val2 < isoVal)
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                    }

                    else
                    {
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    ///  TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }

                if (val2 == isoVal && val4 == isoVal)
                {
                    if (val1 > isoVal && val3 < isoVal)
                    {
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    else
                    {
                        corners << QPointF(phiValues[j + 1], xiValues[i]);
                        corners << QPointF(phiValues[j + 1], xiValues[i + 1]);
                        corners << QPointF(phiValues[j], xiValues[i + 1]);
                    }

                    ///TRANSFORM POINTS FROM MAP COORDINATES (PHI/XI)
                    /// TO XY-COORDINATES
                    corners_trans = toXY(corners, table);

                    //ADD POLYGON TO LIST
                    m_isoPolygons << corners_trans;

                    continue;
                }
            }
        }
    }
}

//#############################################################################
// CHECK TABLE FOR VALIDITY
//#############################################################################
bool
GtContourRunnable::tableValid(GtTable& t)
{
    if (t.getAxesList().size() != 2)
    {
        gtError() << QObject::tr("Number of Axes does not match 2, cannot "
                                 "perform any contour calculation.");

        return false;
    }

    if (!t.valueNames().contains("x"))
    {
        gtError() << QObject::tr("No x values found in this table, "
                                 "cannot perform any contour calculation");

        return false;
    }

    if (!t.valueNames().contains("y"))
    {
        gtError() << QObject::tr("No x values found in this table, "
                                 "cannot perform any contour calculation");

        return false;
    }

    if (!t.valueNames().contains("z"))
    {
        gtError() << QObject::tr("No x values found in this table, "
                                 "cannot perform any contour calculation");

        return false;
    }

    QVector<double> phiValues;
    t.getAxisTicks("phi", phiValues);

    QVector<double> xiValues;
    t.getAxisTicks("xi", xiValues);

    if (phiValues.isEmpty() || xiValues.isEmpty())
    {
        gtError() << "PHI and/or XI values are empty!";
        return false;
    }

    return true;
}

//#############################################################################
// CHECK FOR INTERSECTION
//#############################################################################
bool
GtContourRunnable::intersection(const double& zVal1, const double& zVal2,
                                const double& zValIso, const double& axisValue1,
                                const double& axisValue2, double& val)
{
    if (zVal1 != zValIso && zVal2 != zValIso)
    {
        if (zVal1 > zValIso &&  zVal2 > zValIso)
        {
            return false;
        }

        if (zVal1 < zValIso && zVal2 < zValIso)
        {
            return false;
        }

        val = getTmpValue(axisValue1, axisValue2,
                          zVal1, zVal2, zValIso);

        return true;
    }

    else if (zVal1 == zValIso && zVal2 != zValIso)
    {
        val = axisValue1;
        return true;
    }

    else if (zVal1 != zValIso && zVal2 == zValIso)
    {
        val = axisValue2;
        return true;
    }

    return false;
}

//#############################################################################
// LINEAR INTERPOLATION APPLYING GIVEN MESH
//#############################################################################
double
GtContourRunnable::getTmpValue(const double& x1, const double& x2,
                               const double& y1, const double& y2,
                               const double& isoValue)
{
    double slope;
    double b;
    double temp_value;
    slope = (y2 - y1) / (x2 - x1);
    b = y1 - slope * x1;
    temp_value = (isoValue - b) / slope;

    return temp_value;
}

//#############################################################################
// TRANSFORMATION FROM MAP (XI/PHI) COORDINATES TO XY-COORDINATES
//#############################################################################
QPolygonF
GtContourRunnable::toXY(QPolygonF& polysToTransform, const GtTable& table)
{
    QPolygonF polysXY;

    for (int i = 0; i < polysToTransform.size(); i++)
    {
        double resX = 0.0;
        double resY = 0.0;

        try
        {
            resX = table.getValue2D("x", polysToTransform[i].x(),
                                    polysToTransform[i].y());
            resY = table.getValue2D("y", polysToTransform[i].x(),
                                    polysToTransform[i].y());
        }
        catch (GTlabException& e)
        {
            gtError() << "Exception in GtContourRunnable::toXY"
                      << "what: " << e.what()
                      << "where:" << e.where();
        }

        QPointF p = QPointF(resX, resY);

        polysXY << p;
    }

    return polysXY;
}

//#############################################################################
// Check if considered Rectangle side lies on iso level
//#############################################################################
bool
GtContourRunnable::lineOnIsoLevel(const double& zVal1, const double& zVal2,
                                  const double& zValIso)
{
    if (zVal1 == zValIso && zVal2 == zValIso)
    {
        return true;
    }

    return false;
}
