/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    explicit GtProcessEditorView(GtGraphicsScene* s, QWidget* parent = nullptr);

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
    void drawForeground(QPainter * painter, const QRectF & rect) override;

private:
    /// Show legend indicator
    bool m_showLegend;

};

#endif // GTPROCESSEDITORVIEW_H
