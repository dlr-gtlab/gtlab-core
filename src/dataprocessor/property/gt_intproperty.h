/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_intproperty.h
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTINTPROPERTY_H
#define GTINTPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

class GT_DATAMODEL_EXPORT GtIntProperty : public GtProperty<int>
{
    Q_OBJECT

    Q_PROPERTY(int get READ getVal WRITE setVal)

public:
    using GtProperty<int>::operator=;

    enum BoundType
    {
        BoundLow,
        BoundHigh
    };

    /**
     * @brief GtIntProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtIntProperty(const QString& ident,
                              const QString& name);

    /**
     * @brief GtIntProperty
     * @param ident
     * @param name
     * @param brief
     */
    Q_INVOKABLE GtIntProperty(const QString& ident,
                              const QString& name,
                              const QString& brief);

    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  int value);

    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  int lowSideBoundary,
                  int highSideBoundary,
                  int value = 0);

    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  BoundType boundType,
                  int boundary,
                  int value = 0);

    /**
     * @brief valueToVariant
     * @return
     */
    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(const QVariant& val,
                             const QString& unit) override;

    /**
     * @brief Returns value of low side boundary. 0 if no lower boundary is
     * set.
     * @return Low side boundary.
     */
    int lowSideBoundary() const;

    /**
     * @brief Returns value of high side boundary. 0 if no higher boundary is
     * set.
     * @return High side boundary.
     */
    int highSideBoundary() const;

protected:
    /// Perform low boundary check flag (true=perform check)
    bool m_boundsCheckFlagLow;

    /// Perform high boundary check flag (true=perform check)
    bool m_boundsCheckFlagHi;

    /// Low side boundary
    int m_boundLo;

    /// High side boundary
    int m_boundHi;

    /**
     * @brief validateValue
     * @param value
     * @param unit
     * @return
     */
    bool validateValue(const int& value) override;

};

namespace gt
{

/**
 * @brief Creates a property factory for ints with a default value
 * @param value Default value
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeIntProperty(int value);

    /**
 * @brief Creates a property factory for ints with a default value and a low
 * and high side boundary
 * @param value Default value
 * @param lowSideBoundary Lower side boundary
 * @param highSideBoundary High side boundary
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeIntProperty(int lowSideBoundary,
                                            int highSideBoundary,
                                            int value = 0);

/**
 * @brief Creates a property factory for ints with a default value and a custom
 * boundary
 * @param value Default value
 * @param boundaryType Boundary type
 * @param boundary Boundary
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeIntProperty(GtIntProperty::BoundType boundaryType,
                                            int boundary,
                                            int value = 0);

} // namespace gt

#endif // GTINTPROPERTY_H
