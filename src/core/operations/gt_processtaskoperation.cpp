/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_processtaskoperation.h"

#include "gt_abstractproperty.h"
#include "gt_coreprocessexecutor.h"
#include "gt_executioncontext.h"
#include "gt_objectfactory.h"
#include "gt_objectmementodiff.h"
#include "gt_project.h"
#include "gt_task.h"
#include "gt_propertyreference.h"
#include "gt_processcomponent.h"
#include "gt_processdata.h"
#include "gt_executioneventstream.h"

#include <QJsonObject>
#include <QMetaObject>
#include <QMetaEnum>
#include <QRegularExpression>

#include <memory>

namespace
{

    GtOperationExecutionResult resultFor(
        GtOperationExecutionResult::Status status, QString code,
        QString message, GtObjectMementoDiff const& diff)
    {
        auto result = std::make_unique<GtProcessTaskOperationResult>();
        result->setDiffXml(QString::fromUtf8(diff.toByteArray()));
        return {status, std::move(code), std::move(message), std::move(result)};
    }

    GtObjectMemento projectDataMemento(GtProject& project)
    {
        GtObjectMemento memento = project.toProjectDataMemento();
        memento.setUuid(project.uuid());
        memento.setIdent(project.objectName());
        return memento;
    }

    GtObjectMementoDiff projectDiff(GtProject& project,
                                    GtObjectMemento const& before)
    {
        GtObjectMemento after = projectDataMemento(project);
        after.setIdent(before.ident());
        return GtObjectMementoDiff(before, after);
    }

    bool canApplyProjectDiff(GtProject& project,
                             GtObjectMementoDiff const& diff)
    {
        GtObjectMemento baseline = projectDataMemento(project);
        std::unique_ptr<GtObject> staged = baseline.toObject(*gtObjectFactory);
        if (!staged)
        {
            return false;
        }

        const QByteArray before = baseline.toByteArray();
        GtObjectMementoDiff stagedDiff(diff.toByteArray());
        if (!staged->applyDiff(stagedDiff))
        {
            return false;
        }

        GtObjectMementoDiff reverseDiff(diff.toByteArray());
        if (!staged->revertDiff(reverseDiff))
        {
            return false;
        }

        // Reversing the staged diff must restore every value it touched. This
        // both detects stale edits to affected data and proves the complete
        // diff can be applied before touching the originating project.
        return staged->toMemento().toByteArray() == before;
    }

    QString stateName(GtProcessComponent::STATE state)
    {
        const auto stateEnum = QMetaEnum::fromType<GtProcessComponent::STATE>();
        const char* const key = stateEnum.valueToKey(state);
        return key ? QString::fromLatin1(key) : QString::number(state);
    }

    struct TaskEventConnections
    {
        QList<QMetaObject::Connection> values;

        ~TaskEventConnections()
        {
            for (const auto& connection : values)
            {
                QObject::disconnect(connection);
            }
        }
    };

