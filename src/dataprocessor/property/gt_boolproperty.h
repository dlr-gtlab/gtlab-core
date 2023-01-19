/* GTlab - Gas Turbine laboratory
 * Source File: gt_boolproperty.h
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTBOOLPROPERTY_H
#define GTBOOLPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

/**
 * @brief The GtBoolProperty class
 */
class GT_DATAMODEL_EXPORT GtBoolProperty : public GtProperty<bool>
{
    Q_OBJECT

    Q_PROPERTY(bool get READ getVal WRITE setVal)

public:
    using GtProperty<bool>::operator=;

    /**
     * @brief GtBoolProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtBoolProperty(const QString& ident,
                               const QString& name);

    /**
     * @brief GtBoolProperty
     * @param ident
     * @param name
     * @param brief
     * @param value
     */
    GtBoolProperty(const QString& ident,
                   const QString& name,
                   const QString& brief,
                   bool value = false);

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
};

namespace gt
{

/**
 * @brief Creates a property factory for bools with a default value
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeBoolProperty(bool value);

} // namespace gt

#endif // GTBOOLPROPERTY_H
