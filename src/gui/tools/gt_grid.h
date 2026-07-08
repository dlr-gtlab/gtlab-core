/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_grid.h
 *
 *  Created on: 17.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_GRID_H
#define GTD_GRID_H

#include "gt_gui_exports.h"
#include "gt_version.h"

#include <QObject>
#include <QPointF>
#include <QColor>
#include <QRectF>

#include <memory>

class QPainter;
class QGraphicsView;
class GtRuler;

class GT_GUI_EXPORT GtGrid : public QObject
{
    Q_OBJECT

public:

    explicit GtGrid(QGraphicsView& view);
    ~GtGrid();

    GT_DEPRECATED_ATTR(2, 2, "use setHSpacing instead.")
    void setGridWidth(int value);
    /**
     * @brief Sets the spacing between horizontal (major) grid lines
     * @param value
     */
    void setHSpacing(size_t value);
    size_t hSpacing() const;

    GT_DEPRECATED_ATTR(2, 2, "use setVSpacing instead.")
    void setGridHeight(int value);
    /**
     * @brief Sets the spacing between vertical (major) grid lines
     * @param value
     */
    void setVSpacing(size_t value);
    size_t vSpacing() const;

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

    /** @brief Paints full grid.
     *  @param painter QPainter pointer
     *  @param rect Scene rect
     */
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

    /**
     * @brief Returns whether the grid is visible
     * @return Is grid visible
     */
    bool isVisible() const;

public slots:

    /**
     * @brief Sets the visibility of the grid
     * @param visible Whether the grid should be visible or hidden
     */
    void setVisible(bool visible = true);

    /**
     * @brief Hides the grid
     * @param val hide indicator
     */
    GT_DEPRECATED_ATTR(2, 2, "use setVisible")
    void hideGrid(bool hidden = true);

    /**
     * @brief Shows the grid
     * @param val show indicator
     */
    GT_DEPRECATED_ATTR(2, 2, "use setVisible")
    void showGrid(bool visible = true);

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
    double getScaledGridWidth();

    /** Returns scaled grid height.
        @return Scaled grid height */
    double getScaledGridHeight();

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /** Returns scaled grid vor given orientation.
        @param val Orientation*/
    double getScaledGrid(Qt::Orientation orientation);

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
