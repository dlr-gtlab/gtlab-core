/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>

#include "gt_calculatorpropertyentity.h"
#include "gt_abstractproperty.h"
#include "gt_propertyportentity.h"

GtCalculatorPropertyEntity::GtCalculatorPropertyEntity(GtAbstractProperty* prop,
                                                       QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_property(prop)
{
    populateEntity();
}

void
GtCalculatorPropertyEntity::paint(QPainter* painter,
                                  const QStyleOptionGraphicsItem* /*option*/,
                                  QWidget* /*widget*/)
{
    painter->save();

    painter->drawText(QPoint(15, 14), m_property->objectName());

    painter->restore();
}

QRectF
GtCalculatorPropertyEntity::boundingRect() const
{
    Q_ASSERT(parentItem() == Q_NULLPTR);

    const QRectF rect = parentItem()->boundingRect();

    return QRectF(rect.x(), 0, rect.width(), 20);
}

void
GtCalculatorPropertyEntity::populateEntity()
{
    GtPropertyPortEntity* out = new GtPropertyPortEntity(this);

    const QRectF rect = boundingRect();
    out->setPos(rect.width(), rect.height() / 2);

    if (!m_property->isReadOnly())
    {
        GtPropertyPortEntity* in = new GtPropertyPortEntity(this);
        in->setPos(0, rect.height() / 2);
    }
}

