/* GTlab - Gas Turbine laboratory
 * Source File: gtd_graphicsview.h
 * copyright 2009-2013 by DLR
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_GRAPHICSVIEW_H
#define GTD_GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGestureEvent>
#include "gt_mdi_exports.h"

class GtGraphicsScene;
class GtGrid;
class GtRuler;

class GT_MDI_EXPORT GtGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    GtGraphicsView(GtGraphicsScene* s, QWidget* parent = 0);
    ~GtGraphicsView();

    /** Returns grid.
        @return Grid pointer */
    GtGrid* grid();

    /** Sets new grid. Old grid is deleted.
        @param grid New grid */
    void setGrid(GtGrid* grid);

    /** Sets new horizontal ruler.
        @param ruler New horizontal ruler */
    void setHorizontalRuler(GtRuler* ruler);

    /** Sets new vertical ruler.
        @param ruler New vertical ruler */
    void setVerticalRuler(GtRuler* ruler);

    /** Sets view scale to given factor in percent.
        @param percentage New zoom factor*/
    void setScalePercentage(qreal percentage);

    /** Repaints ruler. */
    void repaintRuler();

    /**
     * @brief snapItemToGrid
     * @param item
     */
    void snapItemToGrid(QGraphicsItem* item);

    /** Sets maximum zoom factor.
        @param val New maximum zoom factor */
    void setMaximumZoom(qreal val);

    /** Sets minimum zoom factor.
        @param val New minimum zoom factor */
    void setMinimumZoom(qreal val);

public slots:

    void snapToGrid(bool val);

protected:
    /// GtdGraphicsScene
    GtGraphicsScene* m_scene;

    virtual void wheelEvent(QWheelEvent *e);

    virtual void scrollContentsBy(int dx, int dy);

    virtual void drawBackground(QPainter *painter, const QRectF &rect);

    virtual void mouseMoveEvent(QMouseEvent* mouseEvent);

    /** Sets view scale to given factor.
        @param val New Scale factor */
    void setScale(qreal val);

private:
    /// Zoom factor
    qreal m_zoom;

    /// Max zoom factor
    qreal m_maxZoom;

    /// Min zoom factor
    qreal m_minZoom;

    /// Number of scheduled scalings
    int m_numsScalings;

    /// Grid
    GtGrid* m_grid;

    /// Horizontal ruler
    GtRuler* m_hRuler;

    /// Vertical ruler
    GtRuler* m_vRuler;

    /// Switch for "Snap to Grid" Mode
    bool m_snap;

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
    void zoomChanged(qreal);

    /** Signal which is emitted when the mouse position changed */
    void mousePositionChanged(const QPointF);

};

#endif // GTD_GRAPHICSVIEW_H
