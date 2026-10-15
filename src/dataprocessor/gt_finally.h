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

#include <cassert>
#include <utility>
#include <type_traits>
#include <exception>

#include <gt_datamodel_exports.h>

namespace gt
{

    namespace detail
    {
        /// logs that an exception occurred at `origin` and a description
        /// (`what`) if available
        GT_DATAMODEL_EXPORT
        void logCriticalException(const char* origin,
                                  const char* what = nullptr) noexcept;

    } // namespace detail

/**
 * @brief The Finally class.
 * Calls a member function on an object in the destructor. Used for cleaning up.
 */
template <typename Functor>
class Finally
{
public:
    /// expose Functor-type
    using function_type = Functor;

    /// default constructor
    Finally() noexcept : m_invoked{true}
    {
        static_assert(std::is_nothrow_default_constructible<Functor>::value,
                      "Functor must be nothrow default constructible");
    }

    /// constructor by moving functor
    explicit Finally(Functor func) noexcept :
        m_func{take(func)}, m_invoked{false}
    {
         static_assert(std::is_nothrow_move_constructible<Functor>::value ||
                       std::is_nothrow_copy_constructible<Functor>::value,
                      "Functor must be nothrow move- or copy-constructible");
    }

    // no copy
    Finally(Finally const&) = delete;
    Finally& operator=(Finally const&) = delete;

    // move allowed
    Finally(Finally&& other) noexcept :
        m_func{take(other.m_func)}, m_invoked{other.m_invoked}
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
        if (isNull()) return;

        // no exception must escape
        if constexpr (std::is_nothrow_invocable<Functor>::value)
        {
            // call functor and explicitly ignore return value
            static_cast<void>(m_func());
        }
        // Functor may throw, swallow any exception
        else
        {
            try
            {
                static_cast<void>(m_func());
                return;
            }
            catch (std::exception const& e)
            {
                detail::logCriticalException(__FUNCTION__, e.what());
            }
            catch (...)
            {
                detail::logCriticalException(__FUNCTION__);
            }
#ifndef GT_FINALLY_DISABLE_ASSERT
            assert(!"gt::Finally threw an exception");
#endif
        }
    }

    /// copies/moves functor object safely
    static constexpr decltype(auto) take(Functor& f) noexcept
    {
        if constexpr (std::is_nothrow_move_constructible<Functor>::value)
        {
            // move
            return std::move(f);
        }
        else
        {
            // copy
            return static_cast<const Functor&>(f);
        }
    }
};

/**
 * @brief Makes a finally object from a functor (e.g. a lambda). The functor
 * must be move or copy constructibe and gurantee not to throw (use noexcept).
 *
 * Usage:
 *
 * {
 *     QLockFile lockFile{...};
 *
 *     auto cleanup = gt::finally([&]() noexcept {
 *         lockFile.unlock();
 *     });
 * }
 *
 * @param t object
 * @param func Function to call on cleanup
 */
template <typename Functor,
          typename = std::enable_if_t<std::is_invocable<Functor>::value>>
inline auto finally(Functor func)
    noexcept(std::is_nothrow_move_constructible<Functor>::value)
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
 * @tparam U Base class in which the member method is implemented
 * (defaults to T)
 */
template <typename T, typename F,
          typename = std::enable_if_t<std::is_member_function_pointer<F>::value &&
                                      std::is_invocable<F, T*>::value>>
inline auto finally(T* t, F f) noexcept
{
    // construct helper lambda
    return finally([t, f]() noexcept(std::is_nothrow_invocable<F, T*>::value) {
        if (t && f) static_cast<void>(std::invoke(f, t));
    });
}

} // namespace gt

#endif // GT_FINALLY_H
