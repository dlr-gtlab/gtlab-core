/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_headlessprojectruntime.h"

TEST(GtHeadlessTaskStatus, InvalidStatusIsNotDone)
{
    GtHeadlessTaskStatus status;

    EXPECT_FALSE(status.isDone());
    EXPECT_EQ(status.state, GtHeadlessTaskStatus::State::Invalid);
}

TEST(GtHeadlessTaskStatus, TerminalStatesAreDone)
{
    for (const auto state : {GtHeadlessTaskStatus::State::Finished,
                             GtHeadlessTaskStatus::State::Failed,
                             GtHeadlessTaskStatus::State::Cancelled,
                             GtHeadlessTaskStatus::State::Shutdown})
    {
        GtHeadlessTaskStatus status;
        status.state = state;
        EXPECT_TRUE(status.isDone());
    }
}

TEST(GtHeadlessTaskHandle, DefaultHandleIsInvalid)
{
    GtHeadlessTaskHandle handle;

    EXPECT_FALSE(handle.isValid());
    EXPECT_TRUE(handle.id().isEmpty());
    EXPECT_FALSE(handle.status().isDone());
    EXPECT_FALSE(handle.cancel());
}

TEST(GtHeadlessProjectRuntime, StartsInCreatedState)
{
    GtHeadlessProjectRuntime runtime;

    EXPECT_EQ(runtime.state(), GtHeadlessProjectRuntime::State::Created);
    EXPECT_TRUE(runtime.projectPath().isEmpty());
}

TEST(GtHeadlessProjectRuntime, RejectsProjectBeforeInitialization)
{
    GtHeadlessProjectRuntime runtime;
    GtHeadlessRuntimeResult result = runtime.openProject(QStringLiteral("missing"));

    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::InvalidState);
    EXPECT_FALSE(result.succeeded());
}

TEST(GtHeadlessProjectRuntime, ReportsMissingCoreServices)
{
    GtHeadlessProjectRuntime runtime;
    const auto result = runtime.initialize();

    EXPECT_EQ(result.code, GtHeadlessRuntimeResult::Code::CoreUnavailable);
    EXPECT_FALSE(result.succeeded());
    EXPECT_EQ(runtime.state(), GtHeadlessProjectRuntime::State::Created);
}
