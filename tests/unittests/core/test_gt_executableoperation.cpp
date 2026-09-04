/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <memory>

#include <QCoreApplication>
#include <thread>

#include "gt_executableoperation.h"
#include "gt_coreapplication.h"
#include "gt_moduleinterface.h"
#include "gt_operationinterface.h"
#include "gt_processmoduleloader.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectgroup.h"

namespace
{

class TestEventSink : public GtExecutionEventSink
{
public:
    void publish(QString, QJsonValue = {}) override { ++publishedEvents; }
    void publish(QString, GtObject const&) override { ++publishedEvents; }

    int publishedEvents = 0;
};

class TestNonOperation : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit TestNonOperation(GtObject* parent = nullptr) :
        GtObject(parent)
    {
    }
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
        context.events().publish(QStringLiteral("test"));
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

class TestOperationModule : public QObject,
                            public GtModuleInterface,
                            public GtOperationInterface
{
    Q_OBJECT
    Q_INTERFACES(GtModuleInterface GtOperationInterface)

public:
    explicit TestOperationModule(QList<QMetaObject> operations) :
        m_operations(std::move(operations))
    {
    }

    QString ident() const override
    {
        return QStringLiteral("TestOperationModule");
    }

    GtVersionNumber version() override
    {
        return {1, 0, 0};
    }

    QString description() const override
    {
        return {};
    }

    QList<QMetaObject> operations() const override
    {
        return m_operations;
    }

private:
    QList<QMetaObject> m_operations;
};

class TestProcessModuleLoader : public GtProcessModuleLoader
{
public:
    bool validate(GtModuleInterface* module) const
    {
        return check(module);
    }

    void registerModule(GtModuleInterface* module)
    {
        insert(module);
    }
};

class TestApplication : public GtCoreApplication
{
public:
    TestApplication() :
        GtCoreApplication(QCoreApplication::instance(), AppMode::Batch)
    {
        init();
    }

protected:
    bool initFirstRun() override
    {
        return true;
    }
};

class OperationModuleLoaderTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_application = std::make_unique<TestApplication>();
    }

    void TearDown() override
    {
        m_application.reset();
    }

private:
    std::unique_ptr<TestApplication> m_application;
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
    EXPECT_EQ(events.publishedEvents, 1);
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

TEST(GtOperationInterface, defaultDeclarationIsEmpty)
{
    GtOperationInterface operationInterface;

    EXPECT_TRUE(operationInterface.operations().isEmpty());
}

TEST_F(OperationModuleLoaderTest, registersDeclaredOperation)
{
    TestOperationModule module({GT_METADATA(TestOperation)});
    TestProcessModuleLoader loader;

    ASSERT_TRUE(loader.validate(&module));
    loader.registerModule(&module);

    ASSERT_TRUE(gtObjectFactory->knownClass(GT_CLASSNAME(TestOperation)));
    std::unique_ptr<GtObject> object(
        gtObjectFactory->newObject(GT_CLASSNAME(TestOperation)));
    EXPECT_NE(qobject_cast<GtExecutableOperation*>(object.get()), nullptr);
    EXPECT_TRUE(gtObjectFactory->unregisterClass(GT_METADATA(TestOperation)));
}

TEST_F(OperationModuleLoaderTest, rejectsNonOperationDeclaration)
{
    TestOperationModule module({GT_METADATA(TestNonOperation)});
    TestProcessModuleLoader loader;

    EXPECT_FALSE(loader.validate(&module));
}

TEST_F(OperationModuleLoaderTest, rejectsDuplicateOperationDeclaration)
{
    TestOperationModule module(
        {GT_METADATA(TestOperation), GT_METADATA(TestOperation)});
    TestProcessModuleLoader loader;

    EXPECT_FALSE(loader.validate(&module));
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
