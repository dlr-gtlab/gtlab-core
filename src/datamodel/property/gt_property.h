#ifndef GTLAB_PARAMETER_H
#define GTLAB_PARAMETER_H

#include <QMetaType>
#include <QString>

#include "gt_abstractproperty.h"
#include "gt_unit.h"
#include "gt_unitconverter.h"

template<class ParamType>
class GtProperty : public GtAbstractProperty
{
public:
    /**
     * @brief ~GtProperty
     */
    virtual ~GtProperty(){}

    /**
     * @brief Returns a reference to the parameter value.
     * @return
     */
    ParamType& get();

    ParamType& operator()(void){return m_value;}

    virtual operator ParamType () const {
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
    ParamType getVal(const QString& unit, bool* success = 0) const;

    /**
     * @brief Setter function for the input value.
     * @param value
     * @param success
     */
    void setVal(const ParamType& value,
                bool* success = 0);

    /**
     * @brief Setter function for the input value with unit conversion.
     * @param value
     * @param unit
     * @param success
     */
    void setVal(const ParamType& value, const QString& unit,
                bool* success = 0);

    /**
     * @brief Reverts property to its initial value.
     */
    void revert() {
        setVal(m_initValue);
    }

    GtProperty<ParamType>& operator=(ParamType val) {
        setVal(val);
        return *this;
    }

protected:
    /**
     * @brief Default constructor.
     */
    GtProperty(){}

    /// Value
    ParamType m_value;

    /// Initial value
    ParamType m_initValue;

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
    virtual ParamType convertFrom(const ParamType &value, const QString& unit,
                         bool* success = 0);

    /**
     * @brief convertTo
     * @param unit
     * @param success
     * @return
     */
    virtual ParamType convertTo(const QString& unit,
                                bool* success = 0) const;

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
    void forceSetVal(const ParamType& value, bool* success = 0);

    /**
     * @brief setValFromConnection
     */
    void setValFromConnection();

};

template<class ParamType>
ParamType& GtProperty<ParamType>::get()
{
    return m_value;
}

template<class ParamType>
ParamType GtProperty<ParamType>::getVal() const
{
    return m_value;
}

template<class ParamType>
ParamType GtProperty<ParamType>::getVal(const QString &unit,
                                         bool *success) const
{
    if (unit.isEmpty())
    {
        if (success != 0)
        {
            *success = true;
        }

        return getVal();
    }

    bool tmpSuccess;

    ParamType val = convertTo(unit, &tmpSuccess);

    if (success != 0)
    {
        *success = tmpSuccess;
    }

    return val;
}

template<class ParamType>
void GtProperty<ParamType>::setVal(const ParamType& value, bool* success)
{
    if (m_connection != Q_NULLPTR)
    {
        gtWarning() << tr("Could not set connected property!") << " ("
                    << objectName() << ")";
        return;
    }

    forceSetVal(value, success);
}

template<class ParamType>
void GtProperty<ParamType>::forceSetVal(const ParamType &value, bool* success)
{
    if (success != 0)
    {
        *success = false;
    }

    if (!validateValue(value))
    {
        return;
    }

    m_value = value;

    if (success != 0)
    {
        *success = true;
    }

    emit changed();
}

template<class ParamType>
void GtProperty<ParamType>::setValFromConnection()
{
    qDebug() << "#### setValFromConnection!";

    // check connection
    if (m_connection == Q_NULLPTR)
    {
        return;
    }

    // get source value in form of QVariant
    QVariant variant = m_connection->valueFromSource();

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
void GtProperty<ParamType>::setVal(const ParamType &value,
                                   const QString &unit, bool* success)
{
    if (success != 0)
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

    if (success != 0)
    {
        *success = tmpSuccess;
    }

    if (tmpSuccess)
    {
         setVal(tmpVal, success);
    }
}

template<class ParamType>
ParamType GtProperty<ParamType>::convertFrom(const ParamType& value,
                                    const QString& /*unit*/,
                                    bool* success)
{
    *success = false;

    // TODO: conversion warning

    return value;
}

template<class ParamType>
ParamType GtProperty<ParamType>::convertTo(const QString& /*unit*/,
                                            bool* success) const
{
    *success = false;

    // TODO: conversion warning

    return m_value;
}

template<class ParamType>
bool GtProperty<ParamType>::validateValue(const ParamType& /*value*/)
{
    return true;
}

#endif // GTLAB_PARAMETER_H
