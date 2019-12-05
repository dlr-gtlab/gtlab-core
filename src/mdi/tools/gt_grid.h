/* GTlab - Gas Turbine laboratory
 * Source File: gtd_grid.h
 * copyright 2009-2013 by DLR
 *
 *  Created on: 17.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_GRID_H
#define GTD_GRID_H

#include "gt_mdi_exports.h"

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QRectF>

class QPainter;
class GtGraphicsView;
class GtRuler;

class GT_MDI_EXPORT GtGrid : public QObject
{
    Q_OBJECT

public:
    GtGrid(GtGraphicsView& view);

    /** Sets grid width to given value.
        @param val New grid width */
    void setGridWidth(int val);

    /** Sets grid height to given value.
        @param val New grid height */
    void setGridHeight(int val);

    /** Sets number of grid points for one grid rect.
        @param horizontal Number of horizontal grid points
        @param vertical Number of vertical grid points */
    void setNumberOfGridPoints(int horizontal, int vertical);

    /** Sets whether the grid should be scaled or not.
        @param val Grid scaling indicator */
    void setScaleGrid(bool val);

    /** Sets whether grid points should be enabled or not.
        @param val Gridpoints indicator */
    void setShowGridPoints(bool val);

    /** Sets whether axis should be enabled or not.
        @param val Axis indicator */
    void setShowAxis(bool val);

    /** Sets horizontal grid line color.
        @param color New horizontal grid line color */
    void setHorizontalGridLineColor(const QColor& color);

    /** Sets vertical grid line color.
        @param color New vertical grid line color */
    void setVerticalGridLineColor(const QColor& color);

    /** Sets grid point color.
        @param color New grid point color */
    void setGridPointColor(const QColor& color);

    /** Paints full grid.
        @param painter QPainter pointer
        @param rect Scene rect */
    void paintGrid(QPainter* painter, const QRectF& rect);

    /** computeTopLeftGridPoint
     * @param p
     * @return Top left grid Point*/
    QPointF computeTopLeftGridPoint(const QPointF& p);

    /** computeNearestGridPoint
     * @param p
     * @return Nearest Grid point to p */
    QPointF computeNearestGridPoint(const QPointF& p);

    /** Sets current grid scale factor.
        @param val Grid scale factor */
    void setGridScaleFactor(int val);

    /** Paints grid on ruler.
        @param ruler GtdRuler */
    void paintRuler(GtRuler* ruler);

    /** Sets new horizontal ruler.
        @param ruler New horizontal ruler */
    void setHorizontalRuler(GtRuler* ruler);

    /** Sets new vertical ruler.
        @param ruler New vertical ruler */
    void setVerticalRuler(GtRuler* ruler);

public slots:
    /**
     * @brief Hides the grid
     * @param val hide indicator
     */
    void hideGrid(bool val = true);

    /**
     * @brief Shows the grid
     * @param val show indicator
     */
    void showGrid(bool val = true);

protected:
    /** Paints horizontal and vertical grid lines.
        @param painter QPainter pointer
        @param rect Scene rect */
    void paintGridLines(QPainter* painter, const QRectF &rect);

    /** Paints horizontal and vertical axis.
        @param painter QPainter pointer
        @param rect Scene rect */
    void paintAxis(QPainter* painter, const QRectF &rect);

    /** Returns scaled grid width.
        @return Scaled grid width */
    qreal getScaledGridWidth();

    /** Returns scaled grid height.
        @return Scaled grid height */
    qreal getScaledGridHeight();

private:
    /// GtdGraphicsView
    GtGraphicsView& m_view;

    /// Grid width
    int m_width;

    /// Grid height
    int m_height;

    /// Number of horizontal grid points
    int m_nohgp;

    /// Number of vertical grid points
    int m_novgp;

    /// Grid scale indicator
    bool m_scaleGrid;

    /// Grid scaling factor
    int m_gridFactor;

    /// Grid points indicator
    bool m_showGridPoints;

    /// Axis indicator
    bool m_showAxis;

    /// Hide grid
    bool m_hideGrid;

    /// Horizontal grid line color
    QColor m_hgColor;

    /// Vertical grid line color
    QColor m_vgColor;

    /// Grid point color
    QColor m_gpColor;

    /// Scene rect
    QRectF m_rect;

    /// Horizontal ruler
    GtRuler* m_hRuler;

    /// Vertical ruler
    GtRuler* m_vRuler;

    /** Returns scaled grid vor given orientation.
        @param val Orientation*/
    qreal getScaledGrid(Qt::Orientation val);

    /** Draws rotated text for vertical axis ticks.
        @param painter Current Painter
        @param x Coordinate
        @param y Coordinate
        @param text Tick text */
    void drawRotatedText(QPainter *painter, int x, int y, const QString &text);

signals:
    void update();

};

#endif // GTD_GRID_H
