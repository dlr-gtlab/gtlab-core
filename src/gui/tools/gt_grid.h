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

    enum PaintOption
    {
        NoPaintOption = 0,
        PaintGrid = 1 << 0,
        PaintAxis = 1 << 1,

        PaintAll  = PaintGrid | PaintAxis,
    };
    using PaintOptions = QFlags<PaintOption>;

    using VisibleAxis = QFlags<Qt::Orientation>;

    /// Constructor. Transfers ownership to view
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGrid(QObject* parent)` instead.")
    explicit GtGrid(QGraphicsView& view);
    explicit GtGrid(QObject* parent = nullptr);
    ~GtGrid();

    /**
     * @brief Sets the spacing between horizontal (major) grid lines
     * @param value Spacing
     */
    void setHSpacing(unsigned value);

    /**
     * @brief Returns the spacing between horizontal (major) grid lines
     * @return Horizontal spacing
     */
    unsigned hSpacing() const;
    /**
     * @brief Sets the spacing between vertical (major) grid lines
     * @param value
     */
    void setVSpacing(unsigned value);
    /**
     * @brief Returns the spacing between vertical (major) grid lines
     * @return Vertical spacing
     */
    unsigned vSpacing() const;

    /** Sets whether the grid should be scaled or not.
        @param val Grid scaling indicator */
    void setScaleGrid(bool val);

    /**
     * @brief Sets which axis should be shown
     * @param show Whether to show the given axis
     * @param orientation Which axis to show
     */
    void setVisibleAxis(bool show = true, VisibleAxis axis = Qt::Horizontal | Qt::Vertical);

    /**
     * @brief Returns which axis is shown, if any.
     * @return which axis is visible
     */
    VisibleAxis visibleAxis() const;

    /**
     * @brief Sets whether the minor grid should be shown.
     * @param Show show minor grid
     */
    void setShowMinorGrid(bool show = true);

    /**
     * @brief Returns whether the minor grid should be shown.
     * @return Whether minor grid is shown
     */
    bool showMinorGrid() const;

    /**
     * @brief Sets the minor horizontal line count
     */
    void setMinorHLineCount(unsigned count);

    /**
     * @brief Sets the minor vertical line count
     */
    void setMinorVLineCount(unsigned count);

    /**
     * @brief Returns the horizontal line count
     * @return Minor horizontal line count
     */
    unsigned minorHLineCount() const;

    /**
     * @brief Returns the vertical line count
     * @return Minor vertical line count
     */
    unsigned minorVLineCount() const;

    /**
     * @brief Sets the major grid line color
     * @param color Major grid line color
     */
    void setLineColor(const QColor& color);
    /**
     * @brief Returns the major grid line color
     * @return Major grid line color
     */
    QColor lineColor() const;

    /**
     * @brief Sets the vertical grid line color
     * @param color Vertical grid line color
     */
    void setMinorLineColor(const QColor& color);
    /**
     * @brief Returns the vertical grid line color
     * @return Vertical grid line color
     */
    QColor minorLineColor() const;

    /**
     * @brief Sets the axis color
     * @param color Axis color
     */
    void setAxisColor(const QColor& color);

    /**
     * @brief Returns the axis color
     * @return Axis color
     */
    QColor axisColor() const;

    /** @brief Paints full grid.
     *  @param painter QPainter pointer
     *  @param rect Scene rect
     */
    virtual void paint(QPainter& painter, const QRectF& rect, PaintOptions options = PaintAll) const;

    /** computeTopLeftGridPoint
     * @param p
     * @return Top left grid Point
     */
    QPointF computeTopLeftGridPoint(const QPointF& p);

    /** computeNearestGridPoint
     * @param p
     * @return Nearest Grid point to p
     */
    QPointF computeNearestGridPoint(const QPointF& p);

    /** Sets current grid scale factor.
     *  @param val Grid scale factor
     */
    void setGridScaleFactor(int val);

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
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setVisible` or `hide` instead.")
    void hideGrid(bool hidden = true)
    {
        setVisible(!hidden);
    }
    void hide();

    /**
     * @brief Shows the grid
     * @param val show indicator
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setVisible` or `show` instead.")
    void showGrid(bool visible = true)
    {
        setVisible(visible);
    }
    void show();

signals:

    /**
     * @brief Emitted if the grid is updated
     */
    GT_DEPRECATED_ATTR(2, 2, "use `visibilityChanged` instead.")
    void update();

    /**
     * @brief Emitted if the grid visibility has changed
     */
    void visibilityChanged(bool visible);

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

protected:
    /** Paints horizontal and vertical grid lines.
        @param painter QPainter pointer
        @param rect Scene rect */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `paint` instead.")
    void paintGridLines(QPainter* painter, const QRectF &rect)
    {
        if (painter) paint(*painter, rect, PaintGrid);
    }

    /** Paints horizontal and vertical axis.
        @param painter QPainter pointer
        @param rect Scene rect */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `paint` instead.")
    void paintAxis(QPainter* painter, const QRectF &rect)
    {
        if (painter) paint(*painter, rect, PaintAxis);
    }

    /** Returns scaled grid width.
        @return Scaled grid width */
    GT_DEPRECATED_REMOVED_IN(2, 2, "No replacement is planned.")
    double getScaledGridWidth();

    /** Returns scaled grid height.
        @return Scaled grid height */
    GT_DEPRECATED_REMOVED_IN(2, 2, "No replacement is planned.")
    double getScaledGridHeight();

public:

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setHSpacing` instead.")
    void setGridWidth(int value)
    {
        return setHSpacing(value);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setVSpacing` instead.")
    void setGridHeight(int value)
    {
        return setVSpacing(value);
    }

    /** Sets number of grid points for one grid rect.
        @param horizontal Number of horizontal grid points
        @param vertical Number of vertical grid points */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setNumberOfGridPoints(int horizontal, int vertical) { }

    /** Sets whether grid points should be enabled or not.
        @param val Gridpoints indicator */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setShowGridPoints(bool val) { }

    /** @brief Sets whether the horizontal axis should be enabled or not.
     *  @param show Whether to show the axis
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `setVisibleAxis` instead.")
    void setShowAxis(bool show);

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setLineColor` instead.")
    void setHorizontalGridLineColor(const QColor& color)
    {
        setLineColor(color);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect, use `setLineColor` instead.")
    void setVerticalGridLineColor(const QColor& color) { }

    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setGridPointColor(const QColor& color) {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `paint` instead.")
    void paintGrid(QPainter* painter, const QRectF& rect)
    {
        if (painter) paint(*painter, rect);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "...?")
    void paintRuler(GtRuler* ruler) const {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "...")
    void setHorizontalRuler(GtRuler* ruler) {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "...")
    void setVerticalRuler(GtRuler* ruler) {}
};

#endif // GTD_GRID_H
