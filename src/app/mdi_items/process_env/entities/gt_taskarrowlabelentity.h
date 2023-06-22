/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    explicit GtTaskArrowLabelEntity(QGraphicsItem* parent = nullptr);

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /**
    * @brief boundingRect
    * @return
    */
    QRectF boundingRect() const override;

protected:
    /**
     * @brief dragEnterEvent
     * @param event
     */
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;

    /**
     * @brief dragLeaveEvent
     * @param event
     */
    void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;

    /**
     * @brief dropEvent
     * @param event
     */
    void dropEvent(QGraphicsSceneDragDropEvent* event) override;

signals:
    void addNewItem(GtObject* obj);

};

#endif // GTTASKARROWLABELENTITY_H
