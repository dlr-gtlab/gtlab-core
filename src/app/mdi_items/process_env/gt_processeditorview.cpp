/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 20.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processeditorview.h"

GtProcessEditorView::GtProcessEditorView(GtGraphicsScene* s, QWidget* parent) :
    GtGraphicsView(s, parent),
    m_showLegend(false)
{

}

void
GtProcessEditorView::setShowLegend(bool val)
{
    m_showLegend = val;
    viewport()->update();
}

void
GtProcessEditorView::drawForeground(QPainter* painter, const QRectF& /*rect*/)
{
    if (m_showLegend)
    {
        painter->save();

        painter->resetTransform();

        QSize s = viewport()->size();

        QPoint p(5, s.height() - 45);

        painter->drawText(p, tr("Legend:"));

        p.setX(10);
        p.setY(s.height() - 30);
        painter->setBrush(QBrush(QColor(235, 145, 40)));
        painter->drawEllipse(p, 4, 4);
        painter->drawText(QPoint(p.x() + 10, p.y() + 5),
                          QStringLiteral("Datamodel Port"));

        p.setX(10);
        p.setY(s.height() - 10);
        painter->setBrush(QBrush(QColor(90, 90, 150)));
        painter->drawEllipse(p, 4, 4);
        painter->drawText(QPoint(p.x() + 10, p.y() + 5),
                          QStringLiteral("Double Property Port"));

        painter->restore();
    }
}

