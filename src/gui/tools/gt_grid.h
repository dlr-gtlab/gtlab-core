/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_grid.h
 *
 *  Created on: 17.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#ifndef GT_GRID_H
#define GT_GRID_H

#include <gt_gui_exports.h>
#include <gt_gridspacing.h>
#include <gt_version.h>

#include <QObject>
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

    /// Painting options. May be used when subclassing the grid to override
    /// how the grid or axis is rendered
    enum PaintOption
    {
        NoPaintOption = 0,
        PaintGrid = 1 << 0,
        PaintAxis = 1 << 1,

        PaintAll  = PaintGrid | PaintAxis,
    };
    using PaintOptions = QFlags<PaintOption>;

    /// Defines how the grid automatically updates when changing the zoom level
    enum ScalingStrategy : unsigned
    {
        /// Grid does not automatically scale to current zoom level. When
        /// zoomed-in or -out too much, grid lines may become too sparse/dense.
        Fixed = 0,
        /// Grid scales in step sizes of 2. Scale changes often. Good default.
        Base2,
        /// Grid scales in step sizes of 10. Natural ticks, but scale changes
        /// least often.
        Base10,
        /// Grid scales in step size of 1, 2, 5, 10, 20, 50, etc. Common in
        /// other tools. Better tick-labels when zoomed-in.
        OneTwoFive,
        /// Default
        DefaultScalingStrategy = OneTwoFive
    };

    using VisibleAxis = QFlags<Qt::Orientation>;

    /// Constructor. Transfers ownership to view
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGrid(QObject* parent)` instead.")
    explicit GtGrid(QGraphicsView& view);
    explicit GtGrid(QObject* parent = nullptr);
    ~GtGrid();

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

    /**
     * @brief Sets both the vertical and horizontal spacing
     * @param value Spacing
     */
    void setSpacing(unsigned value)
    {
        setHSpacing(value);
        setVSpacing(value);
    }

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
     * @brief Sets the device independent pixel distance at which the
     * minor grid should not be drawn, because it is too dense.
     * @param cutoffDensity Cutoff density
     */
    void setMinorGridCutoffDensity(double cutoffDensity);

    /**
     * @brief Returns the device independent pixel distance at which the
     * minor grid is no longer drawn, because it is too dense.
     * @return Cutoff density
     */
    double minorGridCutoffDensity() const;

    /**
     * @brief Sets both the vertical and horizontal subdivisions
     * @param count Divisions
     */
    void setSubdivisions(unsigned count)
    {
        setHSubdivisions(count);
        setVSubdivisions(count);
    }

    /**
     * @brief Sets the minor horizontal subdivisions
     * @param count Divisions
     */
    void setHSubdivisions(unsigned count);

    /**
     * @brief Sets the minor vertical subdivisions
     * @param count Divisions
     */
    void setVSubdivisions(unsigned count);

    /**
     * @brief Returns the horizontal subdivisions
     * @return Minor horizontal subdivisions
     */
    unsigned hSubdivisions() const;

    /**
     * @brief Returns the vertical subdivisions
     * @return Minor vertical subdivisions
     */
    unsigned vSubdivisions() const;

    /**
     * @brief Returns the current spacing for the major grid
     * @return Current grid scaling
     */
    GtGridSpacing currentGridSpacing() const;

    /**
     * @brief Returns the current spacing for the minor grid
     * @return Current minor grid scaling
     */
    GtGridSpacing currentMinorGridSpacing() const;

    /**
     * @brief Returns the grid spacing scaled for the given zoom level using
     * the current scaling strategy
     * @param zoom Zoom level (usually QTransform::m11)
     * @return Scaled grid spacing
     */
    GtGridSpacing scaledGridSpacing(double zoom) const;

    /**
     * @brief Returns the minor grid spacing scaled for the given zoom level
     * using the current scaling strategy
     * @param zoom Zoom level (usually QTransform::m11)
     * @return Scaled grid spacing
     */
    GtGridSpacing scaledMinorGridSpacing(double zoom) const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `setScalingStrategy` instead.")
    void setScaleGrid(bool enable)
    {
        setScalingStrategy(enable ?  Base2 :  Fixed);
    }

    /**
     * @brief Sets the scaling strategy, allowing the grid to automatically
     * update based on the current zoom level.
     * @param val Grid scaling strategy
     */
    void setScalingStrategy(ScalingStrategy strategy);

    /**
     * @brief Returns the current grid scaling strategy
     * @return Scaling strategy
     */
    ScalingStrategy scalingStrategy() const;

    /**
     * @brief Sets the visibility of the grid
     * @param visible Whether the grid should be visible or hidden
     */
    Q_INVOKABLE void setVisible(bool visible = true);

    /// hides the grid and axis
    Q_INVOKABLE void hide();

    /// shows the grid and axis
    Q_INVOKABLE void show();

    /**
     * @brief Returns whether the grid is visible
     * @return Is grid visible
     */
    bool isVisible() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setShowGrid` or `setVisible` instead.")
    Q_INVOKABLE void hideGrid(bool hidden = true)
    {
        showGrid(!hidden);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setShowGrid` or `setVisible` instead.")
    Q_INVOKABLE void showGrid(bool visible = true)
    {
        setVisible(visible); // not using setShowGrid for backwards compatibility
    }
    /**
     * @brief Sets whether the grid should be enabled or not. Independent of
     * show axis.
     * @param show Whether to show the axis
     */
    Q_INVOKABLE void setShowGrid(bool show);

    /**
     * @brief Returns whether the grid is visible
     * @return Is grid shown
     */
    bool showGrid() const;

    /**
     * @brief Sets whether the axis should be enabled or not. Independent of
     * show grid.
     * @param show Whether to show the axis
     */
    Q_INVOKABLE void setShowAxis(bool show);

    /**
     * @brief Returns whether the axis is visible
     * @return Is axis shown
     */
    bool showAxis() const;

    /**
     * @brief Sets which axis should be shown
     * @param show Whether to show the given axis
     * @param orientation Which axis to show
     */
    void setVisibleAxis(VisibleAxis axis = Qt::Horizontal | Qt::Vertical);

    /**
     * @brief Returns which axis is shown, if any.
     * @return which axis is visible
     */
    VisibleAxis visibleAxis() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setLineColor` instead.")
    void setHorizontalGridLineColor(const QColor& color)
    {
        setLineColor(color);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setLineColor` instead.")
    void setVerticalGridLineColor(const QColor& color)
    {
        setLineColor(color);
    }

    /**
     * @brief Sets the pen for the major grid lines
     * @param pen Pen
     */
    void setMajorPen(QPen pen);

    /**
     * @brief Returns the pen for the major grid lines
     * @return pen Pen
     */
    QPen majorPen() const;

    /**
     * @brief Sets the major grid line color. Overrides the pen color.
     * @param color Major grid line color
     */
    void setLineColor(const QColor& color);

    /**
     * @brief Returns the major grid line color
     * @return Major grid line pen color
     */
    QColor lineColor() const;

    /**
     * @brief Sets the pen for the minor grid lines
     * @param pen Pen
     */
    void setMinorPen(QPen pen);

    /**
     * @brief Returns the pen for the minor grid lines
     * @return pen Pen
     */
    QPen minorPen() const;

    /**
     * @brief Sets the vertical grid line color. Overrides the pen color.
     * @param color Vertical grid line color
     */
    void setMinorLineColor(const QColor& color);

    /**
     * @brief Returns the vertical grid line color
     * @return Vertical grid line pen color
     */
    QColor minorLineColor() const;

    /**
     * @brief Sets the pen for the axis
     * @param pen Pen
     */
    void setAxisPen(QPen pen);

    /**
     * @brief Returns the pen for the axis
     * @return pen Pen
     */
    QPen axisPen() const;

    /**
     * @brief Sets the axis color. Overrides the pen color.
     * @param color Axis color
     */
    void setAxisColor(const QColor& color);

    /**
     * @brief Returns the axis pen color.
     * @return Axis color
     */
    QColor axisColor() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `GtGraphicsView::connectHorizontalRuler` instead.")
    void setHorizontalRuler(GtRuler* ruler) {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `GtGraphicsView::connectVerticalRuler` instead.")
    void setVerticalRuler(GtRuler* ruler) {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `GtRuler::paint` instead.")
    void paintRuler(GtRuler* ruler) const {}

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `paint` instead.")
    void paintGrid(QPainter* painter, const QRectF& rect)
    {
        if (painter) paint(*painter, rect);
    }

    /**
     * @brief Paints the full grid and axis.
     * @param painter QPainter pointer
     * @param rect Scene rect
     * @param options Paint options
     */
    virtual void paint(QPainter& painter,
                       const QRectF& rect,
                       PaintOptions options = PaintAll);

    /**
     * @brief Returns the top left major grid point
     * @param p
     * @return Top left major grid point
     */
    QPointF computeTopLeftGridPoint(const QPointF& p);

    /**
     * @brief Returns the top left minor grid point
     * @param p Point to clamp
     * @return Top left minor grid point
     */
    QPointF computeTopLeftMinorGridPoint(const QPointF& p);

    /**
     * @brief Returns the nearest major grid point
     * @param p Point to clamp
     * @return Nearest major grid point to p
     */
    QPointF computeNearestGridPoint(const QPointF& p);

    /**
     * @brief Returns the nearest minor grid point
     * @param p Point to clamp
     * @return Nearest minor grid point to p
     */
    QPointF computeNearestMinorGridPoint(const QPointF& p);

    /**
     * @brief Sets current grid scale factor.
     * @param val Grid scale factor
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Determined when painting, no replacement is planned")
    void setGridScaleFactor(int val) {}

    /**
     * @brief Sets number of grid points for one grid rect.
     * @param horizontal Number of horizontal grid points
     * @param vertical Number of vertical grid points
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setNumberOfGridPoints(int horizontal, int vertical) { }

    /**
     * @brief Sets whether grid points should be enabled or not.
     * @param val Gridpoints indicator
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setShowGridPoints(bool val) { }

    GT_DEPRECATED_REMOVED_IN(2, 2, "Function has no effect. No replacement is planned.")
    void setGridPointColor(const QColor& color) {}

signals:

    GT_DEPRECATED_ATTR(2, 2, "use `updated` instead.")
    void update();

    /**
     * @brief Emitted if the grid settings updated
     */
    void updated();

protected:

    /**
     * @brief Paints horizontal and vertical grid lines.
     * @param painter QPainter pointer
     * @param rect Scene rect
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `paint` instead.")
    void paintGridLines(QPainter* painter, const QRectF &rect)
    {
        if (painter) paint(*painter, rect, PaintGrid);
    }

    /**
     * @brief Paints horizontal and vertical axis.
     * @param painter QPainter pointer
     * @param rect Scene rect
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `paint` instead.")
    void paintAxis(QPainter* painter, const QRectF &rect)
    {
        if (painter) paint(*painter, rect, PaintAxis);
    }

    /// Returns scaled grid width.
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `currentGridSpacing` instead.")
    double getScaledGridWidth() const
    {
        return currentGridSpacing().hSpacing;
    }

    /// Returns scaled grid height.
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `currentGridSpacing` instead.")
    double getScaledGridHeight() const
    {
        return currentGridSpacing().vSpacing;
    }

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GT_GRID_H
