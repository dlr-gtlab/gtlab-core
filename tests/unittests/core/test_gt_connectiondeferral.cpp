/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"
#include "internal/gt_scopedconnectiondeferral.h"

#include <stdexcept>
#include <thread>

TEST(TestGtConnectionDeferral, nestedScopesPreserveOuterDeferral)
{
    EXPECT_FALSE(gt::detail::ScopedConnectionDeferral::isActive());
    {
        gt::detail::ScopedConnectionDeferral outer;
        EXPECT_TRUE(gt::detail::ScopedConnectionDeferral::isActive());
        {
            gt::detail::ScopedConnectionDeferral inner;
            EXPECT_TRUE(gt::detail::ScopedConnectionDeferral::isActive());
        }
        EXPECT_TRUE(gt::detail::ScopedConnectionDeferral::isActive());
    }
    EXPECT_FALSE(gt::detail::ScopedConnectionDeferral::isActive());
}

TEST(TestGtConnectionDeferral, earlyReturnRestoresState)
{
    auto restore = [] {
        gt::detail::ScopedConnectionDeferral deferConnections;
        return gt::detail::ScopedConnectionDeferral::isActive();
    };
    EXPECT_TRUE(restore());
    EXPECT_FALSE(gt::detail::ScopedConnectionDeferral::isActive());
}

TEST(TestGtConnectionDeferral, exceptionRestoresState)
{
    EXPECT_THROW(
        {
            gt::detail::ScopedConnectionDeferral deferConnections;
            throw std::runtime_error("restore failed");
        },
        std::runtime_error);
    EXPECT_FALSE(gt::detail::ScopedConnectionDeferral::isActive());
}

TEST(TestGtConnectionDeferral, otherThreadsDoNotInheritDeferral)
{
    gt::detail::ScopedConnectionDeferral deferConnections;
    bool activeOnOtherThread = true;
    std::thread worker([&activeOnOtherThread] {
        activeOnOtherThread = gt::detail::ScopedConnectionDeferral::isActive();
    });
    worker.join();
    EXPECT_FALSE(activeOnOtherThread);
    EXPECT_TRUE(gt::detail::ScopedConnectionDeferral::isActive());
}
