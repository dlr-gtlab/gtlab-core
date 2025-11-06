/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 * Source File: gt_doubleproperty.h
 *
 *  Created on: 06.11.2025
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#ifndef GT_OPTIONAL_H
#define GT_OPTIONAL_H

#include <type_traits>
#include <utility>
#include <initializer_list>
#include <stdexcept>

namespace gt {

    // ===== nullopt =====
    struct nullopt_t { struct tag {}; constexpr explicit nullopt_t(tag) {} };
    constexpr nullopt_t nullopt{ nullopt_t::tag{} };

    // ===== in_place =====
    struct in_place_t { struct tag {}; constexpr explicit in_place_t(tag) {} };
    constexpr in_place_t in_place{ in_place_t::tag{} };

    // ===== exception =====
    struct bad_optional_access : std::logic_error
    {
        bad_optional_access() : std::logic_error("bad optional access") {}
    };

    // ===== optional storage helper =====
    template <typename T>
    struct opt_storage
    {
        union {
            char dummy_;
            T value_;
        };
        bool engaged_;

        opt_storage() noexcept : dummy_(), engaged_(false) {}
        ~opt_storage() {}

        template <class... Args>
        void construct(Args&&... args)
        {
            ::new (static_cast<void*>(std::addressof(value_))) T(std::forward<Args>(args)...);
            engaged_ = true;
        }

        void destroy() noexcept
        {
            if (engaged_)
            {
                value_.~T();
                engaged_ = false;
            }
        }
    };

    // ===== optional<T> =====
    template <typename T>
    class optional {
        static_assert(!std::is_same<typename std::decay<T>::type, nullopt_t>::value, "T cannot be nullopt_t");
        static_assert(!std::is_reference<T>::value, "optional<T&> is intentionally not supported");

    public:
        using value_type = T;

        // constructors
        constexpr optional() noexcept : storage_() {}
        constexpr optional(nullopt_t) noexcept : storage_() {}

        optional(const T& v)
        {
            storage_.construct(v);
        }
        optional(T&& v)
        {
            storage_.construct(std::move(v));
        }

        template <class... Args>
        explicit optional(in_place_t, Args&&... args)
        {
            storage_.construct(std::forward<Args>(args)...);
        }

        template <class U, class... Args>
        explicit optional(in_place_t, std::initializer_list<U> il, Args&&... args)
        {
            storage_.construct(il, std::forward<Args>(args)...);
        }

        // copy/move
        optional(const optional& other)
        {
            if (other.storage_.engaged_)
            {
                storage_.construct(other.storage_.value_);
            }
        }

        optional(optional&& other) noexcept(std::is_nothrow_move_constructible<T>::value)
        {
            if (other.storage_.engaged_)
            {
                storage_.construct(std::move(other.storage_.value_));
            }
        }

        ~optional()
        {
            storage_.destroy();
        }

        optional& operator=(nullopt_t) noexcept
        {
            reset();
            return *this;
        }

        optional& operator=(const optional& other)
        {
            if (this == &other) return *this;
            if (storage_.engaged_ && other.storage_.engaged_)
            {
                storage_.value_ = other.storage_.value_;
            }
            else if (other.storage_.engaged_)
            {
                storage_.construct(other.storage_.value_);
            }
            else
            {
                reset();
            }
            return *this;
        }

        optional& operator=(optional&& other)
            noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value)
        {
            if (this == &other) return *this;
            if (storage_.engaged_ && other.storage_.engaged_)
            {
                storage_.value_ = std::move(other.storage_.value_);
            }
            else if (other.storage_.engaged_)
            {
                storage_.destroy();
                storage_.construct(std::move(other.storage_.value_));
            }
            else
            {
                reset();
            }
            return *this;
        }

        optional& operator=(const T& v)
        {
            if (storage_.engaged_) storage_.value_ = v;
            else storage_.construct(v);
            return *this;
        }

        optional& operator=(T&& v)
        {
            if (storage_.engaged_) storage_.value_ = std::move(v);
            else storage_.construct(std::move(v));
            return *this;
        }

        // observers
        constexpr bool has_value() const noexcept { return storage_.engaged_; }
        constexpr explicit operator bool() const noexcept { return has_value(); }

        T& value() &
        {
            if (!storage_.engaged_) throw bad_optional_access();
            return storage_.value_;
        }
        const T& value() const &
        {
            if (!storage_.engaged_) throw bad_optional_access();
            return storage_.value_;
        }
        T&& value() &&
        {
            if (!storage_.engaged_) throw bad_optional_access();
            return std::move(storage_.value_);
        }
        const T&& value() const &&
        {
            if (!storage_.engaged_) throw bad_optional_access();
            return std::move(storage_.value_);
        }

