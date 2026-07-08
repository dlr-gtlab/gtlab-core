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

    using VisibleAxis = QFlags<Qt::Orientation>;

    explicit GtGrid(QGraphicsView& view);
    ~GtGrid();

    GT_DEPRECATED_ATTR(2, 2, "use `setHSpacing` instead.")
    void setGridWidth(int value);
    /**
     * @brief Sets the spacing between horizontal (major) grid lines
     * @param value
     */
    void setHSpacing(size_t value);
    size_t hSpacing() const;

    GT_DEPRECATED_ATTR(2, 2, "use `setVSpacing` instead.")
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
    GT_DEPRECATED_ATTR(2, 2, "Function has no effect. No replacement is planned.")
    void setNumberOfGridPoints(int horizontal, int vertical);

    /** Sets whether the grid should be scaled or not.
        @param val Grid scaling indicator */
    void setScaleGrid(bool val);

    /** Sets whether grid points should be enabled or not.
        @param val Gridpoints indicator */
    GT_DEPRECATED_ATTR(2, 2, "Function has no effect. No replacement is planned.")
    void setShowGridPoints(bool val);

    /** @brief Sets whether the horizontal axis should be enabled or not.
     *  @param show Whether to show the axis
     */
    GT_DEPRECATED_ATTR(2, 2, "Use `setAxisVisible` instead.")
    void setShowAxis(bool show);

    /**
     * @brief Sets which axis should be shown
     * @param show Whether to show the given axis
     * @param orientation Which axis to show
     */
    void setAxisVisible(bool show = true, VisibleAxis axis = Qt::Horizontal | Qt::Vertical);

    /**
     * @brief Returns which axis is shown, if any.
     * @return which axis is visible
     */
    VisibleAxis visibleAxis() const;

    /** Sets horizontal grid line color.
        @param color New horizontal grid line color */
    void setHorizontalGridLineColor(const QColor& color);
    void setHLineColor(const QColor& color);

    /** Sets vertical grid line color.
        @param color New vertical grid line color */
    void setVerticalGridLineColor(const QColor& color);
    void setVLineColor(const QColor& color);

    /** Sets grid point color.
        @param color New grid point color */
    GT_DEPRECATED_ATTR(2, 2, "Function has no effect. No replacement is planned.")
    void setGridPointColor(const QColor& color);

    GT_DEPRECATED_ATTR(2, 2, "use `paint` instead.")
    void paintGrid(QPainter* painter, const QRectF& rect);
    /** @brief Paints full grid.
     *  @param painter QPainter pointer
     *  @param rect Scene rect
     */
    void paint(QPainter& painter, const QRectF& rect );

    /** computeTopLeftGridPoint
     * @param p
     * @return Top left grid Point*/
    GT_DEPRECATED_ATTR(2, 2, "No replacement is planned. If this function is "
                             "needed, contact GTlab support.")
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
    GT_DEPRECATED_ATTR(2, 2, "use `setVisible` instead.")
    void hideGrid(bool hidden = true);

    /**
     * @brief Shows the grid
     * @param val show indicator
     */
    GT_DEPRECATED_ATTR(2, 2, "use `setVisible` instead.")
    void showGrid(bool visible = true);

signals:

    /**
     * @brief Emitted if the grid is updated
     */
    GT_DEPRECATED_ATTR(2, 2, "use visibilityChanged isntead.")
    void update();

    /**
     * @brief Emitted if the grid visibility is changed
     */
    void visibilityChanged();

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTD_GRID_H
