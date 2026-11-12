/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_makepropertyconnection.h"
#include "gt_propertyconnection.h"
#include "test_gt_processtestclasses.h"

class TestGtMakePropertyConnection : public ::testing::Test
{
protected:
    void SetUp() override
    {
        roottask = new TestTaskWithProperty;
        calc1 = new TestGtCalculator;
        calc2 = new TestGtCalculator;
        roottask->appendChild(calc1);
        roottask->appendChild(calc2);
    }

    void TearDown() override
    {
        delete roottask;
    }

    TestTaskWithProperty* roottask = nullptr;
    TestGtCalculator* calc1 = nullptr;
    TestGtCalculator* calc2 = nullptr;
};

TEST_F(TestGtMakePropertyConnection, makeConnectionRejectsInvalidInputs)
{
    struct InvalidInput
    {
        GtProcessComponent* source;
        const char* sourceProperty;
        GtProcessComponent* target;
        const char* targetProperty;
    };
    const InvalidInput cases[] = {
        {calc1, "missing", calc2, "doubleProp3"},
        {calc1, "doubleProp2", calc2, "missing"},
        {calc1, "missing", calc2, "missing"},
    };
    const auto connectionCount =
        roottask->findChildren<GtPropertyConnection*>().size();

    for (const auto& input : cases)
    {
        SCOPED_TRACE(input.sourceProperty);
        SCOPED_TRACE(input.targetProperty);

        auto* connection = gt::utils::process::makePropertyConnection(
            *(input.source), input.sourceProperty, *(input.target),
            input.targetProperty);

        EXPECT_EQ(connection, nullptr);
        EXPECT_EQ(roottask->findChildren<GtPropertyConnection*>().size(),
                  connectionCount);
    }
}

TEST_F(TestGtMakePropertyConnection, makeConnectionRequiresRootTask)
{
    TestGtCalculator standaloneSource;
    TestGtCalculator standaloneTarget;

    EXPECT_EQ(gt::utils::process::makePropertyConnection(
                  standaloneSource, "doubleProp2", standaloneTarget,
                  "doubleProp3"),
              nullptr);
    EXPECT_TRUE(
        standaloneSource.findChildren<GtPropertyConnection*>().isEmpty());
}

TEST_F(TestGtMakePropertyConnection, failedConnectionLeavesNoChild)
{
    TestTaskWithProperty otherRoot;
    const auto connectionCount =
        roottask->findChildren<GtPropertyConnection*>().size();

    EXPECT_EQ(gt::utils::process::makePropertyConnection(
                  *calc1, "doubleProp2", otherRoot, "doubleProp3"),
              nullptr);
    EXPECT_EQ(roottask->findChildren<GtPropertyConnection*>().size(),
              connectionCount);
    EXPECT_TRUE(otherRoot.findChildren<GtPropertyConnection*>().isEmpty());
}

TEST_F(TestGtMakePropertyConnection, makeConnectionReportsSuccess)
{
    auto* connection = gt::utils::process::makePropertyConnection(
        *calc1, "doubleProp2", *calc2, "doubleProp3");

    ASSERT_NE(connection, nullptr);
    EXPECT_TRUE(connection->isConnected());
    EXPECT_EQ(connection->parent(), roottask);
    EXPECT_EQ(connection->sourceUuid(), calc1->uuid());
    EXPECT_EQ(connection->targetUuid(), calc2->uuid());
    EXPECT_EQ(connection->sourceProp(), "doubleProp2");
    EXPECT_EQ(connection->targetProp(), "doubleProp3");
}
