/**
 * Adapted from pybind11/mpl.h and extended
 * 
 * Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>, All rights reserved.
 *
 * Licensed unter the 3-clause BSD license
 */

#ifndef GT_MPL_H
#define GT_MPL_H

#include "gt_platform.h"

#include <tuple>
#include <functional>
#include <array>

#if CPP_STANDARD_VERSION < 201400
#error "C++14 or better is required"
#else

namespace gt
{

namespace mpl
{

/// Index sequence for convenient template metaprogramming involving tuples
template<size_t ...> struct index_sequence  { };
template<size_t N, size_t ...S> struct make_index_sequence : make_index_sequence <N - 1, N - 1, S...> { };
template<size_t ...S> struct make_index_sequence <0, S...> { typedef index_sequence<S...> type; };

namespace detail
{

/// Strip the class from a method type
template <typename T> struct remove_class {};
template <typename C, typename R, typename... A> struct remove_class<R (C::*)(A...)> { typedef R type(A...); };
template <typename C, typename R, typename... A> struct remove_class<R (C::*)(A...) const> { typedef R type(A...); };

/**
 * \brief Convert a lambda function to a std::function
 * From http://stackoverflow.com/questions/11893141/inferring-the-call-signature-of-a-lambda-or-arbitrary-callable-for-make-functio
 */
template <typename T> struct lambda_signature_impl {
    using type = typename remove_class<
        decltype(&std::remove_reference<T>::type::operator())>::type;
};
template <typename R, typename... A> struct lambda_signature_impl<R    (A...)> { typedef R type(A...); };
template <typename R, typename... A> struct lambda_signature_impl<R (&)(A...)> { typedef R type(A...); };
template <typename R, typename... A> struct lambda_signature_impl<R (*)(A...)> { typedef R type(A...); };
template <typename T> using lambda_signature = typename lambda_signature_impl<T>::type;
template <typename F> using make_function_type = std::function<lambda_signature<F>>;

struct void_type { };

/// Helper functions for calling a function using a tuple argument while dealing with void/non-void return values
template <typename RetType> struct tuple_dispatch {
    typedef RetType return_type;
    template<typename Func, typename Arg, size_t ... S> return_type operator()(const Func &f, Arg && args, index_sequence<S...>) {
        return f(std::get<S>(std::forward<Arg>(args))...);
    }
};

/// Helper functions for calling a function using a tuple argument (special case for void return values)
template <> struct tuple_dispatch<void> {
    typedef void_type return_type;
    template<typename Func, typename Arg, size_t ... S> return_type operator()(const Func &f, Arg &&args, index_sequence<S...>) {
        f(std::get<S>(std::forward<Arg>(args))...);
        return return_type();
    }
};

//! helper template function for the actual implementation of a compile-time for loop
template<typename GenericLambda, std::size_t ... Is>
constexpr void static_for_impl(GenericLambda&& f, std::index_sequence<Is...>)
{
    // unpack into std::initializer list for "looping" in correct order without recursion
    (void)std::initializer_list<char> {((void)f(std::integral_constant<unsigned, Is>()), '0')...};
}

} // namespace detail

/// Converts type signature (i.e. "const&", "*", tec.) to a c-string
template <typename T>
struct type_signature
{
    // by default return nothing
    static constexpr char const* get() { return ""; }
};

/// Custom type signatures
template <typename T> struct type_signature<T const> { static constexpr char const* get() { return " const"; } };
template <typename T> struct type_signature<T*> { static constexpr char const* get() { return "*"; } };
template <typename T> struct type_signature<T const*> { static constexpr char const* get() { return " const*"; } };
template <typename T> struct type_signature<T&> { static constexpr char const* get() { return "&"; } };
template <typename T> struct type_signature<T const&> { static constexpr char const* get() { return " const&"; } };

/// Wrapper for "typeid" to access and modify name of a type in form of a c-string
template <typename T>
struct type_name
{
    // by default access typeid
    static constexpr char const* get() { return typeid(T).name(); }
};

/// Custom "core" type names
template <> struct type_name<std::string> { static constexpr char const* get() { return "std::string"; } };
template <> struct type_name<void> { static constexpr char const* get() { return "void"; } };
template <> struct type_name<detail::void_type> { static constexpr char const* get() { return "void"; } };

/// Strip type signature to access "core" type
template <typename T> struct type_name<T const> { static constexpr char const* get() { return type_name<T>::get(); } };
template <typename T> struct type_name<T&> { static constexpr char const* get() { return type_name<T>::get(); } };
template <typename T> struct type_name<T const&> { static constexpr char const* get() { return type_name<T>::get(); } };
template <typename T> struct type_name<T*> { static constexpr char const* get() { return type_name<T>::get(); } };
template <typename T> struct type_name<T const*> { static constexpr char const* get() { return type_name<T>::get(); } };

/// For lambda functions delegate to their 'operator()'
template <typename T> struct function_traits :
        public function_traits<typename detail::make_function_type<T>> { };

/// Type traits for std::functions
template <typename ReturnType, typename... Args>
struct function_traits<std::function<ReturnType(Args...)>> {
    enum {
        nargs = sizeof...(Args),
        is_method = 0,
        is_const = 0
    };
    typedef std::function<ReturnType (Args...)>  f_type;
    typedef detail::tuple_dispatch<ReturnType>   dispatch_type;
    typedef typename dispatch_type::return_type  return_type;
    typedef std::tuple<Args...>                  args_type;

    template <size_t i> struct arg {
        typedef typename std::tuple_element<i, args_type>::type type;
    };

    /// name of return type (without signature)
    static constexpr char const* rtypeId() {
        return type_name<return_type>::get();
    }

    /// signature of return type (without name)
    static constexpr char const* rtypeSignature() {
        return type_signature<return_type>::get();
    }

    /// names of argument types (without signatures)
    static constexpr std::array<char const*, nargs> argIds() {
        return { type_name<Args>::get()... };
    }

    /// signatures of argument types (without names)
    static constexpr std::array<char const*, nargs> argSignatures() {
        return { type_signature<Args>::get()... };
    }

    /// invoke the function f with the arguemnts as a tuple
    template <typename Tuple>
    static return_type invoke(const f_type& f, Tuple&& args) {
        return dispatch_type()(f, std::forward<Tuple>(args),
            typename make_index_sequence<nargs>::type());
    }
};

/// static for. Invokes lambda method N times
template<unsigned N, typename GenericLambda>
constexpr void static_for(GenericLambda&& f)
{
    detail::static_for_impl(std::forward<GenericLambda>(f), std::make_index_sequence<N>());
}

/// static foreach. Invokes lambda for each element in tuple
template<typename... _Elements, typename GenericLambda>
constexpr void static_foreach(const std::tuple<_Elements...>& aTuple, GenericLambda&& f)
{
    constexpr auto N = std::tuple_size<std::tuple<_Elements...>>::value;
    static_for<N>([&aTuple, &f](auto i) {
        f(std::get<i>(aTuple));
    });
}

} // namespace mpl

} // namespace gt

#endif // c++ 14 required

#endif /* GT_MPL_H */
