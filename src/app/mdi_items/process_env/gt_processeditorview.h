/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSEDITORVIEW_H
#define GTPROCESSEDITORVIEW_H

#include "gt_graphicsview.h"

/**
 * @brief The GtProcessEditorView class
 */
class GtProcessEditorView : public GtGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessEditorView
     * @param s
     * @param parent
     */
    GtProcessEditorView(GtGraphicsScene* s, QWidget* parent = 0);

    /**
     * @brief setShowLegend
     * @param val
     */
    void setShowLegend(bool val);

protected:
    /**
     * @brief drawForeground
     * @param painter
     * @param rect
     */
    virtual void drawForeground(QPainter * painter,
                                const QRectF & rect) override;

private:
    /// Show legend indicator
    bool m_showLegend;

};

#endif // GTPROCESSEDITORVIEW_H
