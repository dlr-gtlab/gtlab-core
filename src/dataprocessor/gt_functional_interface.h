/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_dynamicinterface.cpp
 * copyright 2022 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#ifndef FUNCTIONAL_INTERFACE_H
#define FUNCTIONAL_INTERFACE_H

#include "gt_dynamicinterface.h"
#include "gt_variant_convert.h"
#include "gt_mpl.h"
#include "gt_platform.h"

#if CPP_STANDARD_VERSION < 201400
#error "C++14 or better is required"
#else

namespace gtlab {
namespace interface {

namespace detail {

template <typename Func>
struct InterfaceFunctionBuilder
{
    explicit InterfaceFunctionBuilder(QString func_name,
                                      const Func& f) :
        wrapped_function(f),
        name(std::move(func_name))
    {
    }

    QVariantList operator()(const QVariantList& variant_list) const
    {
        typedef mpl::function_traits<Func> traits;

        // check matching number of arguments
        if (variant_list.size() != traits::nargs)
        {
            throw std::runtime_error(
                "Function argument mismatch in function '" +
                name.toStdString() +
                "'. "
                "Expected " +
                std::to_string(traits::nargs) + " args, " + "got " +
                std::to_string(variant_list.size()));
        }

        auto wrapped_function_args = from_variant<typename traits::args_type>(variant_list);

        // execute function, the arguments could be passed by reference, hence
        // they cannot be moved
        const auto func_result = traits::invoke(
            wrapped_function, wrapped_function_args);

        // convert result into variant list
        return to_variant_list(func_result);
    }

    Func wrapped_function;
    QString name;
};

} // namespace detail

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
 * @brief Builds a gtlab module interface function from a regular function
 *
 * This function adds argument type checks and also checks for
 * the correct number of arguments.
 *
 * Note: The returned function might throw, if the arguments don't match the
 * arguments of the wrapped function.
 *
 * Usage:
 *
 *   int trisum(int a, int b, int c) {
 *      return a + b + c;
 *   }
 *
 *   auto itf_trisum = make_interface_function("trisum", trisum);
 *
 *   // call trisum
 *   auto result = itf_trisum({1, 2, 3}); // returns QVariantList({6})
 *
 * @param funcName The name of the function to be wrapped
 * @param f The function / functional object
 */
template <
  typename Func,
  typename std::enable_if_t<!std::is_convertible<Func, InterfaceFunction::FunctionType>::value, int> = 0
> // this is disabled for interface functions
InterfaceFunction make_interface_function(
    const QString& funcName, Func&& f, QString help = "")
{
    if (help.isEmpty())
    {
        help = get_default_help<Func>(funcName);
    }

    auto funcWrapper =  detail::InterfaceFunctionBuilder<Func>(
        funcName, std::forward<Func>(f));

    return InterfaceFunction(funcName, std::move(funcWrapper), help);
}

/**
 * @brief Builds a gtlab module interface function from
 * a QVariantList based function
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
 *   QString help = "This is a help to mypow";
 *
 *   ...
 *
 *   auto fun = interface::make_interface_function("mypow", itf_mypow, help);
 *
 * @param funcName The name of the function to be wrapped
 * @param f The function using the signature QVariantList(const QVariantList&)
 * @return An InterfaceFunction object
 */
template <
  typename Func,
  typename std::enable_if_t<std::is_convertible<Func, InterfaceFunction::FunctionType>::value, int> = 0
> // this is only enabled for interface functions
InterfaceFunction make_interface_function(
    const QString& funcName, Func&& f, QString help = "")
{
    if (help.isEmpty())
    {
        help = get_default_help<Func>(funcName);
    }


    return InterfaceFunction(funcName, std::forward<Func>(f), help);
}

} // namespace interface
} // namespace gtlab

#endif // c++ 14 required

#endif // FUNCTIONAL_INTERFACE_H