    void publishTaskEvents(GtTask& task, GtExecutionEventStream& events,
                           TaskEventConnections& connections)
    {
        const auto components = task.findChildren<GtProcessComponent*>();

        auto publishState = [&events](GtProcessComponent* component,
                                      GtProcessComponent::STATE state) {
            events.publish(
                QStringLiteral("process.state_changed"),
                QJsonObject{
                    {QStringLiteral("componentUuid"), component->uuid()},
                    {QStringLiteral("state"), stateName(state)}});
        };
        auto publishProgress = [&events](GtProcessComponent* component,
                                         int progress) {
            events.publish(QStringLiteral("process.progress_changed"),
                           QJsonObject{{QStringLiteral("componentUuid"),
                                        component->uuid()},
                                       {QStringLiteral("progress"), progress}});
        };

        QList<GtProcessComponent*> tree{&task};
        tree.append(components);
        for (GtProcessComponent* component : tree)
        {
            connections.values.append(QObject::connect(
                component, &GtProcessComponent::stateChanged, component,
                [component, publishState](GtProcessComponent::STATE state) {
                    publishState(component, state);
                }));
            connections.values.append(QObject::connect(
                component, &GtProcessComponent::progressStateChanged, component,
                [component, publishProgress](int progress) {
                    publishProgress(component, progress);
                }));

            for (GtAbstractProperty* property :
                 component->monitoringProperties())
            {
                connections.values.append(QObject::connect(
                    property, &GtAbstractProperty::changed, property,
                    [property, component, &events] {
                        events.publish(
                            QStringLiteral(
                                "process.monitoring_property_changed"),
                            QJsonObject{
                                {QStringLiteral("componentUuid"),
                                 component->uuid()},
                                {QStringLiteral("property"), property->ident()},
                                {QStringLiteral("value"),
                                 QJsonValue::fromVariant(
                                     property->valueToVariant())}});
                    }));
            }

            for (const GtPropertyReference& propertyRef :
                 component->containerMonitoringPropertyRefs())
            {
                GtAbstractProperty* const property =
                    propertyRef.resolve(*component);
                if (!property)
                {
                    continue;
                }

                connections.values.append(QObject::connect(
                    property, &GtAbstractProperty::changed, property,
                    [property, component, propertyRef, &events] {
                        events.publish(
                            QStringLiteral(
                                "process.monitoring_property_changed"),
                            QJsonObject{{QStringLiteral("componentUuid"),
                                         component->uuid()},
                                        {QStringLiteral("property"),
                                         propertyRef.toString()},
                                        {QStringLiteral("value"),
                                         QJsonValue::fromVariant(
                                             property->valueToVariant())}});
                    }));
            }
        }
    }

} // namespace

GtProcessTaskOperationResult::GtProcessTaskOperationResult() :
    m_diffXml(
        QStringLiteral("diffXml"), tr("Project Diff"),
        tr("Serialized project Memento diff"), QString(),
        QRegularExpression(QStringLiteral(".*"),
                           QRegularExpression::DotMatchesEverythingOption))
{
    registerProperty(m_diffXml);
}

QString
GtProcessTaskOperationResult::diffXml() const
{
    return m_diffXml.getVal();
}

void
GtProcessTaskOperationResult::setDiffXml(QString xml)
{
    m_diffXml.setVal(std::move(xml));
}

ProcessTaskOperation::ProcessTaskOperation() :
    m_taskUuid(QStringLiteral("taskUuid"), tr("Task UUID"),
               tr("UUID of the originating project task"))
{
    registerProperty(m_taskUuid);
}

bool
ProcessTaskOperation::requiresProject() const
{
    return true;
}

QString
ProcessTaskOperation::taskUuid() const
{
    return m_taskUuid.getVal();
}

void
ProcessTaskOperation::setTaskUuid(QString uuid)
{
    m_taskUuid.setVal(std::move(uuid));
}

std::unique_ptr<GtObject>
ProcessTaskOperation::createData(GtExecutionContext const& context) const
{
    GtProject* const project = context.project();
    if (!project || taskUuid().isEmpty())
    {
        return {};
    }

    auto* const task =
        qobject_cast<GtTask*>(project->getObjectByUuid(taskUuid()));
    if (!task)
    {
        return {};
    }

    return std::unique_ptr<GtObject>(task->clone());
}

