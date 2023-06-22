/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 04.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTVARIANTPROPERTY_H
#define GTVARIANTPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

/**
 * @brief The GtVariantProperty class
 */
class GT_DATAMODEL_EXPORT GtVariantProperty : public GtProperty<QVariant>
{
    Q_OBJECT

public:
    /**
     * @brief GtVariantProperty
     */
    [[deprecated("Provide a property id in the constructor")]]
    GtVariantProperty(const QString& name,
                      const QString& brief,
                      const GtUnit::Category &unitCategory = GtUnit::None,
                      const QVariant& value = QVariant());

    GtVariantProperty(const QString& id,
                      const QString& name,
                      const QString& brief,
                      const GtUnit::Category &unitCategory = GtUnit::None,
                      const QVariant& value = QVariant());

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

};

namespace gt
{

/**
 * @brief Creates a property factory for QVariants
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction
makeVariantProperty(const GtUnit::Category &unitCategory = GtUnit::None);

} // namespace gt

#endif // GTVARIANTPROPERTY_H
