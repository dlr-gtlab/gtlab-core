/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QDebug>

#include "gt_processeditorscene.h"
#include "gt_object.h"
#include "gt_datamodel.h"
#include "gt_processfactory.h"
#include "gt_task.h"
#include "gt_taskentity.h"
#include "gt_tasklinkentity.h"
#include "gt_processdata.h"
#include "gt_logging.h"

GtProcessEditorScene::GtProcessEditorScene(QObject* parent) :
    GtGraphicsScene(parent),
    m_root(nullptr),
    m_current(nullptr),
    m_temp(nullptr),
    m_model(gtDataModel),
    m_mode(M_DEFAULT)
{
    connect(gtDataModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            SLOT(onRowsRemoved(QModelIndex,int,int)));
}

void
GtProcessEditorScene::setRoot(GtTask* task)
{
    if (m_root)
    {
        return;
    }

//    GtProcessData* pd = qobject_cast<GtProcessData*>(task->parentObject());

//    if (!pd)
//    {
//        gtError() << "process data not found!";
//        return;
//    }

    QModelIndex index = m_model->indexFromObject(task);

    if (!index.isValid())
    {
        gtError() << "root index not found!";
        return;
    }

    m_rootIndex = QPersistentModelIndex(index);

    m_root = task;
    switchTo(m_root);
}

void
GtProcessEditorScene::setPreviewMode()
{
    switchMode(M_PREVIEW);
}

void
GtProcessEditorScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
//    qDebug() << "GtProcessEditorScene::dragEnterEvent";
    const QMimeData* mimeData = event->mimeData();

    GtObject* obj = gtDataModel->objectFromMimeData(mimeData, true,
                                                    gtProcessFactory);

    if (obj)
    {
//        qDebug() << "obj = " << obj->objectName();
//        qDebug() << "uuid = " << obj->uuid();
        delete obj;

        emit setPreviewMode();

//        event->accept();
    }
    else
    {
//        event->ignore();
    }

//    event->ignore();

    GtGraphicsScene::dragEnterEvent(event);
}

//void
//GtProcessEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent)
//{
//    qDebug() << "GtProcessEditorScene::mouseReleaseEvent";
//    if (m_mode == M_PREVIEW)
//    {
//        switchMode(M_DEFAULT);
//    }

//    GtGraphicsScene::mouseReleaseEvent(mouseEvent);
//}

void
GtProcessEditorScene::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
//    qDebug() << "GtProcessEditorScene::dragLeaveEvent";

    if (m_mode == M_PREVIEW)
    {
        switchMode(M_DEFAULT);
    }

    GtGraphicsScene::dragLeaveEvent(event);
}

//void
//GtProcessEditorScene::dragMoveEvent(QGraphicsSceneDragDropEvent* /*event*/)
//{

//}

void
GtProcessEditorScene::populate(GtTask* task)
{
    if (!task)
    {
        return;
    }

    clear();

    GtTaskEntity* entity = new GtTaskEntity(task);
    addItem(entity);
    entity->populateTask();

    if (m_mode == M_PREVIEW)
    {
        entity->rootNode()->setToPreviewMode();
    }

    setSceneRect(itemsBoundingRect());

    connect(entity, SIGNAL(switchTo(GtCalculatorEntity*)),
            SLOT(onSwitchItem(GtCalculatorEntity*)));
}

void
GtProcessEditorScene::switchTo(GtTask* task)
{
    if (!task)
    {
        return;
    }

    QList<GtTaskEntity*> entities = findItems<GtTaskEntity*>();

    // disconnect all task entity signals
    foreach (GtTaskEntity* entity, entities)
    {
        disconnect(entity, SIGNAL(switchTo(GtCalculatorEntity*)), this,
                   SLOT(onSwitchItem(GtCalculatorEntity*)));
    }

    m_current = task;

    populate(task);
}

void
GtProcessEditorScene::switchMode(GtProcessEditorScene::Mode mode)
{
    if (m_mode == mode)
    {
        return;
    }

    m_mode = mode;

    if (!m_root)
    {
        return;
    }

    if (mode == M_PREVIEW)
    {
        if (m_temp)
        {
            return;
        }

        GtObject* obj = m_current->clone();

        if (!obj)
        {
            return;
        }

        GtTask* preview = qobject_cast<GtTask*>(obj);

        if (!preview)
        {
            delete obj;
            return;
        }

        m_temp = m_current;

        switchTo(preview);
    }
    else if (mode == M_DEFAULT)
    {
        if (!m_temp)
        {
            return;
        }

        switchTo(m_temp);
        m_temp = nullptr;
    }
}

bool
GtProcessEditorScene::isMemberOf(const QModelIndex& child,
                                 const QModelIndex& parent)
{
    if (!child.isValid() || !parent.isValid())
    {
        return false;
    }

    if (child.model() != parent.model())
    {
        return false;
    }

    if (child == parent)
    {
        return true;
    }

    return isMemberOf(child.parent(), parent);
}

void
GtProcessEditorScene::onSwitchItem(GtCalculatorEntity* entity)
{
    if (!entity)
    {
        return;
    }

    if (GtTaskLinkEntity* tle = qobject_cast<GtTaskLinkEntity*>(entity))
    {
        switchTo(tle->target());
    }
}

void
GtProcessEditorScene::onRowsRemoved(const QModelIndex& parent, int start,
                                    int end)
{
    if (m_mode == M_PREVIEW)
    {
        // nothing to do in preview mode
        return;
    }

    if (isMemberOf(parent, m_rootIndex))
    {
        qDebug() << "GtProcessEditorScene::onRowsRemoved";
        qDebug() << parent;
        qDebug() << "start = " << start;
        qDebug() << "end   = " << end;

        for (int i = start; i <= end; i++)
        {
            QModelIndex child = parent.child(start, 0);

            GtObject* obj = gtDataModel->objectFromIndex(child);

            if (obj)
            {
                qDebug() << "deleting " << obj->objectName() << "...";
            }
        }
    }
}

