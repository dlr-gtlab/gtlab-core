/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 29.06.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_processrunnercommand.h"

#include "gt_objectgroup.h"
#include "gt_processrunnerresponse.h"
#include "gt_project.h"
#include "gt_task.h"
#include "gt_footprint.h"

#include "gt_package.h"
#include "gt_coredatamodel.h"


GtProcessRunnerCommand::GtProcessRunnerCommand(bool isFinalized) :
    m_pSessionId("sessionId", "Session ID", "Session ID"),
    m_isFinalized{isFinalized}
{
    setObjectName("ProcessCommand");

    registerProperty(m_pSessionId);
}

void
GtProcessRunnerCommand::setSessionId(QString sessionId)
{
    m_pSessionId = std::move(sessionId);
}

bool
GtProcessRunnerCommand::isFinalized() const
{
    return !m_pSessionId.getVal().isEmpty() && m_isFinalized;
}

QString
GtProcessRunnerCommand::sessionId() const
{
    return m_pSessionId;
}

void
GtProcessRunnerCommand::setFinalized(bool value)
{
    m_isFinalized = value;
}


GtProcessRunnerInitSessionCommand::GtProcessRunnerInitSessionCommand() :
    GtProcessRunnerCommand{true},
    m_pFootPrint{"footPrint", "Footprint", "Footprint",
                 GtFootprint().exportToXML()}
{
    setObjectName("CreateSessionCommand");

    registerProperty(m_pFootPrint);
}

bool
GtProcessRunnerInitSessionCommand::checkFootprint()
{
    QString otherFp = GtFootprint{}.exportToXML();

    // TODO: Diff footprints properly
    if (m_pFootPrint != otherFp)
    {
        gtWarningId(GT_EXEC_ID) << tr("Footprints do not match!")
                           << tr("proceeding...");
        return true;
    }

    gtInfoId(GT_EXEC_ID) << tr("Footprint matches!");
    return true;
}


GtProcessRunnerRunTaskCommand::GtProcessRunnerRunTaskCommand(
        GtTask* task, QString projectPath) :
    m_pTask("task", "Task", "Task to run", {},
            this, {GT_CLASSNAME(GtTask)}),
    m_pSource("source", "Source", "Root of data objects", {},
              this, {GT_CLASSNAME(GtObjectGroup)}),
    m_pProjectPath("projectPath", tr("Project Path"), tr("Project Path"),
                  std::move(projectPath))
{
    setObjectName("RunTaskCommand");

    registerProperty(m_pTask);
    registerProperty(m_pSource);
    registerProperty(m_pProjectPath);

    if (task)
    {
        setTask(*task);
    }
}

void
GtProcessRunnerRunTaskCommand::setTask(GtTask& task)
{
    auto proj = task.findParent<GtProject*>();
    if (!proj)
    {
        gtWarning() << "WFE:" << tr("Null project!");
        return;
    }

    auto* source = new GtObjectGroup;
    source->setObjectName(proj->objectName());
    source->setParent(this);

    auto packages = proj->findDirectChildren<GtPackage*>();

    // TODO: Create command without cloning?
    for (auto* data : qAsConst(packages))
    {
        if (auto* clone =  data->clone())
        {
            clone->setParent(source);
        }
    }

    auto* clone = task.clone();
    if (!clone)
    {
        gtWarningId(GT_EXEC_ID) << tr("Null task!");
        return;
    }

    clone->setParent(this);

    m_pTask.setVal(clone->uuid());
    m_pSource.setVal(source->uuid());
    setFinalized(true);
}

GtTask*
GtProcessRunnerRunTaskCommand::task()
{
    return qobject_cast<GtTask*>(m_pTask.linkedObject(this));
}

GtObject*
GtProcessRunnerRunTaskCommand::source()
{
    return m_pSource.linkedObject(this);
}

QString
GtProcessRunnerRunTaskCommand::projectPath() const
{
    return m_pProjectPath;
}


GtProcessRunnerCollectTaskCommand::GtProcessRunnerCollectTaskCommand(
        const QString& uuid) :
    GtProcessRunnerCommand{true},
    m_pTaskUuid("task_uuid", "Task Uuid", "Task Uuid to collect", uuid)
{
    setObjectName("CollectTaskCommand");

    registerProperty(m_pTaskUuid);
}

GtProcessRunnerPingCommand::GtProcessRunnerPingCommand() :
    GtProcessRunnerCommand{true}
{
    setObjectName("PingCommand");
}

GtProcessRunnerAbortCommand::GtProcessRunnerAbortCommand()
{
    setObjectName("AbortCommand");
}
