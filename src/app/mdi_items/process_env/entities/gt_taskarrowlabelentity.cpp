/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QGraphicsSceneDragDropEvent>

#include "gt_taskarrowlabelentity.h"
#include "gt_object.h"
#include "gt_datamodel.h"
#include "gt_processfactory.h"
#include "gt_colors.h"

GtTaskArrowLabelEntity::GtTaskArrowLabelEntity(QGraphicsItem* parent) :
    QGraphicsObject(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemIsMovable, false);

//    setFont(QFont("Helvetica", 12));

//    setPlainText(QStringLiteral("+"));

    setAcceptDrops(true);
}

void
GtTaskArrowLabelEntity::paint(QPainter* painter,
                              const QStyleOptionGraphicsItem* /*option*/,
                              QWidget* /*widget*/)
{
    gt::gui::color::setPaintertoGray(painter);

    QRectF br = boundingRect();

    painter->drawRoundedRect(br, 2., 2.);
    painter->drawLine(int(br.width() / 2), 7,
                      int(br.width() / 2), int(br.height() - 7.));
    painter->drawLine(7, int(br.height() / 2),
                      int(br.width() - 7.), int(br.height() / 2));
}

QRectF
GtTaskArrowLabelEntity::boundingRect() const
{
    return {0, 0, 30, 30};
}

void
GtTaskArrowLabelEntity::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                                                    gtProcessFactory);

    if (obj)
    {
        delete obj;

        event->setAccepted(true);


//        event->setDropAction(Qt::LinkAction);
//        event->acceptProposedAction();
    }
    else
    {
        event->setAccepted(false);

//        event->setDropAction(Qt::IgnoreAction);
//        event->acceptProposedAction();

    }

    //    QGraphicsTextItem::dragEnterEvent(event);
}

void
GtTaskArrowLabelEntity::dragLeaveEvent(QGraphicsSceneDragDropEvent* /*event*/)
{

}

void
GtTaskArrowLabelEntity::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                                                    gtProcessFactory);

    if (obj)
    {
        emit addNewItem(obj);
    }
}

