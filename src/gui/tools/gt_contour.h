/* GTlab - Gas Turbine laboratory
 * Source File: gt_contour.cpp
 * copyright 2009-2014 by DLR
 *
 *  Created on: 28.03.2014
 *      Author: Carsten Hollmann (AT-TW)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTCONTOUR_H
#define GTCONTOUR_H

#include "gt_gui_exports.h"

#include "gt_objectmemento.h"
#include "gt_table.h"

class QPolygonF;

/**
 * @brief The GtContour class
 */
class GT_GUI_EXPORT GtContour
{

public:
    /**
     * @brief Constructor
     */
    GtContour();


    /**
     * @brief Calculates contour for a certain parameter and a given set of
     * iso values based on the grid of the given table, for each iso value
     * a list of poygons will be given as output, polygons consisting of
     * two points for contour line plotting and polygons including 3 or more
     * points for colour contour plotting
     * @param table table used for contour calculation
     * @param param param name
     * @param isoValues list of iso values
     * @param calcPolygons flag if polygons with three or more points
     * shall be calculated
     * @param returnPolygonsOnly flag if only polygons with three or more
     *  points shall be returned
     * @param check
     * @return Hash including pairs of iso values and related contor polygons
     */
    QHash<QString, QList<QPolygonF> > calcContour(
            GtTable* table,  const QString& param, QStringList isoValues,
            bool calcPolygons = false, bool returnPolygonsOnly = false,
            bool* check = nullptr);

    /**
     * @brief Calculates contour for a certain parameter and a given iso value
     * based on the grid of the given table, for the iso value
     * a list of poygons will be given as output, polygons consisting of
     * two points for contour line plotting and polygons including 3 or more
     * points for colour contour plotting
     * @param table table used for contour calculation
     * @param param param name
     * @param isoValue iso value
     * @param calcPolygons flag if polygons with three or more points
     * shall be calculated
     * @param returnPolygonsOnly flag if only polygons with three or more
     *  points shall be returned
     * @param check
     * @return Hash including pairs of iso values and related contor polygons
     */
    QList<QPolygonF> calcContour(GtTable* table, const QString& param,
                                 const QString& isoValue,
                                 bool calcPolygons = false,
                                 bool returnPolygonsOnly = false,
                                 bool* check = nullptr);

    /**
     * @brief Calculates contour for a certain parameter and a given number of
     * iso values based on the grid of the given table, for each iso value
     * a list of poygons will be given as output, polygons consisting of
     * two points for contour line plotting and polygons including 3 or more
     * points for colour contour plotting, the iso values will be determined
     * based on the minimum and maximum value of the parameter included in the
     * table
     * @param table table used for contour calculation
     * @param param param name
     * @param nIsoValues number of iso values
     * @param calcPolygons flag if polygons with three or more points
     * shall be calculated
     * @param returnPolygonsOnly flag if only polygons with three or more
     *  points shall be returned
     * @param check
     * @return Hash including pairs of iso values and related contor polygons
     */
    QHash<QString, QList<QPolygonF> > calcContour(
            GtTable* table, const QString &param, int nIsoValues,
            bool calcPoygons = false, bool returnPolygonsOnly = false,
            bool* check = nullptr);

    /**
     * @brief Returns current iso values in ascending order
     * @return list of iso values
     */
    QStringList isoValues() const;

private:
    /// calculate polygons with 3 or more points for colour contour plot or not
    bool m_polygonCalculation;

    /// Member table used for contour calculation
    GtTable m_table;

    /// list of iso values
    QStringList m_isoValues;

    /**
     * @brief checks if given table is valid for contour calculation
     * @return
     */
    bool isValid(GtTable* t, const QString& paramName) const;

    /**
     * @brief Converts given table to internal data structure for further usage
     * @param table
     * @param paramName
     */
    bool setData(GtTable* table, QString paramName, int iDim = 0, int jDim = 0);
};

#endif // GTCONTOUR_H
