/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_graphicsview.h
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
        OwnActiveScene = 1 << 0
    };
    using Options = QFlags<Option>;

    explicit GtGraphicsView(QGraphicsScene* s, QWidget* parent = 0) :
        GtGraphicsView(s, OwnActiveScene, parent)
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

    /** @brief Returns the grid.
      * @return Grid, may be null if no grid is set
      */
    GtGrid* grid();

    /** Sets new horizontal ruler.
        @param ruler New horizontal ruler */
    void setHorizontalRuler(GtRuler* ruler);

    /** Sets new vertical ruler.
        @param ruler New vertical ruler */
    void setVerticalRuler(GtRuler* ruler);

    /** Sets view scale to given factor in percent.
        @param percentage New zoom factor*/
    void setScalePercentage(double percentage);

    /** Repaints ruler. */
    GT_DEPRECATED_REMOVED_IN(2, 2, "No replacement planned.")
    void repaintRuler() {}

    /**
     * @brief snapItemToGrid
     * @param item
     */
    void snapItemToGrid(QGraphicsItem* item);

    /** Sets maximum zoom factor.
        @param val New maximum zoom factor */
    void setMaximumZoom(double val);

    /** Sets minimum zoom factor.
        @param val New minimum zoom factor */
    void setMinimumZoom(double val);

public slots:

    void snapToGrid(bool val);

    void wheelEvent(QWheelEvent *e) override;

    void scrollContentsBy(int dx, int dy) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void paintEvent(QPaintEvent* event) override;

    void mouseMoveEvent(QMouseEvent* mouseEvent) override;

    /** Sets view scale to given factor.
        @param val New Scale factor */
    void setScale(double val);

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /** Smooth zoom animation.
        @param delta Wheel delta */
    void zoomAnimation(int delta);

    /** Returns grid factor.
        @return Grid factor */
    int getGridFactor();

    /** Snaps selected item to the closes grid point
     * @param item selected graphics item
     * @param mousePos mouse position from where the closest grid point is determined
     */
    void snapItemToGrid(QGraphicsItem* item, QPoint mousePos);

private slots:
    /** Called to update view scale. */
    void scalingTime();

    /** Called at animation finish. */
    void animFinished();

signals:
    /** Signal which is emitted when the zoom factor changed */
    void zoomChanged(double);

    /** Signal which is emitted when the mouse position changed */
    void mousePositionChanged(const QPointF);

};

#endif // GT_GRAPHICSVIEW_H
