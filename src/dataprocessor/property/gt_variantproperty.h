/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    GtVariantProperty(const QString& name,
                      const QString& brief,
                      const GtUnit::Category &unitCategory = GtUnit::None,
                      const QVariant& value = QVariant());

    /**
     * @brief valueToVariant
     * @return
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = nullptr) const override;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = nullptr) override;

};

#endif // GTVARIANTPROPERTY_H
