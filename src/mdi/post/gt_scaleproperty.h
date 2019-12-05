/* GTlab - Gas Turbine laboratory
 * Source File: gt_scaleproperty.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#ifndef GTSCALEPROPERTY_H
#define GTSCALEPROPERTY_H

#include "gt_mdi_exports.h"


#include "gt_object.h"

/**
 * @brief The ScaleProperty class
 * Scale options for xy-plot
 */
class GT_MDI_EXPORT GtScaleProperty : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(bool custom READ custom WRITE setCustom)
    Q_PROPERTY(double max READ max WRITE setMax)
    Q_PROPERTY(double min READ min WRITE setMin)
public:

    /**
     * @brief ScaleProperty
     */
    Q_INVOKABLE GtScaleProperty();

    /**
     * @brief custom
     * Getter function for membervariable m_custom
     * @return
     */
    bool custom() const;

    /**
     * @brief setCustom
     * Setter function for membervariable m_custom
     * @param custom
     */
    void setCustom(bool custom);

    /**
     * @brief max
     * Getter function for membervariable m_max
     * @return
     */
    double max() const;

    /**
     * @brief setMax
     * Setter function for membervariable m_custom
     * @param max
     */
    void setMax(double max);

    /**
     * @brief min
     * Getter function for membervariable m_min
     * @return
     */
    double min() const;

    /**
     * @brief setMin
     * Setter function for membervariable m_min
     * @param min
     */
    void setMin(double min);

private:

    /// flag if custom scaling is active or not
    bool m_custom;

    double m_max;

    double m_min;

};


#endif // GTSCALEPROPERTY_H