GtOperationExecutionResult
ProcessTaskOperation::execute(GtOperationExecutionContext& context)
{
    auto* const execution = GtExecutionContext::current();
    GtProject* const project = execution ? execution->project() : nullptr;
    auto* const task = qobject_cast<GtTask*>(context.data());
    if (!project || !task)
    {
        const GtObjectMementoDiff emptyDiff;
        return resultFor(
            GtOperationExecutionResult::Status::Failed,
            QStringLiteral("invalid_task_execution"),
            tr("A project and detached GtTask input are required."), emptyDiff);
    }

    GtObjectMemento before = projectDataMemento(*project);
    GtProcessData processData;
    if (!processData.appendChild(task))
    {
        const GtObjectMementoDiff emptyDiff;
        return resultFor(
            GtOperationExecutionResult::Status::Failed,
            QStringLiteral("invalid_task_tree"),
            tr("The detached task could not be attached for execution."),
            emptyDiff);
    }
    struct DetachTask
    {
        GtTask* task;
        ~DetachTask()
        {
            task->disconnectFromParent();
        }
    } detachTask{task};

    TaskEventConnections eventConnections;
    publishTaskEvents(*task, context.events(), eventConnections);
    GtCoreProcessExecutor executor;
    executor.setSource(project);
    auto cancellationSubscription =
        context.cancellation().subscribe([&executor, task] {
            QMetaObject::invokeMethod(
                &executor,
                [&executor, task] {
                    if (executor.currentRunningTask() == task)
                    {
                        executor.terminateTask(task);
                    }
                },
                Qt::QueuedConnection);
        });

    GtCoreProcessExecutor::TaskExecState executionState =
        GtCoreProcessExecutor::TaskExecState::Invalid;
    executionState = executor.startTask(task);

    const GtObjectMementoDiff diff = projectDiff(*project, before);
    if (executionState != GtCoreProcessExecutor::TaskExecState::Started)
    {
        return resultFor(GtOperationExecutionResult::Status::Failed,
                         QStringLiteral("task_execution_invalid"),
                         tr("The task executor could not start the task."),
                         diff);
    }

    switch (task->currentState())
    {
    case GtProcessComponent::FINISHED:
    case GtProcessComponent::WARN_FINISHED:
        return resultFor(GtOperationExecutionResult::Status::Success, {}, {},
                         diff);
    case GtProcessComponent::TERMINATED: {
        const GtObjectMementoDiff emptyDiff;
        return resultFor(GtOperationExecutionResult::Status::Cancelled,
                         QStringLiteral("task_cancelled"),
                         tr("The task was terminated."), emptyDiff);
    }
    case GtProcessComponent::FAILED:
    default:
        return resultFor(GtOperationExecutionResult::Status::Failed,
                         QStringLiteral("task_failed"),
                         tr("The task did not finish successfully."), diff);
    }
}

GtOperationApplyStatus
ProcessTaskOperation::applyResult(
    GtOperationExecutionResult const& executionResult,
    GtExecutionContext& context) const
{
    if (executionResult.status == GtOperationExecutionResult::Status::Failed)
    {
        return GtOperationApplyStatus::success();
    }

    if (!executionResult.result)
    {
        if (executionResult.status ==
            GtOperationExecutionResult::Status::Cancelled)
        {
            return GtOperationApplyStatus::success();
        }

        return GtOperationApplyStatus::failure(
            tr("The successful operation result is missing."));
    }

    auto const* const result =
        qobject_cast<GtProcessTaskOperationResult const*>(
            executionResult.result.get());
    if (!result)
    {
        return GtOperationApplyStatus::failure(
            tr("The operation result has an unexpected type."));
    }

    const QByteArray xml = result->diffXml().toUtf8();
    if (xml.trimmed().isEmpty())
    {
        return GtOperationApplyStatus::success();
    }

    GtObjectMementoDiff diff(xml);
    if (diff.isNull() || diff.numberOfDiffSteps() == 0)
    {
        return GtOperationApplyStatus::failure(
            tr("The operation result contains an invalid project diff."));
    }

    GtProject* const project = context.project();
    if (!project || !canApplyProjectDiff(*project, diff))
    {
        return GtOperationApplyStatus::failure(
            tr("The project diff is invalid or conflicts with current project "
               "data."));
    }

    const GtObjectMemento beforeApply = projectDataMemento(*project);
    GtObjectMementoDiff applyDiff(diff.toByteArray());
    if (!project->applyDiff(applyDiff))
    {
        // applyDiff processes entries sequentially. Restore the snapshot if an
        // unexpected failure occurs after one or more entries were applied.
        GtObjectMemento current = projectDataMemento(*project);
        GtObjectMementoDiff rollback(current, beforeApply);
        project->applyDiff(rollback);
        return GtOperationApplyStatus::failure(
            tr("The project diff could not be applied."));
    }

    return GtOperationApplyStatus::success();
}
