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
    enum class ScalingStrategy : unsigned
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

    /// Flags to enable the axis along a specific orientation
    using ActiveAxis = QFlags<Qt::Orientation>;

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
    void setSpacing(unsigned spacing)
    {
        setHSpacing(spacing);
        setVSpacing(spacing);
    }

    /**
     * @brief Sets the spacing between vertical (intersecting the x-axis)
     * major grid lines
     * @param value Spacing
     */
    void setHSpacing(unsigned spacing);

    /**
     * @brief Returns the spacing between vertical (intersecting the x-axis)
     * major grid lines
     * @return Horizontal spacing
     */
    unsigned hSpacing() const;

    /**
     * @brief Sets the spacing between horizontal (intersecting the y-axis)
     * major grid lines
     * @param value
     */
    void setVSpacing(unsigned spacing);

    /**
     * @brief Returns the spacing between horizontal (intersecting the y-axis)
     * major grid lines
     * @return Vertical spacing
     */
    unsigned vSpacing() const;

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
     * @brief Sets both the vertical and horizontal subdivisions defining the
     * minor grid
     * @param count Divisions
     */
    void setSubdivisions(unsigned count)
    {
        setHSubdivisions(count);
        setVSubdivisions(count);
    }

    /**
     * @brief Sets the number of subdivisions between vertical major grid
     * lines defining the minor grid
     * @param value Divisions
     */
    void setHSubdivisions(unsigned count);

    /**
     * @brief Returns the number of subdivisions between vertical major grid
     * lines
     * @return Minor subdivisions between vertical major grid lines
     */
    unsigned hSubdivisions() const;

    /**
     * @brief Sets the number of subdivisions between horizontal major grid
     * lines defining the minor grid
     * @param value Divisions
     */
    void setVSubdivisions(unsigned count);

    /**
     * @brief Returns the number of subdivisions between horizontal major grid
     * lines
     * @return Minor subdivisions between horizontal major grid lines
     */
    unsigned vSubdivisions() const;

    /**
     * @brief Returns the current spacing for the major grid for the last
     * zoom level
     * @return Current grid scaling for the last zoom level
     */
    GtGridSpacing scaledGridSpacing() const;

    /**
     * @brief Returns the current spacing for the minor grid for the last
     * zoom level
     * @return Current minor grid scaling for the last zoom level
     */
    GtGridSpacing scaledMinorGridSpacing() const;

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
        setScalingStrategy(enable ? ScalingStrategy::Base2 :
                                    ScalingStrategy::Fixed);
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
     * @brief Sets the visibility of this objects, overriding the grid and the
     * axis. Major grid, minor grid and axis remember their previous state.
     * By default the object is visible.
     * @param visible Whether the object should be visible or hidden
     */
    Q_INVOKABLE void setVisible(bool visible = true);

    /// hides grid and axis
    Q_INVOKABLE void hide();

    /// shows grid and axis
    Q_INVOKABLE void show();

    /**
     * @brief Returns whether the object (grid and axis) is visible.
     * @return Is the object visible
     */
    bool isVisible() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `enableGrid` or `setVisible` instead.")
    Q_INVOKABLE void hideGrid(bool hidden = true)
    {
        setVisible(!hidden); // not using setGridVisible for backwards compatibility
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `enableGrid` or `setVisible` instead.")
    Q_INVOKABLE void showGrid(bool visible = true)
    {
        setVisible(visible); // not using setGridVisible for backwards compatibility
    }

    /**
     * @brief Sets whether the grid should be enabled or not. By default
     * the grid is enabled.
     * Note: minor grid is only shown if the grid is enabled.
     * @param enable Whether to enable the grid
     */
    Q_INVOKABLE void enableGrid(bool enable = true);

    /**
     * @brief Returns whether the grid is actually visible
     * @return Is grid actually visible
     */
    bool isGridVisible() const;

    /**
     * @brief Returns whether the grid is enabled
     * @return Is grid enabled
     */
    bool isGridEnabled() const;

    /**
     * @brief Sets whether the minor grid should be activated. By default
     * the minor grid is enabled.
     * @param enable Whether to enable the minor grid
     */
    void enableMinorGrid(bool enable = true);

    /**
     * @brief Returns whether the minor grid is actually shown.
     * Does not reflect whether the minor grid is too dense to be shown
     * (see `setMinorGridCutoffDensity`)
     * Note:Minor grid is only shown if the grid is enabled.
     * @return Whether minor grid is actually visible
     */
    bool isMinorGridVisible() const;

    /**
     * @brief Returns whether the minor grid is enabled.
     * @return Whether minor grid is enabled
     */
    bool isMinorGridEnabled() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setActiveAxis` instead.")
    Q_INVOKABLE void setShowAxis(bool show)
    {
        // not using setAxisVisible for backwards compatibility
        setActiveAxis(!show ? ActiveAxis{} : Qt::Horizontal);
    }

    /**
     * @brief Returns whether any axis is actually visible.
     * @return Is any axis visible
     */
    bool isAxisVisible() const;

    /**
     * @brief Sets which axis should be enabled. By default no axis is visible.
     * @param axis Orientations of the axis to enable
     */
    void setActiveAxis(ActiveAxis axis);

    /**
     * @brief Returns which axis is active.
     * @return which axis is active
     */
    ActiveAxis activeAxis() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setLineColor` instead.")
    void setHorizontalGridLineColor(const QColor& color)
    {
        setMajorLineColor(color);
    }

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setLineColor` instead.")
    void setVerticalGridLineColor(const QColor& color)
    {
        setMajorLineColor(color);
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
    void setMajorLineColor(const QColor& color);

    /**
     * @brief Returns the major grid line color
     * @return Major grid line pen color
     */
    QColor majorLineColor() const;

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
     * @brief Explicitly sets the current zoom level. Note: current zoom level
     * is updated automatically when painting. Only needed when not painintg
     * the grid. Usually QTransform::m11.
     * @param zoom Current zoom level
     */
    void setCurrentZoom(double zoom);

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
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `scaledGridSpacing` instead.")
    double getScaledGridWidth() const
    {
        return scaledGridSpacing().hSpacing;
    }

    /// Returns scaled grid height.
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `scaledGridSpacing` instead.")
    double getScaledGridHeight() const
    {
        return scaledGridSpacing().vSpacing;
    }

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GT_GRID_H
