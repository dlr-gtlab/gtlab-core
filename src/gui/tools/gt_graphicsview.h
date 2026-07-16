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
        DestroyActiveSceneOnDeletion = 1 << 0
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
     * @brief Sets the view zoom level to the given factor in percent.
     * @param percentage New zoom factor
     */
    void setScalePercentage(double percentage)
    {
        setZoomPercentage(percentage);
    }

    /**
     * @brief Sets the view zoom level to the given factor in percent.
     * @param percentage New zoom factor
     */
    void setZoomPercentage(double percentage);

    /**
     * @brief Returns the current zoom level in percent.
     * @return Zoom level in percent
     */
    double zoomPercentage() const;

    /**
     * @brief Returns the current zoom level
     * @return Zoom level
     */
    double zoom() const;

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

    /**
     * @brief snapItemToGrid
     * @param item
     */
    void snapItemToGrid(QGraphicsItem* item);

public slots:

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setSnapToGrid` instead.")
    void snapToGrid(bool enable) { setSnapToGrid(enable); }

    /**
     * @brief Sets whether items are snapped to the grid when moving
     * @param enable Snap to grid indicator
     */
    void setSnapToGrid(bool enable);

    /**
     * @brief Sets the snap to grid threshold. The threshold is used when moving
     * items by the cursor.
     * @param threshold Snap to grid threshold. Use 0 to disable the threshold.
     */
    void setSnapToGridThreshold(double threshold);

    /**
     * @brief Sets view scale to given factor.
     * @param val New Scale factor
     */
    void setScale(double scale);

signals:

    /// Signal which is emitted when the zoom factor changed
    void zoomChanged(double);

    /// Signal which is emitted when the mouse position changed
    void mousePositionChanged(const QPointF);

protected:

    void wheelEvent(QWheelEvent *e) override;

    void scrollContentsBy(int dx, int dy) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* mouseEvent) override;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief Smooth zoom animation.
     * @param delta Wheel delta
     */
    void zoomAnimation(int delta);

    /**
     * @brief Snaps selected item to the closest grid point.
     * @param item Selected graphics item
     * @param mousePos Mouse position from where the closest grid point is determined
     */
    void snapItemToGrid(QGraphicsItem* item, QPoint mousePos);

private slots:

    /// Called to update view scale.
    void scalingTime();

    /// Called at animation finish.
    void animFinished();
};

#endif // GT_GRAPHICSVIEW_H
