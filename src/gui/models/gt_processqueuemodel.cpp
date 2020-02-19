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
#include "gt_application.h"


GtProcessQueueModel::GtProcessQueueModel(GtProcessExecutor* proExec,
                                         QObject* parent) :
    m_proExec(proExec), QAbstractItemModel(parent)
{
    updateTaskList();

    if (m_proExec != Q_NULLPTR)
    {
        connect(m_proExec, &GtProcessExecutor::queueChanged,
                this, &GtProcessQueueModel::onQueueChanged);

        if (!m_proExec->queue().isEmpty())
        {
            connect(m_proExec->queue().first(),
                    SIGNAL(stateChanged(GtProcessComponent::STATE)),
                    this, SLOT(onStateChanged()));
        }
    }
}


int
GtProcessQueueModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

int
GtProcessQueueModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);

    if (m_proExec == Q_NULLPTR)
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
            if (m_tasks.size() == 0 && row == 0)
            {
                return "No tasks running or queued!";
            }

            return m_tasks.at(row)->objectName();
        }
        case Qt::DecorationRole:
        {
            GtTask* task = m_tasks.at(row);

            if (task == Q_NULLPTR)
            {
                return QVariant();
            }

            if (task->currentState() == GtTask::RUNNING)
            {
                return gtApp->icon(
                           QStringLiteral("inProgressIcon_16.png"));
            }
            else if (task->currentState() == GtTask::QUEUED)
            {
                return gtApp->icon(
                           QStringLiteral("sleepIcon_16.png"));
            }
            else if (task->currentState() == GtTask::FINISHED)
            {
                return gtApp->icon(
                           QStringLiteral("checkIcon_16.png"));
            }
            else if (task->currentState() == GtTask::FAILED)
            {
                return gtApp->icon(
                           QStringLiteral("processFailedIcon_16.png"));
            }
            else if (task->currentState() == GtTask::CONNECTING)
            {
                return gtApp->icon(
                           QStringLiteral("networkIcon_16.png"));
            }
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
    Q_UNUSED(parent);

    if (m_proExec == Q_NULLPTR)
    {
        gtDebug() << "No process executor defined";
        return QModelIndex();
    }
    else if (m_proExec->queue().size() == 0)
    {
        gtDebug() << "No tasks in queue";
        return QModelIndex();
    }
    else if (column != 0)
    {
        gtDebug() << "Invalid column index";
        return QModelIndex();
    }
    else if (row >= m_proExec->queue().size())
    {
        gtDebug() << "Invalid row index";
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex
GtProcessQueueModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

bool
GtProcessQueueModel::setData(const QModelIndex &index,
                             const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    Q_UNUSED(role);
    return true;
}

void
GtProcessQueueModel::setProcessExecutor(GtProcessExecutor* procExec)
{
    beginResetModel();
    m_proExec = procExec;
    endResetModel();
}

void
GtProcessQueueModel::updateTaskList()
{
    if (m_proExec != Q_NULLPTR)
    {
        m_tasks.clear();

        if (!m_proExec->queue().isEmpty())
        {
            foreach (GtTask* task, m_proExec->queue())
            {
                if (task != Q_NULLPTR)
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