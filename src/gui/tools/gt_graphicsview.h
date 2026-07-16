/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_graphicsview.h
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#ifndef GT_GRAPHICSVIEW_H
#define GT_GRAPHICSVIEW_H

#include <gt_gui_exports.h>
#include <gt_version.h>

#include <QGraphicsView>
#include <QGestureEvent>

#include <memory>

class QGraphicsScene;
class GtGrid;
class GtRuler;

class GT_GUI_EXPORT GtGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    enum Option : int
    {
        /// Nop option
        NoOption = 0,
        /// Indicates that the scene, the view is currently assigned to,
        /// is deleted, if the view is deleted.
        /// (default for backwards compatibility,
        ///  normally QGraphicsScene is NOT owned by a QGraphicsView)
        DestroyActiveSceneOnDeletion = 1 << 0,
        /// Disables smooth zooming when using the mouse wheel
        NoSmoothZoom = 1 << 1,
    };
    using Options = QFlags<Option>;

    explicit GtGraphicsView(QGraphicsScene* s, QWidget* parent = 0) :
        GtGraphicsView(s, DestroyActiveSceneOnDeletion, parent)
    { }
    explicit GtGraphicsView(QGraphicsScene* s, Options options, QWidget* parent = 0);
    explicit GtGraphicsView(Options options, QWidget* parent = 0);

    ~GtGraphicsView() override;

    /**
     * @brief Sets the options of this view.
     * @param options Options
     */
    void setOptions(Options options);
    /**
     * @brief Overrides the given option.
     * @param option Option
     * @param enabled Whether the option is enabled
     */
    void setOption(Option option, bool enabled = true);
    /**
     * @brief Returns the options of this view
     * @return Options
     */
    Options options() const;

    /**
     *  @brief Sets the new grid. Takes ownership of the grid.
     *  Old grid is deleted.
     *  @param grid New grid
     */
    void setGrid(GtGrid* grid);

    /**
     * @brief Returns the grid.
     * @return Grid, may be null if no grid is set
     */
    GtGrid* grid();

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `connectHorizontalRuler` instead.")
    void setHorizontalRuler(GtRuler* ruler)
    {
        connectHorizontalRuler(ruler);
    }

    /**
     * @brief Configures the view to use the new horizontal ruler. Does not take
     * ownership.
     * @param ruler New horizontal ruler
     */
    void connectHorizontalRuler(GtRuler* ruler);

    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `connectVerticalRuler` instead.")
    void setVerticalRuler(GtRuler* ruler)
    {
        connectVerticalRuler(ruler);
    }

    /**
     * @brief Configures the view to use the new vertical ruler. Does not take
     * ownership.
     * @param ruler New vertical ruler
     */
    void connectVerticalRuler(GtRuler* ruler);

    /// Repaints ruler.
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `GtRuler::paint` instead.")
    void repaintRuler() {}

    /**
     * @brief Sets view scale to given factor.
     * Same as setZoom.
     * @param val New Scale factor
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `zoomBy` instead.")
    Q_INVOKABLE void setScale(double scale)
    {
        zoomBy(scale);
    }

    /**
     * @brief Sets the view zoom level to the given factor in percent.
     * Same as setZoomPercentage.
     * @param percentage New zoom factor
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use zoomByPercentage instead.")
    Q_INVOKABLE void setScalePercentage(double percentage)
    {
        zoomByPercentage(percentage);
    }

    /**
     * @brief Returns the current zoom level
     * @return Zoom level
     */
    double zoom() const;

    /**
     * @brief Returns the current zoom level in percent.
     * @return Zoom level in percent
     */
    double zoomPercentage() const;

    /**
     * @brief Sets the new zoom level.
     * @param scale New zoom level
     */
    Q_INVOKABLE void zoomBy(double scale);

    /**
     * @brief Sets the view zoom level to the given factor in percent.
     * @param percentage New zoom factor
     */
    Q_INVOKABLE void zoomByPercentage(double percentage);

    void animateZoomTowards(const QPointF& scenePos, double factor);

    /**
     * @brief Sets maximum zoom factor.
     * @param val New maximum zoom factor
     */
    void setMaximumZoom(double val);

    /**
     * @brief Sets minimum zoom factor.
     * @param val New minimum zoom factor
     */
    void setMinimumZoom(double val);

    /**
     * @brief Returns whether items are snapped to the grid when moving
     * @return Snap to grid indicator
     */
    bool snapToGrid() const;

    /**
     * @brief Returns the snap to grid threshold. The threshold is used when
     * moving items by the cursor.
     * @return Snap to grid threshold
     */
    bool snapToGridThreshold() const;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setSnapToGrid` instead.")
    Q_INVOKABLE void snapToGrid(bool enable) { setSnapToGrid(enable); }

    /**
     * @brief Sets whether items are snapped to the grid when moving
     * @param enable Snap to grid indicator
     */
    Q_INVOKABLE void setSnapToGrid(bool enable);

    /**
     * @brief Sets the snap to grid threshold. The threshold is used when moving
     * items by the cursor. Threshold value is in the view's coordinate system.
     * @param threshold Snap to grid threshold. Use 0 to disable the threshold.
     */
    Q_INVOKABLE void setSnapToGridThreshold(double threshold);

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `snapItemToGrid(QGraphicsItem&)` instead.")
    Q_INVOKABLE void snapItemToGrid(QGraphicsItem* item)
    {
        if (item) snapItemToGrid(*item);
    }

    /**
     * @brief Snaps the item to the nearest grid point, regardless of the
     * threshold snapping distance
     * @param item
     */
    Q_INVOKABLE void snapItemToGrid(QGraphicsItem& item);

    /// "overrides" QGraphicsView::scale method to react to changes to scale
    void scale(double dx, double dy);

signals:

    /// Signal which is emitted when the zoom factor changed
    void zoomChanged(double);

    /// Signal which is emitted when the mouse position changed (scene position)
    void mousePositionChanged(QPointF);

protected:

    void wheelEvent(QWheelEvent *e) override;

    void scrollContentsBy(int dx, int dy) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* mouseEvent) override;

private:

    // hide and expose own scale function
    using QGraphicsView::scale;
    // not supported, may cause artifacts with grid or rulers
    using QGraphicsView::rotate;
    // not supported, may cause artifacts with grid or rulers
    using QGraphicsView::shear;

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    void applyZoomAnimation(double scale);

    /**
     * @brief Snaps selected item to the closest grid point.
     * @param item Selected graphics item
     * @param mousePos Mouse position from where the closest grid point is determined
     */
    void snapItemToGrid(QGraphicsItem& item, QPoint mousePos);
};

#endif // GT_GRAPHICSVIEW_H
