/* GTlab - Gas Turbine laboratory
 * Source File: gt_functionalinterface.h
 * copyright 2022 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GTFUNCTIONALINTERFACE_H
#define GTFUNCTIONALINTERFACE_H

#include "gt_sharedfunction.h"
#include "gt_variantconvert.h"
#include "gt_mpl.h"
#include "gt_typetraits.h"
#include "gt_platform.h"

#include <stdexcept>

namespace gt
{
namespace interface
{
namespace detail
{

template <typename Func>
struct SharedFunctionBuilder
{
    explicit SharedFunctionBuilder(QString func_name,
                                      const Func& f) :
        wrapped_function(f),
        name(std::move(func_name))
    {
    }

    QVariantList operator()(const QVariantList& args) const noexcept(false)
    {
        using f_traits = gt::mpl::function_traits<Func>;
        using args_type = typename f_traits::args_type;

        // check matching number of arguments
        if (args.size() != f_traits::nargs)
        {
            throw std::runtime_error(
                "Function argument mismatch in function '" +
                name.toStdString() +
                "'. Expected " +
                std::to_string(f_traits::nargs) + " args, got " +
                std::to_string(args.size()));
        }

        auto wrappedFargs = gt::fromVariant<args_type>(args);

        // execute function, the arguments could be passed by reference, hence
        // they cannot be moved
        const auto result = f_traits::invoke(wrapped_function, wrappedFargs);

        // convert result into variant list
        return gt::toVariantList(result);
    }

    Func wrapped_function;
    QString name;
};

} // namespace detail

/**
 * @brief Attempts to create a readable function signature for the Function type
 * @return Function signature
 */
template <typename Func>
inline QString getFunctionSignature()
{
    using f_traits = gt::mpl::function_traits<Func>;

    QString signature = QString{f_traits::rtypeId()} +
                        QString{f_traits::rtypeSignature()} +
                        QStringLiteral(" (");

    auto args    = f_traits::argIds();
    auto argSigs = f_traits::argSignatures();

    static_assert(args.size() == argSigs.size(), "invalid layout!");

    if (!args.empty())
    {
        for (size_t i = 0; i < args.size(); ++i)
        {
            signature += QString{args[i]} + QString{argSigs[i]};
            signature += QStringLiteral(", ");
        }
        signature.remove(signature.size() - 2, 2);
    }
    signature += QStringLiteral(")");

    // some type ids contain unwanted compiler symbols
    signature.remove(QStringLiteral("class "))
             .remove(QStringLiteral("struct "));

    return signature;
}

/**
 * @brief Overload that accepts a functor directly
 * @return Function signature
 */
template <typename Func>
inline QString getFunctionSignature(Func const&)
{
    return getFunctionSignature<Func>();
}

/**
 * @brief Generates a default help for the function consiting of its signature.
 * This may not be perfect, therefore one should provide a proper help.
 * @return Default help
 */
template <typename Func>
inline QString getDefaultHelp()
{
    return QStringLiteral("Signature: ") + getFunctionSignature<Func>();
}

/**
 * @brief Overload that accepts a functor directly
 * @return Default help
 */
template <typename Func>
inline QString getDefaultHelp(Func const&)
{
    return getDefaultHelp<Func>();
}

/**
 * @brief Builds a gtlab module shared function from a regular function
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
 *   auto itf_trisum = makeSharedFunction("trisum", trisum);
 *
 *   // call trisum
 *   auto result = itf_trisum({1, 2, 3}); // returns QVariantList({6})
 *
 * @param funcName The name of the function to be wrapped
 * @param f The function / functional object
 * @return Shared function
 */
template <
    typename Func,
    trait::enable_if_not_convertible<Func, SharedFunction::FunctionType> = true
> // this is disabled for shared functions
inline SharedFunction
makeSharedFunction(QString const& funcName, Func&& f, QString help = {})
{
    if (help.isEmpty())
    {
        help = getDefaultHelp<Func>();
    }

    auto funcWrapper =  detail::SharedFunctionBuilder<Func>(
        funcName, std::forward<Func>(f));

    return SharedFunction(funcName, std::move(funcWrapper), std::move(help));
}

/**
 * @brief Builds a gtlab module shared function from
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
 *   auto fun = interface::makeSharedFunction("mypow", itf_mypow, help);
 *
 * @param funcName The name of the function to be wrapped
 * @param f The function using the signature QVariantList(const QVariantList&)
 * @return Shared function
 */
template <
    typename Func,
    trait::enable_if_convertible<Func, SharedFunction::FunctionType> = true
> // this is only enabled for shared functions
inline SharedFunction
makeSharedFunction(QString const& funcName, Func&& f, QString help = {})
{
    if (help.isEmpty())
    {
        help = getDefaultHelp<Func>();
    }

    return SharedFunction(funcName, std::forward<Func>(f), std::move(help));
}

} // namespace interface

} // namespace gt

#endif // GTFUNCTIONALINTERFACE_H
