/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTLAB_PARAMETER_H
#define GTLAB_PARAMETER_H

#include <QMetaType>
#include <QString>

#include "gt_abstractproperty.h"
#include "gt_unit.h"
#include "gt_utilities.h"
#include "gt_unitconverter.h"

template<class ParamType>
class GtProperty : public GtAbstractProperty
{
public:

    /**
     * @brief The Bound struct
     * Can handle boundaries for numerical propeties
     */
    struct Bound
    {
        enum BoundType
        {
            BoundLow,
            BoundHigh
        };

        /**
         * @brief type - low or high value boundary
         */
        BoundType type;

        /**
         * @brief value of the boundary
         */
        ParamType value;
    };

    /**
     * @brief Returns a reference to the parameter value.
     * @return
     */
    ParamType& get();
    ParamType const & get() const;

    ParamType& operator()(void){return m_value;}

    virtual operator ParamType () const
    {
        return getVal();
    }

    /**
     * @brief Constant getter function.
     * @return
     */
    ParamType getVal() const;

    /**
     * @brief Constant getter function with unit conversion.
     * @param unit
     * @param success
     * @return
     */
    ParamType getVal(const QString& unit, bool* success = nullptr) const;

    /**
     * @brief Setter function for the input value.
     * @param value
     * @param success
     */
    void setVal(const ParamType& value,
                bool* success = nullptr);

    /**
     * @brief Setter function for the input value with unit conversion.
     * @param value
     * @param unit
     * @param success
     */
    void setVal(const ParamType& value,
                const QString& unit,
                bool* success = nullptr);

    /**
     * @brief Reverts property to its initial value.
     */
    void revert() override {
        setVal(m_initValue);
    }

    GtProperty<ParamType>& operator=(const ParamType& val) {
        setVal(val);
        return *this;
    }

protected:
    /**
     * @brief Default constructor.
     */
    GtProperty(){}

    /// Value
    ParamType m_value = {};

    /// Initial value
    ParamType m_initValue = {};

    /// Pointer to unit converter object
    GtUnitConverter<ParamType>* m_unitConverter =
            &GtUnitConverter<ParamType>::instance();

    /**
     * @brief convertFrom
     * @param value
     * @param unit
     * @param success
     * @return
     */
    virtual ParamType convertFrom(const ParamType& value,
                                  const QString& unit,
                                  bool* success = nullptr);

    /**
     * @brief convertTo
     * @param unit
     * @param success
     * @return
     */
    virtual ParamType convertTo(const QString& unit,
                                bool* success = nullptr) const;

    /**
     * @brief validateValue
     * @param value
     * @param unit
     * @return
     */
    virtual bool validateValue(const ParamType& value);

private:
    /**
     * @brief Setter function for the input value (forced).
     * @param value
     * @param success
     */
    void forceSetVal(const ParamType& value, bool* success = nullptr);

    /**
     * @brief setValFromConnection
     */
    void setValFromConnection() override;

};

template<class ParamType>
inline ParamType& GtProperty<ParamType>::get()
{
    return m_value;
}

template<class ParamType>
inline ParamType const & GtProperty<ParamType>::get() const
{
    return m_value;
}


template<class ParamType>
inline ParamType GtProperty<ParamType>::getVal() const
{
    return m_value;
}

template<class ParamType>
inline ParamType GtProperty<ParamType>::getVal(const QString& unit,
                                               bool* success) const
{
    if (unit.isEmpty())
    {
        return gt::valueSuccess(getVal(), success);
    }

    bool _success = false;

    ParamType val = convertTo(unit, &_success);

    return gt::valueSetSuccess(val, _success, success);
}

template<class ParamType>
inline void GtProperty<ParamType>::setVal(const ParamType& value,
                                          bool* success)
{
    if (m_connection)
    {
        gtWarning().medium().nospace()
                << tr("Could not set connected property!") << " ("
                << objectName() << ")";
        return;
    }

    forceSetVal(value, success);
}

template<class ParamType>
inline void GtProperty<ParamType>::forceSetVal(const ParamType& value,
                                               bool* success)
{
    if (success)
    {
        *success = false;
    }

    if (!validateValue(value))
    {
        return;
    }

    m_value = value;

    if (success)
    {
        *success = true;
    }

    emit changed();
}

