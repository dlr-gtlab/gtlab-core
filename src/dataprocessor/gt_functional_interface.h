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

#include "gt_variant_convert.h"
#include "gt_mpl.h"

#if __cplusplus < 201400
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
        const mpl::function_traits<Func> traits;
        // check matching number of arguments
        if (variant_list.size() != traits.nargs)
        {
            throw std::runtime_error(
                "Function argument mismatch in function '" +
                name.toStdString() +
                "'. "
                "Expected " +
                std::to_string(traits.nargs) + " args, " + "got " +
                std::to_string(variant_list.size()));
        }

        typename mpl::function_traits<Func>::args_type
            wrapped_function_args;

        // convert all variant arguments into the argument
        // types required for the wrapped function
        mpl::static_for<traits.nargs>(
            [&variant_list, &wrapped_function_args](const auto i)
        {
            typedef typename mpl::function_traits<
                Func>::template arg<i>::type ArgType;
            const auto& variant_arg = variant_list.at(i);

            // check if is convertible
            if (!can_convert<ArgType>(variant_arg))
            {
                throw std::runtime_error(
                    "Cannot convert function argument " +
                    std::to_string(i) + ". Expecting type '" +
                    typeid(ArgType).name() + "' got value: " +
                    variant_arg.toString().toStdString()
                );
            }

            // convert
            std::get<i>(wrapped_function_args) =
                variant_arg.template value<ArgType>();
        });

        // execute function
        const auto func_result = decltype(traits)::dispatch(
            wrapped_function, std::move(wrapped_function_args));

        // convert result into variant list
        return to_variant_list(func_result);
    }

    Func wrapped_function;
    QString name;
};

} // namespace detail

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
template <typename Func>
std::function<QVariantList(QVariantList)> make_interface_function(
    const QString& funcName, Func&& f)
{
    return detail::InterfaceFunctionBuilder<Func>(
        funcName, std::forward<Func>(f));
}

} // namespace interface
} // namespace gtlab

#endif // c++ 14 required

#endif // FUNCTIONAL_INTERFACE_H
