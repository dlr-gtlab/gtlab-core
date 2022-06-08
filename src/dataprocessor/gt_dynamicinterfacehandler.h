/* GTlab - Gas Turbine laboratory
 * Source File: dynamicinterfacehandler.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef DYNAMICINTERFACEHANDLER_H
#define DYNAMICINTERFACEHANDLER_H

#include "gt_datamodel_exports.h"

#include "gt_functional_interface.h"

#include <QVariantList>
#include <QMap>

#include <functional>

namespace gtlab {

/**
 * @brief This is the functional interface
 *
 * It provides a function with the signature
 *
 *   std::function<QVariantList(const QVariantList&)>
 *
 * a help() and name() method.
 *
 * To convert arbitrary functions into InterfaceFunction,
 * use `auto func = gtlab::interface::make_interface_function(the_function)`
 */
class InterfaceFunction
{
public:
    using FunctionType = std::function<QVariantList(const QVariantList&)>;

    InterfaceFunction(QString func_name, FunctionType func, QString help) :
        m_f(std::move(func)),
        m_name(std::move(func_name)),
        m_help(std::move(help))
    {}

    InterfaceFunction() = default;

    // We want to have implicit conversion from nullptr as it should behave
    // as a function pointer
    // cppcheck-suppress noExplicitConstructor // NOLINTNEXTLINE
    InterfaceFunction(nullptr_t) {}

    /**
     * @brief Calls the function
     */
    QVariantList operator()(const QVariantList& l) const
    {
        return m_f(l);
    }

    /**
     * @brief The name of the function
     */
    const QString& name() const
    {
        return m_name;
    }

    /**
     * @brief Returns the docstring of the interface function
     */
    const QString& help() const
    {
        return m_help;
    }

    /**
     * @brief checks, whether the function is a nullptr
     */
    bool is_null() const
    {
        return !m_f;
    }

    // We want to have implicit bool checks aka if (myFunction)
    // NOLINTNEXTLINE
    operator bool() const
    {
        return !is_null();
    }

    bool operator==(nullptr_t) const
    {
        return is_null();
    }

    bool operator!=(nullptr_t) const
    {
        return !is_null();
    }

private:
    FunctionType m_f;
    QString m_name, m_help;
};

namespace priv
{

class GT_DATAMODEL_EXPORT DynamicInterfaceHandler
{

public:
    static DynamicInterfaceHandler& instance();

    /**
     * @brief Adds a function to the handler
     *
     * Returns false, if a function with the same ID already exists
     */
    bool addInterface(const QString& id, InterfaceFunction func);

    /**
     * @brief Gets a function from the handler
     *
     * Returns nullptr, if functions does not exist
     */
    InterfaceFunction getInterfaceFunc(const QString& id);

    /**
     * @brief Returns the IDs of the registered functions
     */
    QStringList getRegisteredFunctionIDs() const;

private:
    QMap<QString, InterfaceFunction> m_interfaces;

    DynamicInterfaceHandler() = default;

};

} // namespace priv


namespace interface
{

namespace priv
{

template <typename Func>
QString get_function_signature()
{
    return QString("Signature:\n    %1\n").arg(typeid(Func).name());
}

template <typename Func>
QString get_default_help(const QString& function_name)
{
    QString help = QString("Help on function '%1'\n\n").arg(function_name);

    help += get_function_signature<Func>();
    return help;
}

/**
 * @brief Registers an InterfaceFunction to the handler
 *
 * Usage:
 *
 *   QVariantList itf_mypow(const QVariantList& args)
 *   {
 *      // check for argument count (2 required) and
 *      // argument types is ommitted for brevity
 *      // but needs to be included in production code!
 *
 *      double value = args[0].toDouble();
 *      int exponent = args[1].toInt();
 *
 *      double result = ...
 *
 *      return QVariantList({result});
 *   }
 *
 *   ...
 *
 *   interface::register_function("mypow", itf_mypow);
 */

template <class Func,
    typename std::enable_if<std::is_convertible<Func,
        InterfaceFunction::FunctionType>::value, Func>::type* = nullptr
> // only enabled for functions that resemble the dynamic interface
inline bool register_function(QString ident, Func func, QString help = "")
{
    InterfaceFunction int_func(ident, std::move(func), std::move(help));
    return gtlab::priv::DynamicInterfaceHandler::instance()
        .addInterface(std::move(ident), std::move(int_func));
}

/**
 * @brief Registers a function like function object.
 *
 * It is wrapped into a InterfaceFunction before adding it to the interface.
 *
 * Type checks and argument count checks are automatically included
 * by the InterfaceFunction that wraps func.
 *
 * Usage:
 *
 *   double mypow(double value, int exponent)
 *   {
 *      double result = ...
 *      return result;
 *   }
 *
 *   ...
 *
 *   interface::register_function("mypow", mypow);
 */

template <class Func,
    typename std::enable_if<!std::is_convertible<Func,
        InterfaceFunction::FunctionType>::value, Func>::type* = nullptr
> // this is disabled for interface functions
inline bool
register_function(QString ident, const Func& func, QString help = "")
{
    auto wrapped = gtlab::interface::make_interface_function(ident, func);

    if (help.isEmpty())
    {
        help = get_default_help<Func>(ident);
    }

    InterfaceFunction int_func(ident, wrapped, std::move(help));

    return gtlab::priv::DynamicInterfaceHandler::instance()
        .addInterface(std::move(ident), std::move(int_func));
}

} // namespace priv


/**
 * @brief Retrieves an interface function from Interface Handler
 *
 * Usage:
 *
 * auto itf_mypow = interface::get:function("mypow");
 *
 * auto result = itf_mypow({3.0, 2}); // returns QVariantList({9.0})
 */
inline InterfaceFunction
get_function(const QString& ident)
{
    return gtlab::priv::DynamicInterfaceHandler::instance().getInterfaceFunc(ident);
}

} // namespace interface
} // namespace gtlab

#endif // DYNAMICINTERFACEHANDLER_H
