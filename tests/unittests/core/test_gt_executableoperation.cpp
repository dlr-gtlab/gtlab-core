/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <functional>
#include <future>
#include <memory>
#include <sstream>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QThread>
#include <thread>

#include "gt_executioneventfilewriter.h"
#include "gt_executioneventstream.h"
#include "gt_executableoperation.h"
#include "gt_stdioexecutioneventencoder.h"
#include "gt_coreapplication.h"
#include "gt_moduleinterface.h"
#include "gt_operationinterface.h"
#include "gt_processmoduleloader.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectgroup.h"

namespace
{

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


QJsonObject protocolRecord(QByteArray const& line)
{
    constexpr auto prefix = "@gtlab-operation-v1 ";
    EXPECT_TRUE(line.startsWith(prefix));
    EXPECT_TRUE(line.endsWith('\n'));

    QJsonParseError error;
    auto const document = QJsonDocument::fromJson(
        line.mid(static_cast<int>(std::char_traits<char>::length(prefix))).trimmed(),
        &error);
    EXPECT_EQ(error.error, QJsonParseError::NoError);
    return document.object();
}

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
    GtExecutionId streamExecutionId;
    GtExecutionEventStream events(streamExecutionId);
    GtCancellationToken cancellation;
    std::thread cancellationThread([&]() { cancellation.requestCancellation(); });
    cancellationThread.join();
    GtOperationExecutionContext context(data.get(), events, cancellation);

    const QString executionIdText = context.executionId().toString();
    auto result = testOperation->execute(context);

    EXPECT_TRUE(testOperation->observedData);
    EXPECT_TRUE(testOperation->observedCancellation);
    EXPECT_EQ(testOperation->observedExecutionId, executionIdText);
    EXPECT_EQ(context.executionId().toString(), events.executionId().toString());
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


TEST(GtExecutionEventStream, assignsSequenceAndNotifiesLocalObservers)
{
    GtExecutionId executionId;
    GtExecutionEventStream stream(executionId);
    QVector<GtExecutionEvent> observed;
    QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                     [&observed](GtExecutionEvent event) {
                         observed.push_back(std::move(event));
                     });

    stream.publish(QStringLiteral("started"));
    stream.publish(QStringLiteral("progress"), QJsonValue(0.5));
    stream.publish(QStringLiteral("finished"), QJsonObject{{"count", 3}});

    ASSERT_EQ(observed.size(), 3);
    for (int index = 0; index < observed.size(); ++index) {
        EXPECT_EQ(observed.at(index).sequence(), static_cast<quint64>(index));
        EXPECT_EQ(observed.at(index).executionId().toString(), executionId.toString());
    }
    EXPECT_EQ(observed.at(1).payload(), QJsonValue(0.5));
}

TEST(GtExecutionEventStream, preservesObserverOrderDuringReentrantPublication)
{
    GtExecutionEventStream stream(GtExecutionId{});
    QVector<quint64> firstObserver;
    QVector<quint64> secondObserver;
    QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                     [&stream, &firstObserver](GtExecutionEvent event) {
                         firstObserver.push_back(event.sequence());
                         if (event.sequence() == 0) {
                             stream.publish(QStringLiteral("reentrant"));
                         }
                     });
    QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                     [&secondObserver](GtExecutionEvent event) {
                         secondObserver.push_back(event.sequence());
                     });

    stream.publish(QStringLiteral("initial"));

    EXPECT_EQ(firstObserver, QVector<quint64>({0, 1}));
    EXPECT_EQ(secondObserver, QVector<quint64>({0, 1}));
}

