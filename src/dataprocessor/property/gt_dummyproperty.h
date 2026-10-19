/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTDUMMYPROPERTY_H
#define GTDUMMYPROPERTY_H

#include "gt_abstractproperty.h"
#include "../gt_objectmemento.h"

/** Placeholder retaining the complete original dynamic-property record. */
class GT_DATAMODEL_EXPORT GtDummyProperty : public GtAbstractProperty
{
    Q_OBJECT
public:
    explicit GtDummyProperty(
        const GtObjectMemento::DynamicPropertyData& definition);

    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override;
    bool setValueFromVariant(const QVariant& value,
                             const QString& unit) override;
    gt::PropertyConfiguration configuration() const override;
    bool applyConfiguration(const gt::PropertyConfiguration& config) override;

    const GtObjectMemento::DynamicPropertyData& originalDefinition() const;
    QVariant storedValue() const;

private:
    GtObjectMemento::DynamicPropertyData m_definition;
};

#endif // GTDUMMYPROPERTY_H
