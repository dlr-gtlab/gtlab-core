/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKARROWLABELENTITY_H
#define GTTASKARROWLABELENTITY_H

#include <QGraphicsTextItem>

class GtObject;

/**
 * @brief The GtTaskArrowLabelEntity class
 */
class GtTaskArrowLabelEntity : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief GtTaskArrowLabelEntity
     * @param parent
     */
    GtTaskArrowLabelEntity(QGraphicsItem* parent = Q_NULLPTR);

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) Q_DECL_OVERRIDE;

    /**
    * @brief boundingRect
    * @return
    */
    virtual QRectF boundingRect() const Q_DECL_OVERRIDE;

protected:
    /**
     * @brief dragEnterEvent
     * @param event
     */
    virtual void dragEnterEvent(
            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief dragLeaveEvent
     * @param event
     */
    virtual void dragLeaveEvent(
            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief dropEvent
     * @param event
     */
    virtual void dropEvent(
            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

signals:
    void addNewItem(GtObject* obj);

};

#endif // GTTASKARROWLABELENTITY_H
