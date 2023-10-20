/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_stringproperties.h
 *
 *  Created on: 14.10.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTSTRINGPROPERTY_H
#define GTSTRINGPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

#include <memory>

class QValidator;

class GT_DATAMODEL_EXPORT GtStringProperty : public GtProperty<QString>
{
    Q_OBJECT

    Q_PROPERTY(QString get READ getVal WRITE setVal)

public:
    using GtProperty<QString>::operator=;

    /**
     * @brief GtStringProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtStringProperty(const QString& ident,
                                 const QString& name);

    /**
     * @brief GtStringProperty
     * @param id
     * @param name
     * @param brief
     * @param value
     */
    GtStringProperty(const QString& ident,
                     const QString& name,
                     const QString& brief,
                     const QString& value = QString(),
                     QValidator* validator = nullptr);

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
    bool setValueFromVariant(const QVariant& val, const QString& unit) override;

    /**
     * @brief validator
     * @return the validator to use
     */
    QValidator* validator();

    ~GtStringProperty() override;

protected:
    std::unique_ptr<QValidator> m_validator;
};

namespace gt
{

/**
 * @brief Creates a property factory for strings with a default value
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeStringProperty(QString strValue = "");


} // namespace gt

#endif // GTSTRINGPROPERTY_H
