/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORPROPERTYENTITY_H
#define GTCALCULATORPROPERTYENTITY_H

#include <QGraphicsObject>
#include <QPointer>

class GtAbstractProperty;

/**
 * @brief The GtCalculatorPropertyEntity class
 */
class GtCalculatorPropertyEntity : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief GtCalculatorPropertyEntity
     * @param parent
     */
    explicit GtCalculatorPropertyEntity(GtAbstractProperty* prop,
                                        QGraphicsItem* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /**
    * @brief boundingRect
    * @return
    */
    QRectF boundingRect() const override;

private:
    /// property
    QPointer<GtAbstractProperty> m_property;

    /**
     * @brief populateEntity
     */
    void populateEntity();

};

#endif // GTCALCULATORPROPERTYENTITY_H
