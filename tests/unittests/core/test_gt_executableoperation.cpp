/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <functional>
#include <memory>
#include <sstream>

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <thread>

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
        EXPECT_EQ(observed.at(index).executionId().toString(),
                  executionId.toString());
    }
    EXPECT_EQ(observed.at(1).jsonPayload(), QJsonValue(0.5));
    EXPECT_EQ(observed.at(2).jsonPayload().toObject().value("count"), 3);
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

    const QJsonObject record = protocolRecord(line);
    EXPECT_EQ(record.value("version"), 1);
    EXPECT_EQ(record.value("kind"), "event");
    EXPECT_EQ(record.value("executionId"), executionId.toString());
    EXPECT_EQ(record.value("sequence"), 7);
    EXPECT_EQ(record.value("eventType"), "progress");
    EXPECT_EQ(record.value("payloadEncoding"), "json");
    EXPECT_EQ(record.value("payload").toObject().value("ratio"), 0.5);
}

TEST(GtStdioExecutionEventEncoder, encodesAbsentPayloadAsJsonNull)
{
    std::ostringstream output;

    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    encoder.encodeEvent(GtExecutionEvent(executionId, 0,
                                         QStringLiteral("started")));

    const QJsonObject record = protocolRecord(QByteArray::fromStdString(output.str()));
    EXPECT_TRUE(record.contains("payload"));
    EXPECT_EQ(record.value("payloadEncoding"), "json");
    EXPECT_TRUE(record.value("payload").isNull());
}

TEST(GtStdioExecutionEventEncoder, encodesMementoPayloadWithoutRawNewlines)
{
    std::ostringstream output;

    GtObject object;
    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    encoder.encodeEvent(GtExecutionEvent(executionId, 0, QStringLiteral("snapshot"),
                                         object.toMemento().toByteArray()));

    ASSERT_EQ(QByteArray::fromStdString(output.str()).count('\n'), 1);
    const QJsonObject record = protocolRecord(QByteArray::fromStdString(output.str()));
    EXPECT_EQ(record.value("payloadEncoding"), "memento-xml-base64");
    const QByteArray xml = QByteArray::fromBase64(
        record.value("payload").toString().toLatin1());
    EXPECT_EQ(xml, object.toMemento().toByteArray());
}

TEST(GtStdioExecutionEventEncoder, encodesNullResultAndRejectsLaterRecords)
{
    std::ostringstream output;

    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    EXPECT_TRUE(encoder.encodeResult(nullptr));
    EXPECT_TRUE(encoder.isTerminal());
    encoder.encodeEvent(GtExecutionEvent(executionId, 1, QStringLiteral("late")));
    EXPECT_FALSE(encoder.encodeFailure(QStringLiteral("late"), QStringLiteral("late")));

    const QByteArray line = QByteArray::fromStdString(output.str());
    const QByteArray expected =
        "@gtlab-operation-v1 {\"executionId\":\"" +
        executionId.toString().toUtf8() +
        "\",\"kind\":\"result\",\"result\":null,"
        "\"resultEncoding\":\"null\",\"version\":1}\n";
    EXPECT_EQ(line, expected);

    const QJsonObject record = protocolRecord(line);
    EXPECT_EQ(record.value("kind"), "result");
    EXPECT_EQ(record.value("resultEncoding"), "null");
    EXPECT_TRUE(record.value("result").isNull());
}

TEST(GtStdioExecutionEventEncoder, encodesMementoResult)
{
    std::ostringstream output;

    GtObject result;
    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    ASSERT_TRUE(encoder.encodeResult(&result));

    const QJsonObject record = protocolRecord(QByteArray::fromStdString(output.str()));
    EXPECT_EQ(record.value("kind"), "result");
    EXPECT_EQ(record.value("resultEncoding"), "memento-xml-base64");
    EXPECT_EQ(QByteArray::fromBase64(record.value("result").toString().toLatin1()),
              result.toMemento().toByteArray());
}

TEST(GtStdioExecutionEventEncoder, serializesConcurrentWholeRecordWrites)
{
    std::ostringstream output;
    GtExecutionId firstExecutionId;
    GtExecutionId secondExecutionId;
    GtStdioExecutionEventEncoder first(firstExecutionId, output);
    GtStdioExecutionEventEncoder second(secondExecutionId, output);

    auto publish = [](GtStdioExecutionEventEncoder& encoder,
                      GtExecutionId const& executionId) {
        for (quint64 sequence = 0; sequence < 20; ++sequence) {
            encoder.encodeEvent(GtExecutionEvent(
                executionId, sequence, QStringLiteral("progress"),
                QJsonObject{{"sequence", static_cast<qint64>(sequence)}}));
        }
    };

    std::thread firstThread(publish, std::ref(first), std::cref(firstExecutionId));
    std::thread secondThread(publish, std::ref(second), std::cref(secondExecutionId));
    firstThread.join();
    secondThread.join();

    const QList<QByteArray> lines =
        QByteArray::fromStdString(output.str()).split('\n');
    ASSERT_EQ(lines.size(), 41);
    for (int index = 0; index < 40; ++index) {
        const QJsonObject record = protocolRecord(lines.at(index) + '\n');
        EXPECT_EQ(record.value("kind"), "event");
        EXPECT_EQ(record.value("eventType"), "progress");
    }
}

TEST(GtStdioExecutionEventEncoder, encodesStructuredFailure)
{
    std::ostringstream output;

    GtExecutionId executionId;
    GtStdioExecutionEventEncoder encoder(executionId, output);
    EXPECT_TRUE(encoder.encodeFailure(QStringLiteral("operation.failed"),
                                      QStringLiteral("Calculation failed"),
                                      QJsonObject{{"iteration", 4}}));

    const QByteArray line = QByteArray::fromStdString(output.str());
    const QByteArray expected =
        "@gtlab-operation-v1 {\"details\":{\"iteration\":4},"
        "\"errorCode\":\"operation.failed\",\"executionId\":\"" +
        executionId.toString().toUtf8() +
        "\",\"kind\":\"failure\",\"message\":\"Calculation failed\","
        "\"version\":1}\n";
    EXPECT_EQ(line, expected);

    const QJsonObject record = protocolRecord(line);
    EXPECT_EQ(record.value("kind"), "failure");
    EXPECT_EQ(record.value("errorCode"), "operation.failed");
    EXPECT_EQ(record.value("message"), "Calculation failed");
    EXPECT_EQ(record.value("details").toObject().value("iteration"), 4);
}

#include "test_gt_executableoperation.moc"
