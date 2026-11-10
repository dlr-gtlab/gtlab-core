/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_recordingexecutor.h"

#include "gt_abstractproperty.h"
#include "gt_calculator.h"
#include "gt_recordingobserver.h"
#include "gt_recordingsession.h"
#include "gt_task.h"

#include <QFile>
#include <QMutex>
#include <QMutexLocker>
#include <QTextStream>

void
test::recordingLog(const QString& line)
{
    static QMutex mutex;

    const QString path = qEnvironmentVariable("GTLAB_TEST_RECORDING_LOG");
    if (path.isEmpty()) return;

    QMutexLocker locker(&mutex);
    QFile file(path);
    if (!file.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream(&file) << line << '\n';
}

namespace
{

/// Logs every recording callback as one line
class LoggingObserver : public GtRecordingObserver
{
public:
    void recordingStarted(const GtRecordingSession& session) override
    {
        m_component = session.component().objectName();
        const QUuid parent = session.parentId();
        test::recordingLog(QStringLiteral("started %1 id=%2 parent=%3")
                               .arg(m_component, session.id().toString(),
                                    parent.isNull() ? QStringLiteral("none")
                                                    : parent.toString()));
    }

    void propertyRead(const GtObject& owner,
                      const GtAbstractProperty& property) override
    {
        logAccess(QStringLiteral("read"), owner, property);
    }

    void propertyWritten(const GtObject& owner,
                         const GtAbstractProperty& property) override
    {
        logAccess(QStringLiteral("written"), owner, property);
    }

    void recordingFinished(const GtRecordingSession&, bool success) override
    {
        test::recordingLog(QStringLiteral("finished %1 success=%2")
                               .arg(m_component)
                               .arg(success ? 1 : 0));
    }

private:
    QString m_component;

    void logAccess(const QString& kind, const GtObject& owner,
                   const GtAbstractProperty& property)
    {
        test::recordingLog(
            QStringLiteral("%1 %2.%3 in %4")
                .arg(kind, owner.objectName(), property.ident(), m_component));
    }
};

} // namespace

TestRecordingExecutor::TestRecordingExecutor() = default;

bool
TestRecordingExecutor::exec(GtCalculator* calculator)
{
    if (!calculator) return false;

    LoggingObserver observer;
    GtRecordingSession session{*calculator, observer,
                               calculator->linkedObjects()};

    return session.run([&]() { return calculator->run(); });
}

bool
TestRecordingExecutor::exec(GtTask* task)
{
    if (!task) return false;

    LoggingObserver observer;
    GtRecordingSession session{*task, observer, task->linkedObjects()};

    // GtTask::exec() leaves setUp() to the executor
    return session.run([&]() {
        if (!task->setUp())
        {
            task->setState(GtProcessComponent::FAILED);
            return false;
        }
        return task->runIteration();
    });
}
