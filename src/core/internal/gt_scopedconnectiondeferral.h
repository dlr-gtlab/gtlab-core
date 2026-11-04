/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_SCOPEDCONNECTIONDEFERRAL_H
#define GT_SCOPEDCONNECTIONDEFERRAL_H

namespace gt
{
namespace detail
{

/**
 * Defer automatic task connections while restoring objects for a deep copy.
 * Synchronous restores in this scope inherit deferral; keep the scope limited
 * to restoring the copied hierarchy. Explicit connection transfer is unaffected.
 */
class ScopedConnectionDeferral
{
public:
    ScopedConnectionDeferral() noexcept
    {
        ++depth();
    }
    ~ScopedConnectionDeferral()
    {
        --depth();
    }

    ScopedConnectionDeferral(const ScopedConnectionDeferral&) = delete;
    ScopedConnectionDeferral& operator=(const ScopedConnectionDeferral&) =
        delete;

    static bool isActive() noexcept
    {
        return depth() != 0;
    }

private:
    static unsigned& depth() noexcept
    {
        static thread_local unsigned value = 0;
        return value;
    }
};

} // namespace detail
} // namespace gt

#endif // GT_SCOPEDCONNECTIONDEFERRAL_H
