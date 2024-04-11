/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_enumproperty.h
 *
 *  Created on: 31.03.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +49 551 709 2708
 */

#ifndef GT_ENUMPROPERTY_H
#define GT_ENUMPROPERTY_H

#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"

#include <QMetaEnum>

/**
 * @brief The GtEnumProperty class is a GtModeProperty extension
 * which simplifies storing enums in the datamodel.
 * Additionaly to the regular usage of properties, you have to state the
 * enumeration which values you want to display during declaration.
 *
 * Example:
 * ```
 * PpmEnumProperty<PrEDiCTGlobals::BladeRowTypeEnum> m_bladeRowType;
 * ```
 */
template<typename T>
class GtEnumProperty : public GtModeProperty
{
public:
    /**
     * @brief GtEnumProperty<T> constructor.
     * The initial value is the enum with the lowest value.
     * @param ident identification string.
     * @param name to describe the property.
     * @param brief is a detailed description, which is displayed in a ToolTip.
     */
    GtEnumProperty<T>(const QString& ident,
                      const QString& name,
                      const QString& brief);

    /**
     * @brief GtEnumProperty<T> constructor with initial value.
     * @param ident identification string.
     * @param name to describe the property.
     * @param brief tis a detailed description, which is displayed in a ToolTip.
     * @param value will be the initial value of this property.
     */
    GtEnumProperty<T>(const QString& ident,
                      const QString& name,
                      const QString& brief,
                      const T value);

    /**
     * @brief Enables statements like GtEnumProperty<MyEnum> = MyEnum::value.
     * @param value is a value from the enum type.
     * @return GtEnumProperty with value as value.
     */
    GtEnumProperty<T>& operator=(const T value);

    /**
     * @brief operator T enables implicit converting to the Enum which is stated
     * in the template.
     * ```
     * MyEnum = GtEnumProperty<MyEnum>::value
     * ```
     */
    virtual operator T() const {
        return getVal();
    }

    /**
     * @brief Getter for the current enum value.
     * @return Enum which is part of this property.
     */
    T getVal() const;

    /**
     * @brief Sets the value of this property.
     * @param value which should be set.
     * @param success holds the success status after the method call completed.
     */
    void setVal(const T value, bool *success = nullptr);

    /**
     * @brief Creates a meta enum out of the template enum.
     * @return The QMetaEnum generatd.
     */
    QMetaEnum getMetaEnum() const;

private:

    /**
     * @brief This method is hidden for GtEnumProperty
     */
    QString getVal(const QString& unit, bool* success = nullptr) const = delete;

    /**
     * @brief This method is hidden for GtEnumProperty
     */
    QString& get() = delete;

    /**
     * @brief This method is hidden for GtEnumProperty
     */
    void setVal(const QString& value, bool* success = nullptr) = delete;
};

template<typename T>
inline GtEnumProperty<T>::GtEnumProperty(const QString& ident,
                                         const QString& name,
                                         const QString& brief) :
    GtModeProperty(ident, name, brief)
{
    const QMetaEnum metaEnum = QMetaEnum::fromType<T>();

    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        GtModeTypeProperty *subProperty =
                new GtModeTypeProperty(metaEnum.key(i), "");

        subProperty->setParent(this);
        registerSubProperty(*subProperty);
    }
}

template<typename T>
inline GtEnumProperty<T>::GtEnumProperty(const QString &ident,
                                         const QString &name,
                                         const QString &brief,
                                         const T value) :
    GtEnumProperty<T>(ident, name, brief)
{
    setVal(value);
}

template<typename T>
inline GtEnumProperty<T>& GtEnumProperty<T>::operator=(const T value)
{
    this->setVal(value);
    return *this;
}

template<typename T>
inline T GtEnumProperty<T>::getVal() const
{
    const QString key = GtModeProperty::getVal();
    return T(getMetaEnum().keyToValue(key.toUtf8()));
}

template<typename T>
inline void GtEnumProperty<T>::setVal(const T value, bool* success)
{
    GtModeProperty::setVal(getMetaEnum().valueToKey(static_cast<int>(value)), success);
}

template<typename T>
inline QMetaEnum GtEnumProperty<T>::getMetaEnum() const
{
    return QMetaEnum::fromType<T>();
}

namespace gt
{

/**
 * @brief Creates a property factory for enums with a default value
 */
template <typename T>
gt::PropertyFactoryFunction makeEnumProperty(const T& value)
{
    return [value](const QString& id)
    {
        auto p = new GtEnumProperty<T>(id, id, "");
        p->setVal(std::move(value));
        return p;
    };
}

} // namespace gt

#endif // GT_ENUMPROPERTY_H
