/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 *  Created on: 29.06.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERCOMMAND_H
#define GTPROCESSRUNNERCOMMAND_H

#include "gt_processrunnerglobals.h"
#include "gt_objectlinkproperty.h"

#include <memory>

class GtFootprint;
class GtTask;
class GtProcessRunnerResponse;
class GT_CORE_EXPORT GtProcessRunnerCommand : public GtObject
{
    Q_OBJECT

public:

    void setSessionId(QString sessionId);

    bool isFinalized() const;

    QString /*const&*/ sessionId() const;

protected:

    explicit GtProcessRunnerCommand(bool isFinalized = false);

    void setFinalized(bool value);

private:

    GtStringProperty m_pSessionId;
    bool m_isFinalized{false};
};


// create/restore session
class GT_CORE_EXPORT GtProcessRunnerInitSessionCommand :
        public GtProcessRunnerCommand
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerInitSessionCommand();

    bool checkFootprint();

private:

    GtStringProperty m_pFootPrint;
};


// run task
class GT_CORE_EXPORT GtProcessRunnerRunTaskCommand :
        public GtProcessRunnerCommand
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerRunTaskCommand(GtTask* task = {},
                                              QString projectPath = {});

    GtTask* task();

    GtObject* source();

    QString projectPath() const;

private:

    GtObjectLinkProperty m_pTask;
    GtObjectLinkProperty m_pSource;
    GtStringProperty m_pProjectPath;

    void setTask(GtTask& task);
};

// collect task
class GT_CORE_EXPORT GtProcessRunnerCollectTaskCommand :
        public GtProcessRunnerCommand
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerCollectTaskCommand(QString const& uuid = {});

    QString /*const&*/ taskUuid() const { return m_pTaskUuid; }

private:

    GtStringProperty m_pTaskUuid;
};

// ping -> recieve response with current state
class GT_CORE_EXPORT GtProcessRunnerAbortCommand : public GtProcessRunnerCommand
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerAbortCommand();
};

// ping -> recieve response with current state
class GT_CORE_EXPORT GtProcessRunnerPingCommand : public GtProcessRunnerCommand
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerPingCommand();
};

#endif // GTPROCESSRUNNERCOMMAND_H
