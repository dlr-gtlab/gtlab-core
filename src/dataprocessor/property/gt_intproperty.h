/* GTlab - Gas Turbine laboratory
 * Source File: gt_intproperty.h
 * copyright 2009-2015 by DLR
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
    Q_INVOKABLE GtIntProperty(const QString& ident, const QString& name,
                              const QString& brief);

    /**
     * @brief GtIntProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param value
     */
    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  const GtUnit::Category& unitCategory,
                  const int& value = 0);

    /**
     * @brief GtIntProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param lowSideBoundary
     * @param highSideBoundary
     * @param value
     */
    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  const GtUnit::Category& unitCategory,
                  const int lowSideBoundary,
                  const int highSideBoundary,
                  const int& value = 0);

    /**
     * @brief GtIntProperty
     * @param ident
     * @param name
     * @param brief
     * @param unitCategory
     * @param boundType
     * @param boundary
     * @param value
     */
    GtIntProperty(const QString& ident,
                  const QString& name,
                  const QString& brief,
                  const GtUnit::Category& unitCategory,
                  GtIntProperty::BoundType boundType,
                  const int boundary,
                  const int& value = 0.0);

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
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeIntProperty(int value);

} // namespace gt

#endif // GTINTPROPERTY_H