TEST(GtExecutionEventStream, deliversQueuedEventAcrossThreads)
{
    GtExecutionId executionId;
    GtExecutionEventStream stream(executionId);
    QThread observerThread;
    QObject observer;
    observer.moveToThread(&observerThread);
    std::promise<GtExecutionEvent> delivered;
    auto future = delivered.get_future();
    QObject::connect(&stream, &GtExecutionEventStream::eventPublished, &observer,
                     [&delivered](GtExecutionEvent event) {
                         delivered.set_value(std::move(event));
                     }, Qt::QueuedConnection);
    observerThread.start();

    stream.publish(QStringLiteral("cross-thread"), QJsonValue(42));
    ASSERT_EQ(future.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    const GtExecutionEvent event = future.get();
    EXPECT_EQ(event.executionId().toString(), executionId.toString());
    EXPECT_EQ(event.sequence(), 0U);
    EXPECT_EQ(event.payload(), QJsonValue(42));

    QMetaObject::invokeMethod(&observer, [&observer] {
        observer.moveToThread(QCoreApplication::instance()->thread());
    }, Qt::BlockingQueuedConnection);
    observerThread.quit();
    observerThread.wait();
}

TEST(GtExecutionEventFileWriter, writesOneCompactJsonLinePerEvent)
{
    QTemporaryDir directory;
    ASSERT_TRUE(directory.isValid());
    const QString eventPath = directory.filePath(QStringLiteral("events.ndjson"));
    const QString logPath = directory.filePath(QStringLiteral("worker.log"));
    GtExecutionEventStream stream(GtExecutionId{});
    GtExecutionEventFileWriter writer(eventPath);
    ASSERT_TRUE(writer.isOpen());
    QObject::connect(&stream, &GtExecutionEventStream::eventPublished,
                     &writer, &GtExecutionEventFileWriter::writeEvent);

    stream.publish(QStringLiteral("started"));
    stream.publish(QStringLiteral("progress"), QJsonObject{{"ratio", 0.5}});
    QFile log(logPath);
    ASSERT_TRUE(log.open(QIODevice::WriteOnly));
    log.write("ordinary worker output\n");
    log.close();

    QFile events(eventPath);
    ASSERT_TRUE(events.open(QIODevice::ReadOnly));
    const QList<QByteArray> lines = events.readAll().split('\n');
    ASSERT_EQ(lines.size(), 3);
    for (int index = 0; index < 2; ++index) {
        QJsonParseError error;
        const QJsonObject record = QJsonDocument::fromJson(lines.at(index), &error).object();
        EXPECT_EQ(error.error, QJsonParseError::NoError);
        EXPECT_EQ(record.value("sequence"), index);
        EXPECT_FALSE(lines.at(index).contains("ordinary worker output"));
    }
}

TEST(GtStdioExecutionEventEncoder, encodesJsonEventAsV1Record)
{
    std::ostringstream output;
    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    encoder.encodeEvent(GtExecutionEvent(executionId, 7, QStringLiteral("progress"),
                                         QJsonObject{{"ratio", 0.5}}));

    const QByteArray line = QByteArray::fromStdString(output.str());
    const QByteArray expected =
        "@gtlab-operation-v1 {\"eventType\":\"progress\",\"executionId\":\"" +
        executionId.toString().toUtf8() +
        "\",\"kind\":\"event\",\"payload\":{\"ratio\":0.5},"
        "\"payloadEncoding\":\"json\",\"sequence\":7,\"version\":1}\n";
    EXPECT_EQ(line, expected);
}

TEST(GtStdioExecutionEventEncoder, serializesConcurrentProtocolRecords)
{
    std::ostringstream output;
    GtExecutionId firstId;
    GtExecutionId secondId;
    GtStdioExecutionEventEncoder first(firstId, output);
    GtStdioExecutionEventEncoder second(secondId, output);
    auto publish = [](GtStdioExecutionEventEncoder& encoder, GtExecutionId const& id) {
        for (quint64 sequence = 0; sequence < 20; ++sequence) {
            encoder.encodeEvent(GtExecutionEvent(id, sequence,
                QStringLiteral("progress"), QJsonValue(static_cast<int>(sequence))));
        }
    };
    std::thread firstThread(publish, std::ref(first), std::cref(firstId));
    std::thread secondThread(publish, std::ref(second), std::cref(secondId));
    firstThread.join();
    secondThread.join();

    const QList<QByteArray> lines = QByteArray::fromStdString(output.str()).split('\n');
    ASSERT_EQ(lines.size(), 41);
    for (int index = 0; index < 40; ++index) {
        EXPECT_TRUE(lines.at(index).startsWith("@gtlab-operation-v1 {"));
        QJsonParseError error;
        QJsonDocument::fromJson(lines.at(index).mid(20), &error);
        EXPECT_EQ(error.error, QJsonParseError::NoError);
    }
}

#include "test_gt_executableoperation.moc"
