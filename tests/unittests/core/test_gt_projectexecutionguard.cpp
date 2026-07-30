/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_project.h"
#include "gt_projectexecutionguard.h"

namespace
{
class TestProject : public GtProject
{
public:
    explicit TestProject(QString path) : GtProject(std::move(path)) {}
};
}

TEST(GtProjectExecutionGuard, UsesCanonicalProjectIdentity)
{
    TestProject first(QStringLiteral("/tmp/gtlab-project/./project.gtlab"));
    TestProject second(QStringLiteral("/tmp/gtlab-project/project.gtlab"));

    EXPECT_EQ(GtProjectExecutionGuard::projectKey(&first),
              GtProjectExecutionGuard::projectKey(&second));
}

TEST(GtProjectExecutionGuard, RejectsNullProjects)
{
    TestProject project(QString{});
    GtProjectExecutionGuard guard;

    EXPECT_TRUE(GtProjectExecutionGuard::projectKey(nullptr).isEmpty());
    EXPECT_FALSE(GtProjectExecutionGuard::projectKey(&project).isEmpty());
    EXPECT_EQ(guard.tryAcquire(nullptr),
              GtProjectExecutionGuard::Result::InvalidProject);
    EXPECT_EQ(guard.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Acquired);
    EXPECT_TRUE(guard.isHeld());
    EXPECT_FALSE(guard.key().isEmpty());
    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(nullptr));
    EXPECT_TRUE(GtProjectExecutionGuard::isBusy(&project));
}

TEST(GtProjectExecutionGuard, SerializesPathlessProjects)
{
    TestProject project(QString{});
    GtProjectExecutionGuard first;
    GtProjectExecutionGuard second;

    ASSERT_EQ(first.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Acquired);
    EXPECT_EQ(second.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Busy);
}

TEST(GtProjectExecutionGuard, ReacquiringReleasesPreviousProject)
{
    TestProject first(QStringLiteral("/tmp/gtlab-project-reacquire-a.gtlab"));
    TestProject second(QStringLiteral("/tmp/gtlab-project-reacquire-b.gtlab"));
    GtProjectExecutionGuard guard;

    ASSERT_EQ(guard.tryAcquire(&first),
              GtProjectExecutionGuard::Result::Acquired);
    ASSERT_TRUE(guard.isHeld());
    ASSERT_EQ(guard.tryAcquire(&second),
              GtProjectExecutionGuard::Result::Acquired);

    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(&first));
    EXPECT_TRUE(GtProjectExecutionGuard::isBusy(&second));
    EXPECT_EQ(guard.key(),
              GtProjectExecutionGuard::projectKey(&second));

    guard.release();
    guard.release();
    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(&second));
}

TEST(GtProjectExecutionGuard, SerializesSameProjectAndReleases)
{
    TestProject project(QStringLiteral("/tmp/gtlab-project.gtlab"));
    GtProjectExecutionGuard first;
    GtProjectExecutionGuard second;

    EXPECT_EQ(first.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Acquired);
    EXPECT_TRUE(GtProjectExecutionGuard::isBusy(&project));
    EXPECT_EQ(second.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Busy);

    first.release();
    EXPECT_FALSE(GtProjectExecutionGuard::isBusy(&project));
    EXPECT_EQ(second.tryAcquire(&project),
              GtProjectExecutionGuard::Result::Acquired);
}

TEST(GtProjectExecutionGuard, DoesNotSerializeDifferentProjects)
{
    TestProject first(QStringLiteral("/tmp/gtlab-project-a.gtlab"));
    TestProject second(QStringLiteral("/tmp/gtlab-project-b.gtlab"));
    GtProjectExecutionGuard firstGuard;
    GtProjectExecutionGuard secondGuard;

    EXPECT_EQ(firstGuard.tryAcquire(&first),
              GtProjectExecutionGuard::Result::Acquired);
    EXPECT_EQ(secondGuard.tryAcquire(&second),
              GtProjectExecutionGuard::Result::Acquired);
}
