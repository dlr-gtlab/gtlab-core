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
#include "gt_variantconvert.h"
#include "gt_mpl.h"
#include "gt_typetraits.h"
#include "gt_platform.h"

#include <stdexcept>

#if CPP_STANDARD_VERSION < 201400
#error "C++14 or better is required"
#else

namespace gt
{
namespace interface
{
namespace detail
{

template <typename Func>
struct InterfaceFunctionBuilder
{
    explicit InterfaceFunctionBuilder(QString func_name,
                                      const Func& f) :
        wrapped_function(f),
        name(std::move(func_name))
    {
    }

    QVariantList operator()(const QVariantList& variantList) const
    {
        using f_traits = gt::mpl::function_traits<Func>;
        using args_type = typename f_traits::args_type;

        // check matching number of arguments
        if (variantList.size() != f_traits::nargs)
        {
            throw std::runtime_error(
                "Function argument mismatch in function '" +
                name.toStdString() +
                "'. Expected " +
                std::to_string(f_traits::nargs) + " args, got " +
                std::to_string(variantList.size()));
        }

        auto wrappedFargs = gt::fromVariant<args_type>(variantList);

        // execute function, the arguments could be passed by reference, hence
        // they cannot be moved
        const auto func_result = f_traits::invoke(
            wrapped_function, wrappedFargs);

        // convert result into variant list
        return gt::toVariantList(func_result);
    }

    Func wrapped_function;
    QString name;
};

} // namespace detail

template <typename Func>
QString getFunctionSignature()
{
    return QString("Signature:\n    %1\n").arg(typeid(Func).name());
}

template <typename Func>
QString getDefaultHelp(const QString& function_name)
{
    QString help = QString("Help on function '%1'\n\n").arg(function_name);

    help += getFunctionSignature<Func>();
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
    trait::if_not_convertible<Func, InterfaceFunction::FunctionType> = true
> // this is disabled for interface functions
InterfaceFunction makeInterfaceFunction(
        const QString& funcName, Func&& f, QString help = {})
{
    if (help.isEmpty())
    {
        help = getDefaultHelp<Func>(funcName);
    }

    auto funcWrapper =  detail::InterfaceFunctionBuilder<Func>(
        funcName, std::forward<Func>(f));

    return InterfaceFunction(funcName, std::move(funcWrapper), std::move(help));
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
    trait::if_convertible<Func, InterfaceFunction::FunctionType> = true
> // this is only enabled for interface functions
InterfaceFunction makeInterfaceFunction(
        const QString& funcName, Func&& f, QString help = {})
{
    if (help.isEmpty())
    {
        help = getDefaultHelp<Func>(funcName);
    }

    return InterfaceFunction(funcName, std::forward<Func>(f), std::move(help));
}

} // namespace interface

} // namespace gt

#endif // c++ 14 required

#endif // FUNCTIONAL_INTERFACE_H
