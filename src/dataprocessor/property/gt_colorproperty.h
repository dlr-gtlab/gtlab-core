/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GT_COLORPROPERTY_H
#define GT_COLORPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"


class GT_DATAMODEL_EXPORT GtColorProperty : public GtStringProperty
{
    Q_OBJECT

public:
    GtColorProperty(QString const& ident,
                    QString const& name,
                    QString const& brief,
                    QString const& color = {});

    /**
     * @brief Overloaded function to convert internal property value to
     * QVariant. Value is converted based on given unit.
     * @param unit Unit into which the value is to be converted.
     * @param ok Whether conversion was successfull or not.
     * @return Value as QVariant
     */
    QVariant valueToVariant(QString const& unit, bool* ok = nullptr) const override;

    /**
     * @brief Overloaded function to set internal property value from given
     * QVariant. Value of QVariant is converted based on given unit.
     * @param val New value in form of QVariant.
     * @param unit Unit of the input value.
     * @param success Whether the value could be set or not
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(QVariant const& val, QString const& unit) override;

    GtColorProperty& operator=(const QString& val);

};


#endif // GT_COLORPROPERTY_H
