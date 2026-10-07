/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Created on: 4.7.2023
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_FINALLY_H
#define GT_FINALLY_H

#include <utility>

#include <gt_version.h>

namespace gt
{

/**
 * @brief The Finally class.
 * Calls a member function on an object in the destructor. Used for cleaning up.
 */
template <typename Functor>
class Finally
{
public:

    Finally() noexcept : m_invoked(true)
    {
    }

    explicit Finally(Functor func) noexcept : m_func{std::move(func)}
    {
    }

    // no copy
    Finally(Finally const&) = delete;
    Finally& operator=(Finally const&) = delete;

    // move allowed
    Finally(Finally&& other) noexcept :
        m_func{std::move(other.m_func)}, m_invoked{other.m_invoked}
    {
        other.clear();
    };

    Finally& operator=(Finally&& other) noexcept
    {
        Finally tmp{std::move(other)};
        this->swap(tmp);
        return *this;
    };

    ~Finally() noexcept
    {
        invoke();
    }

    /**
     * @brief Check if function or object is null
     * @return
     */
    bool isNull() const noexcept
    {
        return m_invoked;
    }

    /**
     * @brief Calls the cleanup function. Object will go invalid.
     */
    void finalize() noexcept
    {
        invoke();
        clear();
    }

    /**
     * @brief Clears object
     */
    void clear() noexcept
    {
        m_invoked = true;
    }

    /**
     * @brief Getter to access the functor used
     * @return Returns the functor used
     */
    Functor const& get() const
    {
        return m_func;
    }

    /**
     * @brief Swaps this object with other
     * @param other Other object
     */
    void swap(Finally& other) noexcept
    {
        using std::swap; // ADL
        swap(m_func, other.m_func);
        swap(m_invoked, other.m_invoked);
    }

    /// hidden friend function only found by ADL (best practice)
    friend void swap(Finally& a, Finally& b) noexcept
    {
        a.swap(b);
    }

private:

    // actual function to call
    Functor m_func;

    /// Indicates that method was already
    /// called
    bool m_invoked = false;

    /**
     * @brief Calls the cleanup function.
     */
    void invoke() noexcept
    {
        // call cleanup function
        if (!isNull()) m_func();
    }
};

/**
 * @brief Makes a finally object from a lambda
 *
 * Usage:
 *
 * {
 *     QLockFile lockFile{...};
 *
 *     auto cleanup = gt::finally([&](){
 *         lockFile.unlock();
 *     });
 * }
 *
 * @param t object
 * @param func Function to call on cleanup
 */
template<typename Functor>
inline auto finally(Functor func)
{
    return Finally<Functor>{std::move(func)};
}

/**
 * @brief Makes a finally object from a member function
 *
 * Usage:
 *
 * {
 *     QLockFile lockFile{...};
 *
       auto cleanup = gt::finally(&lockFile, &QLockFile::unlock);
 * }
 *
 * @param t object
 * @param func Member function to call on cleanup
 * @tparam T Object type to call memeber method on
 * @tparam R Return value of member method
 * @tparam U Base class in whcih the member method is implemented (defaults to T)
 */
template<typename T, typename R, typename U = T>
inline auto finally(T* t, R(U::*func)())
{
    // construct helper lambda
    return finally([obj = t, func](){
        if (obj && func)
        {
            (obj->*func)();
        }
    });
}

} // namespace gt

#endif // GT_FINALLY_H
