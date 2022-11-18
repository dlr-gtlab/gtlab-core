/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 29.8.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunnerconnectionstrategy.h"

#include "gt_eventloop.h"
#include "gt_processrunnercommand.h"
#include "gt_processrunnerresponse.h"
#include "gt_processrunnernotification.h"

#include <QMutexLocker>
#include <QTimer>

GtProcessRunnerConnectionStrategy::GtProcessRunnerConnectionStrategy(
        QObject* parent) :
    QObject(parent)
{

}

void
GtProcessRunnerConnectionStrategy::connectToTarget(GtEventLoop& loop)
{
    if (!m_targetAddress.isValid())
    {
        emit loop.failed();
        return;
    }
    connectTo(loop, m_targetAddress.address, m_targetAddress.port);
}

const gt::process_runner::ConnectionAddress&
GtProcessRunnerConnectionStrategy::targetAddress() const
{
    return m_targetAddress;
}

void
GtProcessRunnerConnectionStrategy::setTargetAddress(
        gt::process_runner::ConnectionAddress address)
{
    m_targetAddress = std::move(address);
}

bool
GtProcessRunnerConnectionStrategy::hasResponse() const
{
    QMutexLocker locker{&m_mutex};
    return !m_responses.empty();
}

bool
GtProcessRunnerConnectionStrategy::hasCommand() const
{
    QMutexLocker locker{&m_mutex};
    return !m_commands.empty();
}

bool
GtProcessRunnerConnectionStrategy::hasNotification() const
{
    QMutexLocker locker{&m_mutex};
    return !m_notifications.empty();
}

std::unique_ptr<GtProcessRunnerResponse>
GtProcessRunnerConnectionStrategy::nextResponse()
{
    QMutexLocker locker{&m_mutex};
    return std::unique_ptr<GtProcessRunnerResponse>{
        m_responses.empty() ? nullptr :  m_responses.dequeue()
    };
}

std::unique_ptr<GtProcessRunnerCommand>
GtProcessRunnerConnectionStrategy::nextCommand()
{
    QMutexLocker locker{&m_mutex};
    return std::unique_ptr<GtProcessRunnerCommand>{
        m_commands.empty() ? nullptr :  m_commands.dequeue()
    };
}

std::unique_ptr<GtProcessRunnerNotification>
GtProcessRunnerConnectionStrategy::nextNotification()
{
    QMutexLocker locker{&m_mutex};
    return std::unique_ptr<GtProcessRunnerNotification>{
        m_notifications.empty() ? nullptr :  m_notifications.dequeue()
    };
}

void
GtProcessRunnerConnectionStrategy::queueData(std::unique_ptr<GtObject> data)
{
    if (auto* obj = data.release())
    {
        // we own this object now
        obj->setParent(this);

        if (auto* robj = qobject_cast<GtProcessRunnerResponse*>(obj))
        {
            queueResponse(robj);
        }
        else if (auto* cobj = qobject_cast<GtProcessRunnerCommand*>(obj))
        {
            queueCommand(cobj);
        }
        else if (auto* nobj = qobject_cast<GtProcessRunnerNotification*>(obj))
        {
            queueNotification(nobj);
        }
        else
        {
            gtWarningId(GT_EXEC_ID) << tr("Unkown data object!");
        }
    }
    else
    {
        gtWarningId(GT_EXEC_ID) << tr("Null object!");
    }
}

void
GtProcessRunnerConnectionStrategy::queueResponse(GtProcessRunnerResponse* o)
{
    {
        QMutexLocker locker{&m_mutex};
        m_responses.enqueue(o);
    }
    emit recievedResponse();
}

void
GtProcessRunnerConnectionStrategy::queueCommand(GtProcessRunnerCommand* o)
{
    {
        QMutexLocker locker{&m_mutex};
        m_commands.enqueue(o);
    }
    emit recievedCommand();
}

void
GtProcessRunnerConnectionStrategy::queueNotification(GtProcessRunnerNotification* o)
{
    {
        QMutexLocker locker{&m_mutex};
        m_notifications.enqueue(o);
    }
    emit recievedNotification();
}
