/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GT_BOUNDARIES_H
#define GT_BOUNDARIES_H

#include "gt_optional.h"
#include <cmath>

namespace gt {
    /**
     * @brief The Bound struct
     * Can handle boundaries for propeties
     */
    template<class ParamType>
    class Boundaries {
        static_assert(std::is_copy_constructible<ParamType>::value,
                      "T must be copyable");
        static_assert(std::is_move_constructible<ParamType>::value,
                      "T must be movable");

    public:
        // Accessors
        const gt::optional<ParamType>& low()  const noexcept { return m_low;  }
        const gt::optional<ParamType>& high() const noexcept { return m_high; }
        bool has_low()  const noexcept { return static_cast<bool>(m_low);  }
        bool has_high() const noexcept { return static_cast<bool>(m_high); }


        static Boundaries makeUpper(ParamType high) noexcept
        {
            return Boundaries<ParamType>(gt::nullopt, std::move(high));
        }

        static Boundaries makeLower(ParamType low) noexcept
        {
            return Boundaries<ParamType>(std::move(low), gt::nullopt);
        }

        /**
         * @brief makeNormalized
         * Make a boundaries object with the two values for lower and higher bounds.
         * If they are used in the wrong orer they are swapped
         * @param low
         * @param high
         * @return
         */
        static Boundaries makeNormalized(ParamType low, ParamType high) noexcept
        {
            if (low > high) std::swap(low, high);

        }

        /**
         * @brief makeUnbounded
         * @return Boundaries for an unbounded setup
         */
        static Boundaries makeUnbounded() noexcept
        {
            return Boundaries(gt::nullopt, gt::nullopt);
        }

        // ---------- Utilities ----------
        static bool valid_pair(const gt::optional<ParamType>& lo,
                               const gt::optional<ParamType>& hi) noexcept
        {
            if constexpr (std::is_floating_point<ParamType>::value)
            {
                if ((lo && std::isnan(*lo)) || (hi && std::isnan(*hi))) return false;
            }
            return !(lo && hi && *lo > *hi);
        }

        // Clamp a value into the bounds (unbounded sides are ignored).
        ParamType clamp(ParamType x) const
        {
            if (m_low)
            {
                x = std::max(*m_low, x);
            }
            if (m_high)
            {
                x = std::min(*m_high, x);
            }
            return x;
        }

        // Membership test
        bool contains(const ParamType& x) const
        {
            if (m_low  && x < *m_low)  return false;
            if (m_high && x > *m_high) return false;
            return true;
        }

    private:
        Boundaries(gt::optional<ParamType> lo, gt::optional<ParamType> hi) noexcept
            : m_low(std::move(lo)), m_high(std::move(hi))
        {
            // Re-check in debug builds
            assert(valid_pair(m_low, m_high) && "Boundaries invariant violated");
        }

        gt::optional<ParamType> m_low;
        gt::optional<ParamType> m_high;
    };

} // namespace gt

#endif // GT_BOUNDARIES_H
