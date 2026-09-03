/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <memory>
#include <thread>

#include "gt_executableoperation.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectgroup.h"

namespace
{

class TestEventSink : public GtExecutionEventSink
{
public:
    void publish(QString const& eventType) override
    {
        publishedEvents << eventType;
    }

    QStringList publishedEvents;
};

class TestOperation : public GtExecutableOperation
{
    Q_OBJECT
    Q_PROPERTY(int savedValue READ savedValue WRITE setSavedValue)

public:
    Q_INVOKABLE explicit TestOperation(GtObject* parent = nullptr) :
        GtExecutableOperation(parent)
    {
    }

    int savedValue() const
    {
        return m_savedValue;
    }

    void setSavedValue(int value)
    {
        m_savedValue = value;
    }

    bool requiresProject() const override
    {
        return false;
    }

    std::unique_ptr<GtObject>
    createData(GtExecutionContext const&) const override
    {
        return std::make_unique<GtObject>();
    }

    std::unique_ptr<GtObject>
    execute(GtOperationExecutionContext& context) override
    {
        observedData = context.data() != nullptr;
        observedCancellation = context.cancellation().isCancellationRequested();
        observedExecutionId = context.executionId().toString();
        context.events().publish(QStringLiteral("test.operation.executed"));
        return std::make_unique<GtObject>();
    }

    GtOperationApplyResult
    applyResult(GtObject const* result, GtExecutionContext&) const override
    {
        return result ? GtOperationApplyResult::success() :
                        GtOperationApplyResult::failure(
                            QStringLiteral("Missing result"));
    }

    bool observedData {false};
    bool observedCancellation {false};
    QString observedExecutionId;

private:
    int m_savedValue {0};
};

class RegisteredTestOperation
{
public:
    RegisteredTestOperation()
    {
        gtObjectFactory->registerClass(GT_METADATA(TestOperation));
    }

    ~RegisteredTestOperation()
    {
        gtObjectFactory->unregisterClass(GT_METADATA(TestOperation));
    }
};

} // namespace

TEST(GtExecutableOperation, roundtripReconstructsAndExecutesOperation)
{
    RegisteredTestOperation registration;
    auto original = std::make_unique<TestOperation>();
    original->setSavedValue(42);
    GtObjectMemento memento = original->toMemento();
    original.reset();

    auto reconstructed = memento.toObject(*gtObjectFactory);
    auto* operation = qobject_cast<GtExecutableOperation*>(reconstructed.get());
    ASSERT_NE(operation, nullptr);

    auto* testOperation = static_cast<TestOperation*>(operation);
    EXPECT_EQ(testOperation->savedValue(), 42);

    auto data = testOperation->createData(GtExecutionContext{});
    TestEventSink events;
    GtCancellationToken cancellation;
    std::thread cancellationThread([&]() { cancellation.requestCancellation(); });
    cancellationThread.join();
    GtOperationExecutionContext context(data.get(), events, cancellation);

    const QString executionId = context.executionId().toString();
    auto result = testOperation->execute(context);

    EXPECT_TRUE(testOperation->observedData);
    EXPECT_TRUE(testOperation->observedCancellation);
    EXPECT_EQ(testOperation->observedExecutionId, executionId);
    EXPECT_EQ(events.publishedEvents,
              QStringList{QStringLiteral("test.operation.executed")});
    EXPECT_NE(result, nullptr);
    GtExecutionContext clientContext;
    EXPECT_TRUE(testOperation->applyResult(result.get(), clientContext).isSuccess());
}

TEST(GtExecutableOperation, rejectsReconstructedNonOperationBeforeExecution)
{
    GtObjectGroup object;
    GtObjectMemento memento = object.toMemento();

    auto reconstructed = memento.toObject(*gtObjectFactory);
    ASSERT_NE(reconstructed, nullptr);
    EXPECT_EQ(qobject_cast<GtExecutableOperation*>(reconstructed.get()), nullptr);
}

TEST(GtOperationApplyResult, exposesStructuredFailure)
{
    const auto success = GtOperationApplyResult::success();
    const auto failure = GtOperationApplyResult::failure(QStringLiteral("error"));

    EXPECT_TRUE(success.isSuccess());
    EXPECT_FALSE(failure.isSuccess());
    EXPECT_EQ(failure.errorMessage(), QStringLiteral("error"));
}

#include "test_gt_executableoperation.moc"
