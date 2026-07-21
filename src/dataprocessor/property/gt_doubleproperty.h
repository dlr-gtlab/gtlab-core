/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_doubleproperty.h
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTP_DOUBLEPARAM_H
#define GTP_DOUBLEPARAM_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"
#include "gt_boundaries.h"

/**
 * @brief The GtDoubleProperty class
 */
class GT_DATAMODEL_EXPORT GtDoubleProperty : public GtProperty<double>
{
    Q_OBJECT

    Q_PROPERTY(double get READ getVal WRITE setVal)

public:
    using GtProperty<double>::operator=;

    enum BoundType
    {
        BoundLow,
        BoundHigh
    };

    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtDoubleProperty(const QString& ident,
                                 const QString& name);
    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     * @param brief
     */
    Q_INVOKABLE GtDoubleProperty(const QString& ident, const QString& name,
                                 const QString& brief);
    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param value
     */
    GtDoubleProperty(const QString& ident,
                     const QString& name,
                     const QString& brief,
                     const GtUnit::Category& unitCategory,
                     const double& value = 0.0);

    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param lowSideBoundary
     * @param highSideBoundary
     * @param value
     */
    [[deprecated("Use function with GtDoubleProperty::Boundaries definition instead")]]
    GtDoubleProperty(const QString& ident,
                     const QString& name,
                     const QString& brief,
                     const GtUnit::Category& unitCategory,
                     const double lowSideBoundary,
                     const double highSideBoundary,
                     const double& value = 0.0);

    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param boundType
     * @param boundary
     * @param value
     */
    [[deprecated("Use function with GtDoubleProperty::Boundaries definition instead")]]
    GtDoubleProperty(const QString& ident,
                     const QString& name,
                     const QString& brief,
                     const GtUnit::Category& unitCategory,
                     GtDoubleProperty::BoundType boundType,
                     const double boundary,
                     const double& value = 0.0);

    /**
     * @brief GtDoubleProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param bounds
     * @param value
     */
    GtDoubleProperty(const QString& ident,
                     const QString& name,
                     const QString& brief,
                     const GtUnit::Category& unitCategory,
                     gt::Boundaries<double> bounds,
                     const double& value = 0.0);

    // operator overloads
    void operator+=(double const& b);
    void operator-=(double const& b);
    void operator*=(double const& b);
    void operator/=(double const& b);


    /**
     * @brief valueToVariant
     * @return
     */
    QVariant valueToVariant(const QString& unit,
                            bool* success = 0) const override;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(const QVariant& val,
                             const QString& unit) override;

    /**
     * @brief Returns value of low side boundary. 0.0 if no lower boundary is
     * set.
     * @return Low side boundary.
     */
    double lowSideBoundary() const;

    /**
     * @brief Returns value of high side boundary. 0.0 if no higher boundary is
     * set.
     * @return High side boundary.
     */
    double highSideBoundary() const;

    /**
     * @brief initialValue of double property (0.0 if not specified)
     * @return
     */
    double initialValue() const;

    /**
     * @brief lowBoundActive
     * @return true if low bound value for the property is active
     */
    bool lowSideBoundaryActive() const;

    /**
     * @brief highBoundActive
     * @return true if high bound value for the property is active
     */
    bool highSideBoundaryActive() const;

protected:
    /// Perform low boundary check flag (true=perform check)
    bool m_boundsCheckFlagLow;

    /// Perform high boundary check flag (true=perform check)
    bool m_boundsCheckFlagHi;

    /// Low side boundary
    double m_boundLo;

    /// High side boundary
    double m_boundHi;

    /**
     * @brief convertFrom
     * @param value
     * @param unit
     * @param success
     * @return
     */
    double convertFrom(const double& value, const QString& unit,
                       bool* success = 0) override;

    /**
     * @brief convertTo
     * @param unit
     * @param success
     * @return
     */
    double convertTo(const QString& unit,
                     bool* success = 0) const override;

    /**
     * @brief validateValue
     * @param value
     * @param unit
     * @return
     */
    bool validateValue(const double& value) override;

};

namespace gt
{

/**
 * @brief Creates a property factory for doubles with a default value
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeDoubleProperty(double value);

/**
 * @brief Creates a property factory for doubles with a default value and a unit
 * @param name - name in the GUI
 * @param brief - description for tool tip in GUI
 * @param unitCategory - category for the GTlab unit system
 * @param value Default - value
 * @return function factory for usage in container definitions
 */
GT_DATAMODEL_EXPORT
    gt::PropertyFactoryFunction makeDoubleProperty(const QString& name,
                       const QString& brief,
                       const GtUnit::Category& unitCategory,
                       const double& value = 0.0);

/**
 * @brief Creates a property factory for doubles with a default value, a unit
 * and a low and high side boundary
 * @param name - name in the GUI
 * @param brief - description for tool tip in GUI
 * @param unitCategory - category for the GTlab unit system
 * @param boundaries - The numeric boundaries allowed for `:value`
 * @param value Default - value
 * @return function factory for usage in container definitions
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeDoubleProperty(const QString& name,
                       const QString& brief,
                       const GtUnit::Category& unitCategory,
                       gt::Boundaries<double> boundaries,
                       const double& value = 0.0);

} // namespace gt

#endif // GTP_DOUBLEPARAM_H
