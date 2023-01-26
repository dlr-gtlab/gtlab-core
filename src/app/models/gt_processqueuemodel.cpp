/* GTlab - Gas Turbine laboratory
 * Source File: gt_processqueuemodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 11.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_processqueuemodel.h"

#include <QIcon>

#include "gt_logging.h"
#include "gt_processexecutor.h"
#include "gt_task.h"
#include "gt_icons.h"


GtProcessQueueModel::GtProcessQueueModel(GtCoreProcessExecutor* exec,
                                         QObject* parent) :
    QAbstractItemModel(parent)
{
    setProcessExecutor(exec);

    connect(&gt::processExecutorManager(),
            &GtProcessExecutorManager::executorChanged,
            this, &GtProcessQueueModel::setProcessExecutor,
            Qt::UniqueConnection);
}


int
GtProcessQueueModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 1;
}

int
GtProcessQueueModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    if (!m_exec)
    {
        return 0;
    }
    return m_tasks.size();
}

QVariant
GtProcessQueueModel::data(const QModelIndex &index, int role) const
{
    QVariant retVal;

    if (!index.isValid())
    {
        gtDebug() << "Invalid Model Index!";
        return QVariant();
    }

    int row = index.row();
    int column = index.column();

    if (row >= m_tasks.size())
    {
        gtDebug() << "Invalid row index";
        return QVariant();
    }

    if (column != 0)
    {
        gtDebug() << "Invalid column index";
        return QVariant();
    }

    switch(role)
    {
        case Qt::DisplayRole:
        {
            if (m_tasks.empty() && row == 0)
            {
                return "No tasks running or queued!";
            }

            return m_tasks.at(row)->objectName();
        }
        case Qt::DecorationRole:
        {
            GtTask* task = m_tasks.at(row);

            if (!task)
            {
                return {};
            }

            if (task->currentState() == GtTask::RUNNING)
            {
                return gt::gui::icon::inProgress16();
            }
            else if (task->currentState() == GtTask::QUEUED)
            {
                return gt::gui::icon::sleep16();
            }
            else if (task->currentState() == GtTask::FINISHED)
            {
                return gt::gui::icon::check16();
            }
            else if (task->currentState() == GtTask::FAILED)
            {
                return gt::gui::icon::processFailed16();
            }
            else if (task->currentState() == GtTask::CONNECTING)
            {
                return gt::gui::icon::network16();
            }
            break;
        }

        default:
            break;
    }

    return retVal;
}

QModelIndex
GtProcessQueueModel::index(int row, int column,
                           const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    auto exec = m_exec;

    if (!exec)
    {
        gtDebug().medium() << "No process executor defined";
        return {};
    }
    else if (exec->queue().empty())
    {
        gtDebug().medium() << "No tasks in queue";
        return {};
    }
    else if (column != 0)
    {
        gtDebug().medium() << "Invalid column index";
        return {};
    }
    else if (row >= exec->queue().size())
    {
        gtDebug().medium() << "Invalid row index";
        return {};
    }

    return createIndex(row, column);
}

QModelIndex
GtProcessQueueModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return {};
}

bool
GtProcessQueueModel::setData(const QModelIndex &index,
                             const QVariant &value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return true;
}

void
GtProcessQueueModel::updateTaskList()
{
    if (m_exec)
    {
        m_tasks.clear();

        if (!m_exec->queue().isEmpty())
        {
            foreach (GtTask* task, m_exec->queue())
            {
                if (task)
                {
                    m_tasks.append(task);
                }
            }

            connect(m_tasks.first(),
                    SIGNAL(stateChanged(GtProcessComponent::STATE)),
                    this, SLOT(onStateChanged()));
        }
    }
}

void
GtProcessQueueModel::setProcessExecutor(GtCoreProcessExecutor* exec)
{
    beginResetModel();
    if (m_exec)
    {
        disconnect(m_exec.data());
    }
    m_exec = exec;
    if (exec)
    {
        connect(exec, &GtCoreProcessExecutor::queueChanged,
                this, &GtProcessQueueModel::onQueueChanged);
    }
    updateTaskList();
    endResetModel();
}

void
GtProcessQueueModel::onQueueChanged()
{
    beginResetModel();
    updateTaskList();
    endResetModel();

    emit queueChanged();
}

void
GtProcessQueueModel::onStateChanged()
{
    emit dataChanged(index(0, 0), index(0, 0));
}