        template <class U>
        T value_or(U&& default_value) const &
        {
            return storage_.engaged_ ? storage_.value_ : static_cast<T>(std::forward<U>(default_value));
        }
        template <class U>
        T value_or(U&& default_value) &&
        {
            return storage_.engaged_ ? std::move(storage_.value_) : static_cast<T>(std::forward<U>(default_value));
        }

        // pointer-like
        T* operator->()
        {
            return std::addressof(value());
        }
        const T* operator->() const {
            return std::addressof(value());
        }
        T& operator*() & { return value(); }
        const T& operator*() const & { return value(); }
        T&& operator*() && { return std::move(value()); }
        const T&& operator*() const && { return std::move(value()); }

        // modifiers
        void reset() noexcept { storage_.destroy(); }

        template <class... Args>
        T& emplace(Args&&... args)
        {
            storage_.destroy();
            storage_.construct(std::forward<Args>(args)...);
            return storage_.value_;
        }

        template <class U, class... Args>
        T& emplace(std::initializer_list<U> il, Args&&... args)
        {
            storage_.destroy();
            storage_.construct(il, std::forward<Args>(args)...);
            return storage_.value_;
        }

        void swap(optional& other)
            noexcept(std::is_nothrow_move_constructible<T>::value && noexcept(std::swap(std::declval<T&>(), std::declval<T&>())))
        {
            using std::swap;
            if (storage_.engaged_ && other.storage_.engaged_)
            {
                swap(storage_.value_, other.storage_.value_);
            }
            else if (storage_.engaged_)
            {
                other.storage_.construct(std::move(storage_.value_));
                storage_.destroy();
            }
            else if (other.storage_.engaged_)
            {
                storage_.construct(std::move(other.storage_.value_));
                other.storage_.destroy();
            }
        }

    private:
        opt_storage<T> storage_;
    };

    // ===== helpers =====
    template <class T>
    optional<typename std::decay<T>::type> make_optional(T&& v) {
        return optional<typename std::decay<T>::type>(std::forward<T>(v));
    }

    template <class T, class... Args>
    optional<T> make_optional(Args&&... args) {
        return optional<T>(in_place, std::forward<Args>(args)...);
    }

    // ===== comparisons =====
    template <class T, class U>
    inline bool operator==(const optional<T>& a, const optional<U>& b) {
        if (bool(a) != bool(b)) return false;
        return !a || (*a == *b);
    }
    template <class T, class U>
    inline bool operator!=(const optional<T>& a, const optional<U>& b) { return !(a == b); }

    template <class T>
    inline bool operator==(const optional<T>& a, nullopt_t) noexcept { return !a; }
    template <class T>
    inline bool operator==(nullopt_t, const optional<T>& a) noexcept { return !a; }
    template <class T>
    inline bool operator!=(const optional<T>& a, nullopt_t) noexcept { return bool(a); }
    template <class T>
    inline bool operator!=(nullopt_t, const optional<T>& a) noexcept { return bool(a); }

    template <class T, class U>
    inline bool operator==(const optional<T>& a, const U& b) { return a && (*a == b); }
    template <class T, class U>
    inline bool operator==(const U& a, const optional<T>& b) { return b && (a == *b); }
    template <class T, class U>
    inline bool operator!=(const optional<T>& a, const U& b) { return !(a == b); }
    template <class T, class U>
    inline bool operator!=(const U& a, const optional<T>& b) { return !(a == b); }

    // (Optional) ordering: engaged > disengaged; then value ordering
    template <class T, class U>
    inline bool operator<(const optional<T>& a, const optional<U>& b) {
        if (!b) return false;
        if (!a) return true;
        return *a < *b;
    }
    template <class T, class U>
    inline bool operator>(const optional<T>& a, const optional<U>& b) { return b < a; }
    template <class T, class U>
    inline bool operator<=(const optional<T>& a, const optional<U>& b) { return !(b < a); }
    template <class T, class U>
    inline bool operator>=(const optional<T>& a, const optional<U>& b) { return !(a < b); }

} // namespace gt

/*
Usage:

#include "mini_optional.hpp"
#include <iostream>
#include <string>

int main() {
    mini::optional<std::string> o;           // empty
    o = "hello";
    if (o) std::cout << *o << "\n";          // prints "hello"

    auto p = mini::make_optional<std::string>(3, 'x'); // "xxx"
    std::cout << p.value_or("fallback") << "\n";

    o.reset();
    try {
        std::cout << o.value();              // throws bad_optional_access
    } catch (const mini::bad_optional_access&){}

    mini::optional<int> a = 42, b;
    std::cout << (a == 42) << " " << (b == mini::nullopt) << "\n";
}
*/

#endif // GT_OPTIONAL_H