template<class ParamType>
inline void GtProperty<ParamType>::setValFromConnection()
{
    // check connection
    if (!m_connection)
    {
        return;
    }

    // get source value in form of QVariant
    QVariant variant = gt::getConnectedValue(*m_connection);

    // check variant
    if (!variant.isValid())
    {
        return;
    }

    // check ability to convert to given param type
    if (!variant.canConvert<ParamType>())
    {
        return;
    }

    // convert to param
    ParamType param = variant.value<ParamType>();

    // set value (forced)
    forceSetVal(param);
}

template<class ParamType>
inline void GtProperty<ParamType>::setVal(const ParamType& value,
                                          const QString& unit,
                                          bool* success)
{
    if (success)
    {
        *success = false;
    }

    if (unit.isEmpty())
    {
        setVal(value, success);
        return;
    }

    bool tmpSuccess = false;
    ParamType tmpVal = convertFrom(value, unit, &tmpSuccess);

    if (success)
    {
        *success = tmpSuccess;
    }

    if (tmpSuccess)
    {
         setVal(tmpVal, success);
    }
}

template<class ParamType>
inline ParamType GtProperty<ParamType>::convertFrom(const ParamType& value,
                                                    const QString& /*unit*/,
                                                    bool* success)
{
    // TODO: conversion warning
    return gt::valueError(value, success);
}

template<class ParamType>
inline ParamType GtProperty<ParamType>::convertTo(const QString& /*unit*/,
                                                  bool* success) const
{
    // TODO: conversion warning
    return gt::valueError(m_value, success);
}

template<class ParamType>
inline bool GtProperty<ParamType>::validateValue(const ParamType& /*value*/)
{
    return true;
}

namespace gt
{

template <typename PropertyType>
inline gt::PropertyFactoryFunction makePropertyFactory()
{
    return [=](const QString& id) {
        return new PropertyType(id, id);
    };
}

/**
 * @brief Creates a property factory for Ts with a default value
 */
template <typename PropertyType, typename T>
inline gt::PropertyFactoryFunction makePropertyFactory(const T& value)
{
    return [=](const QString& id) {
        auto p = new PropertyType(id, id);
        auto _ = p->setValueFromVariant(std::move(value), "");
        Q_UNUSED(_);
        return p;
    };
}

/**
 * @brief Wraps a property factory function and returns a read only property
 * @param propertyFactory Function to wrap
 * @return Read only property factory function
 */
template <typename Functor>
inline gt::PropertyFactoryFunction makeReadOnly(Functor propertyFactory)
{
    return [func = std::move(propertyFactory)](QString const& id) -> GtAbstractProperty* {
        GtAbstractProperty* tmp = func(id);
        if (!tmp) return nullptr;

        tmp->setReadOnly(true);
        return tmp;
    };
};

/**
 * @brief Wraps a property factory function and returns a hidden property
 * @param propertyFactory Function to wrap
 * @return Hidden property factory function
 */
template <typename Functor>
inline gt::PropertyFactoryFunction makeHidden(Functor propertyFactory)
{
    return [func = std::move(propertyFactory)](QString const& id)  -> GtAbstractProperty* {
        GtAbstractProperty* tmp = func(id);
        if (!tmp) return nullptr;

        tmp->hide(true);
        return tmp;
    };
};

/**
 * @brief Wraps a property factory function and returns an optional property
 * @param propertyFactory Function to wrap
 * @param isActive Flag to set the property as active
 * @return Optional property factory function
 */
template <typename Functor>
inline gt::PropertyFactoryFunction makeOptional(Functor propertyFactory,
                                                bool isActive = false)
{
    return [func = std::move(propertyFactory), isActive](QString const& id) -> GtAbstractProperty* {
        GtAbstractProperty* tmp = func(id);
        if (!tmp) return nullptr;

        tmp->setOptional(true);
        tmp->setActive(isActive);
        return tmp;
    };
};

/**
 * @brief Wraps a property factory function and returns an monitoring property
 * @param propertyFactory Function to wrap
 * @return Monitoring property factory function
 */
template <typename Functor>
inline gt::PropertyFactoryFunction makeMonitoring(Functor propertyFactory)
{
    return [func = std::move(propertyFactory)](QString const& id) -> GtAbstractProperty* {
        GtAbstractProperty* tmp = func(id);
        if (!tmp) return nullptr;

        tmp->setMonitoring(true);
        return tmp;
    };
};
} // namespace gt

#endif // GTLAB_PARAMETER_H
