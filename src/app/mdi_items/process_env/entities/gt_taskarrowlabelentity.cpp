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
#include <QDebug>

#include "gt_taskarrowlabelentity.h"
#include "gt_object.h"
#include "gt_datamodel.h"
#include "gt_processfactory.h"
#include "gt_application.h"
#include "gt_palette.h"

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
    painter->setPen(QPen(QColor(70, 70, 70), 1.5));
    painter->setBrush(Qt::white);

    if (gtApp->inDarkMode())
    {
        painter->setPen(QPen(QColor(170, 170, 170), 1.5));
        painter->setBrush(QBrush(GtPalette::basicDarkColor()));
    }

    QRectF br = boundingRect();

    painter->drawRoundedRect(br, 2., 2.);
    painter->drawLine(int(br.width() / 2), 7,
                      int(br.width() / 2), int(br.height() - 7.));
    painter->drawLine(7, int(br.height() / 2),
                      int(br.width() - 7.), int(br.height() / 2));
}

QRectF GtTaskArrowLabelEntity::boundingRect() const
{
    return QRectF(0, 0, 30, 30);
}

void
GtTaskArrowLabelEntity::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
//    qDebug() << "GtTaskArrowLabelEntity::dragEnterEvent";

    const QMimeData* mimeData = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                                                    gtProcessFactory);

    if (obj != Q_NULLPTR)
    {
//        qDebug() << "obj = " << obj->objectName();
//        qDebug() << "uuid = " << obj->uuid();
        delete obj;

        event->setAccepted(true);


//        event->setDropAction(Qt::LinkAction);
//        event->acceptProposedAction();


//        qDebug() << "accepted!";
    }
    else
    {
        event->setAccepted(false);


//        event->setDropAction(Qt::IgnoreAction);
//        event->acceptProposedAction();


//        qDebug() << "ignored!";
    }

    //    QGraphicsTextItem::dragEnterEvent(event);
}

void
GtTaskArrowLabelEntity::dragLeaveEvent(QGraphicsSceneDragDropEvent* /*event*/)
{
//    qDebug() << "GtTaskArrowLabelEntity::dragLeaveEvent";
}

void
GtTaskArrowLabelEntity::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << "GtTaskArrowLabelEntity::dropEvent";

    const QMimeData* mimeData = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                                                    gtProcessFactory);

    if (obj != Q_NULLPTR)
    {
        emit addNewItem(obj);
    }
}

