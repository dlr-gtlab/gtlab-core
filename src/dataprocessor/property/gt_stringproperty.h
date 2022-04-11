/* GTlab - Gas Turbine laboratory
 * Source File: gt_stringproperties.h
 * copyright 2009-2015 by DLR
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
    bool setValueFromVariant(const QVariant& val, const QString& unit,
                             bool* success = nullptr) override;

    /**
     * @brief validator
     * @return the validator to use
     */
    QValidator* validator();

    ~GtStringProperty() override;

protected:
    std::unique_ptr<QValidator> m_validator;
};

#endif // GTSTRINGPROPERTY_H
