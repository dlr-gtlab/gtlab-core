/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <future>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>

#include "gt_executioncontext.h"
#include "gt_project.h"

static_assert(!std::is_constructible_v<GtExecutionContextScope,
                                       GtExecutionContext&&>);

namespace
{
class TestProject : public GtProject
{
public:
    explicit TestProject(QString path) : GtProject(std::move(path)) {}
};
}

TEST(GtExecutionContext, storesProjectData)
{
    GtExecutionContext context(nullptr, QStringLiteral("/project"));

    EXPECT_EQ(context.project(), nullptr);
    EXPECT_EQ(context.projectPath(), QStringLiteral("/project"));
    EXPECT_TRUE(context.isValid());
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST(GtExecutionContext, projectOnlyContextIsValid)
{
    TestProject project(QString{});
    GtExecutionContext context(&project);

    EXPECT_TRUE(context.isValid());
    EXPECT_EQ(context.project(), &project);
    EXPECT_EQ(context.projectPath(), project.path());
}

TEST(GtExecutionContext, emptyContextIsInvalid)
{
    GtExecutionContext context;

    EXPECT_FALSE(context.isValid());
}

TEST(GtExecutionContext, scopeInstallsAndRestoresContext)
{
    GtExecutionContext context(nullptr, QStringLiteral("/project"));

    EXPECT_EQ(GtExecutionContext::current(), nullptr);
    {
        GtExecutionContextScope scope(context);
        EXPECT_EQ(GtExecutionContext::current(), &context);
        EXPECT_EQ(GtExecutionContext::currentContext(), &context);
    }
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST(GtExecutionContext, nestedScopesRestorePreviousContext)
{
    GtExecutionContext outer(nullptr, QStringLiteral("/outer"));
    GtExecutionContext inner(nullptr, QStringLiteral("/inner"));

    GtExecutionContextScope outerScope(outer);
    EXPECT_EQ(GtExecutionContext::current(), &outer);
    {
        GtExecutionContextScope innerScope(inner);
        EXPECT_EQ(GtExecutionContext::current(), &inner);
    }
    EXPECT_EQ(GtExecutionContext::current(), &outer);
}

TEST(GtExecutionContext, scopeRestoresAfterEarlyReturn)
{
    GtExecutionContext context(nullptr, QStringLiteral("/project"));

    const auto installAndReturn = [&context]() {
        GtExecutionContextScope scope(context);
        return GtExecutionContext::current();
    };

    EXPECT_EQ(installAndReturn(), &context);
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST(GtExecutionContext, scopeRestoresAfterException)
{
    GtExecutionContext context(nullptr, QStringLiteral("/project"));

    try
    {
        GtExecutionContextScope scope(context);
        throw std::runtime_error("test");
    }
    catch (std::runtime_error const&)
    {
    }

    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}

TEST(GtExecutionContext, contextsAreIsolatedBetweenThreads)
{
    GtExecutionContext first(nullptr, QStringLiteral("/first"));
    GtExecutionContext second(nullptr, QStringLiteral("/second"));
    std::promise<void> firstReady;
    std::promise<void> releaseFirst;
    auto firstReadyFuture = firstReady.get_future();
    auto releaseFirstFuture = releaseFirst.get_future();
    GtExecutionContext const* observedByFirst = nullptr;
    GtExecutionContext const* observedBySecond = nullptr;

    std::thread firstThread([&]() {
        GtExecutionContextScope scope(first);
        observedByFirst = GtExecutionContext::current();
        firstReady.set_value();
        releaseFirstFuture.wait();
    });

    firstReadyFuture.wait();
    EXPECT_EQ(GtExecutionContext::current(), nullptr);

    std::thread secondThread([&]() {
        GtExecutionContextScope scope(second);
        observedBySecond = GtExecutionContext::current();
    });

    secondThread.join();
    releaseFirst.set_value();
    firstThread.join();

    EXPECT_EQ(observedByFirst, &first);
    EXPECT_EQ(observedBySecond, &second);
    EXPECT_EQ(GtExecutionContext::current(), nullptr);
}
