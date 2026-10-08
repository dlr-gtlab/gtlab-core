/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_abstractcalculatorexecutor.cpp
 */

#include "gtest/gtest.h"

#include "gt_abstractcalculatorexecutor.h"
#include "gt_calculator.h"
#include "gt_task.h"

namespace
{

    /// Configurable result returned by SpyGtTask::runIteration().
    ///
    /// Held in a file-scope flag (rather than a second data member) so that the
    /// spy keeps a single call-counter member.
    static bool g_runIterationResult = true;

    /// GtTask spy that counts how many times the execution path invokes
    /// runIteration().
    class SpyGtTask : public GtTask
    {
    public:
        int runIterationCalls = 0;

        bool runIteration() override
        {
            ++runIterationCalls;
            return g_runIterationResult;
        }
    };

    /// Executor that does NOT override exec(GtTask*). It only satisfies the pure
    /// virtual exec(GtCalculator*) so that the inherited default exec(GtTask*)
    /// is the implementation actually used.
    class DefaultTaskExecutor : public GtAbstractCalculatorExecutor
    {
    public:
        bool exec(GtCalculator* calculator) override
        {
            (void)calculator;
            return true;
        }

        // Re-introduce the base class' exec(GtTask*); the overload declared above
        // would hide it otherwise.
        using GtAbstractCalculatorExecutor::exec;
    };

    /// Executor that provides its own exec(GtTask*) implementation instead of the
    /// inherited default.
    class CustomTaskExecutor : public GtAbstractCalculatorExecutor
    {
    public:
        int taskExecCalls = 0;
        bool taskExecResult = true;

        bool exec(GtCalculator* calculator) override
        {
            (void)calculator;
            return true;
        }

        bool exec(GtTask* task) override
        {
            ++taskExecCalls;
            (void)task;
            return taskExecResult;
        }
    };

} // namespace

// ---------------------------------------------------------------------------
// exec(GtTask*)
// ---------------------------------------------------------------------------

// The default implementation rejects a null task.
TEST(GtAbstractCalculatorExecutor, execTaskNullTaskReturnsFalse)
{
    DefaultTaskExecutor executor;

    EXPECT_FALSE(executor.exec(static_cast<GtTask*>(nullptr)));
}

// The default exec(GtTask*) delegates to task->runIteration() and returns its
// result.
TEST(GtAbstractCalculatorExecutor, execTaskDefaultPathDelegatesToRunIteration)
{
    DefaultTaskExecutor executor;
    SpyGtTask task;
    g_runIterationResult = true;

    EXPECT_TRUE(executor.exec(&task));
    EXPECT_EQ(task.runIterationCalls, 1);
}

// The default exec(GtTask*) propagates a failing runIteration().
TEST(GtAbstractCalculatorExecutor,
     execTaskDefaultPathPropagatesRunIterationFailure)
{
    DefaultTaskExecutor executor;
    SpyGtTask task;
    g_runIterationResult = false;

    EXPECT_FALSE(executor.exec(&task));
    EXPECT_EQ(task.runIterationCalls, 1);
}

// An executor with its own exec(GtTask*) override runs that implementation
// instead of the inherited default (which would call runIteration()).
TEST(GtAbstractCalculatorExecutor, execTaskCustomImplBypassesDefaultPath)
{
    CustomTaskExecutor executor;
    SpyGtTask task;
    executor.taskExecResult = true;

    EXPECT_TRUE(executor.exec(&task));
    EXPECT_EQ(executor.taskExecCalls, 1);
    // The inherited default path (runIteration) must not have been taken.
    EXPECT_EQ(task.runIterationCalls, 0);
}

// The result of a custom exec(GtTask*) implementation is returned as-is.
TEST(GtAbstractCalculatorExecutor, execTaskCustomImplPropagatesFailure)
{
    CustomTaskExecutor executor;
    SpyGtTask task;
    executor.taskExecResult = false;

    EXPECT_FALSE(executor.exec(&task));
    EXPECT_EQ(executor.taskExecCalls, 1);
}
