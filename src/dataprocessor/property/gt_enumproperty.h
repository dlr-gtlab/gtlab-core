/* GTlab - Gas Turbine laboratory
 * Source File: gt_enumproperty.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +49 551 709 2708
 */

#ifndef GT_ENUMPROPERTY_H
#define GT_ENUMPROPERTY_H

#include <QMetaEnum>

#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"

/**
 * @brief The GtEnumProperty class is a GtModeProperty extension which simplifies storing enums in the datamodel.
 * Additionaly to the regular usage of properties, you have to state the enumeration which values you want to display during declaration.
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
     * @brief operator T enables implicit converting to the Enum which is stated in the template.
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
     * @brief Constant getter function with unit conversion. This method is hidden for GtEnumProperty.
     * @param unit of the result.
     * @param success holds the success status after the method call completed.
     * @return The string of the currently selected mode as QString.
     */
    QString getVal(const QString& unit,
                   bool* success = 0) const;

    /**
     * @brief Returns a reference to the parameter value. This method is hidden for GtEnumProperty.
     * @return
     */
    QString& get();

    /**
     * @brief Setter function for the input value. This method is hidden for GtEnumProperty.
     * @param value which should be set.
     * @param success holds the success status after the method call completed.
     */
    void setVal(const QString& value,
                bool* success = 0);
};

template<typename T>
GtEnumProperty<T>::GtEnumProperty(const QString& ident,
                                  const QString& name,
                                  const QString& brief) :
    GtModeProperty(ident, name, brief)
{
    const QMetaEnum metaEnum = QMetaEnum::fromType<T>();
    for (int i = 0; i < metaEnum.keyCount(); i++)
    {
        GtModeTypeProperty *subProperty = new GtModeTypeProperty(metaEnum.key(i), "");
        subProperty->setParent(this);
        registerSubProperty(*subProperty);
    }
}

template<typename T>
GtEnumProperty<T>::GtEnumProperty(const QString &ident,
                                  const QString &name,
                                  const QString &brief,
                                  const T value) :
    GtEnumProperty<T>(ident, name, brief)
{
    setVal(value);
}

template<typename T>
GtEnumProperty<T>& GtEnumProperty<T>::operator=(const T value)
{
    this->setVal(value);
    return *this;
}

template<typename T>
T GtEnumProperty<T>::getVal() const
{
    const QString key = GtModeProperty::getVal();
    return T(getMetaEnum().keyToValue(key.toUtf8()));
}

template<typename T>
void GtEnumProperty<T>::setVal(const T value,
                               bool *success)
{
    GtModeProperty::setVal(getMetaEnum().valueToKey(value), success);
}

template<typename T>
QMetaEnum GtEnumProperty<T>::getMetaEnum() const
{
    return QMetaEnum::fromType<T>();
}

#endif // GT_ENUMPROPERTY_H
