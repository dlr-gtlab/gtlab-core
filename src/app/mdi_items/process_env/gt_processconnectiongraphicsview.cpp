/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectiongraphicsview.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_processconnectionview.h"
#include "gt_processconnectionmodel.h"
#include "gt_processconnectionscene.h"
#include "gt_processpropertyportentity.h"
#include "gt_processpropertyconnectionentity.h"
#include "gt_propertyconnection.h"
#include "gt_task.h"

#include "gt_processconnectiongraphicsview.h"

GtProcessConnectionGraphicsView::GtProcessConnectionGraphicsView(
    GtProcessConnectionView* outputView,
    GtProcessConnectionView* inputView,
    QWidget* parent) :
    QGraphicsView(parent),
    m_outputView(outputView),
    m_inputView(inputView),
    m_scene(new GtProcessConnectionScene(this)),
    m_root(nullptr)
{
    setScene(m_scene);
    setRenderHint(QPainter::Antialiasing);
}

void
GtProcessConnectionGraphicsView::updatePorts(GtProcessConnectionView* view,
        QPaintEvent* /*event*/)
{
    // check view
    if (!view)
    {
        return;
    }

    const int rowSize = view->sizeHintForRow(0);

    //        qDebug() << "   |-> row size = " << rowSize;

    QModelIndex lastVisibleIndex = view->lastVisibleIndex();
    const int lastVisibleIndexY = view->visualRect(lastVisibleIndex).y();
    int lastVisibleY = view->viewport()->height();

    if (lastVisibleIndexY < lastVisibleY)
    {
        lastVisibleY = lastVisibleIndexY;
    }

    //        qDebug() << "   |-> last visible = " << lastVisibleIndex.data();
//                qDebug() << "   |-> last position = " << lastVisibleY;

    // check whether view is equal to input view
    if (m_inputView == view)
    {
        //        qDebug() << "#### updating input ports...";
        GtProcessPortMap tmpMap = m_inputPorts;
        GtProcessConnectionModel* model = m_inputView->connectionModel();

        if (!model)
        {
            return;
        }

        for (int i = 0; i <= lastVisibleY; i = i + rowSize)
        {
            QModelIndex index = view->indexAt(QPoint(0, i));

            if (index.isValid())
            {
                bool isProperty =
                        index.data(
                            GtProcessConnectionModel::PropertyRole).toBool();

                if (isProperty)
                {
                    GtProcessConnectionItem* item = model->itemFromIndex(index);

                    if (item)
                    {
                        GtProcessPropertyPortEntity* entity =
                            updatePortEntity(item, m_inputPorts, rowSize,
                                             i, Qt::AlignRight);

                        if (entity)
                        {
                            if (tmpMap.contains(item))
                            {
                                tmpMap.remove(item);
                            }
                        }
                    }
                }
            }
        }

        // clear hidden ports
        for (auto e : tmpMap.keys())
        {
            GtProcessPropertyPortEntity* entity =
                m_inputPorts.take(e);

            delete entity;
        }
    }

    // check whether view is equal to output view
    if (m_outputView == view)
    {
        //        qDebug() << "#### updating output ports...";
        GtProcessPortMap tmpMap = m_outputPorts;
        GtProcessConnectionModel* model = m_outputView->connectionModel();

        if (!model)
        {
            return;
        }

        for (int i = 0; i <= lastVisibleY; i = i + rowSize)
        {
            QModelIndex index = view->indexAt(QPoint(0, i));

            if (index.isValid())
            {
                bool isProperty =
                        index.data(
                            GtProcessConnectionModel::PropertyRole).toBool();

                if (isProperty)
                {
                    GtProcessConnectionItem* item = model->itemFromIndex(index);

                    if (item)
                    {
                        GtProcessPropertyPortEntity* entity =
                            updatePortEntity(item, m_outputPorts, rowSize,
                                             i, Qt::AlignLeft);

                        if (entity)
                        {
                            if (tmpMap.contains(item))
                            {
                                tmpMap.remove(item);
                            }
                        }
                    }
                }
            }
        }

        // clear hidden ports
        for (auto e : tmpMap.keys())
        {
            GtProcessPropertyPortEntity* entity =
                m_outputPorts.take(e);

            if (entity)
            {
                delete entity;
            }
        }
    }

    updateConnections();
}

void
GtProcessConnectionGraphicsView::createConnections(
        const QList<GtPropertyConnection*>& list)
{
    foreach (GtPropertyConnection* connection, list)
    {
        createConnection(connection);
    }
}

GtProcessPropertyConnectionEntity*
GtProcessConnectionGraphicsView::createConnection(
        GtPropertyConnection* connection)
{
    // check connection
    if (!connection)
    {
        return nullptr;
    }

    QList<GtProcessPropertyConnectionEntity*> list =
            m_scene->findItems<GtProcessPropertyConnectionEntity*>();

    // check whether connection already exists in map
    if (std::any_of(std::begin(list), std::end(list), [&connection](GtProcessPropertyConnectionEntity* entity) {
            return connection == entity->connection();
        }))
    {
        return nullptr;
    }

    // check whether connection already exists in map
//    if (m_connections.contains(connection))
//    {
//        return Q_NULLPTR;
//    }

    // create new conneciton entity
    GtProcessPropertyConnectionEntity* entity =
            new GtProcessPropertyConnectionEntity(connection);

    // insert connection entity to map
//    m_connections.insert(connection, entity);

    // add entity to scene
    m_scene->addItem(entity);

    qDebug() << "connection entity created!";

    // return success
    return entity;
}

GtProcessConnectionScene*
GtProcessConnectionGraphicsView::connectionScene()
{
    return m_scene;
}

void
GtProcessConnectionGraphicsView::setRoot(GtTask* task)
{
    m_root = task;
}

GtTask*
GtProcessConnectionGraphicsView::root()
{
    return m_root;
}

GtProcessPropertyPortEntity*
GtProcessConnectionGraphicsView::findOutputPortEntity(const QString& uuid,
                                                      const QString& propId)
{
//    qDebug() << "#### findOutputPortEntity";
    return findPortEntityHelper(m_outputPorts, uuid, propId);
}

GtProcessPropertyPortEntity*
GtProcessConnectionGraphicsView::findInputPortEntity(const QString& uuid,
                                                     const QString& propId)
{
//    qDebug() << "#### findInputPortEntity";
    return findPortEntityHelper(m_inputPorts, uuid, propId);
}

GtProcessPropertyPortEntity*
GtProcessConnectionGraphicsView::findPortEntityHelper(GtProcessPortMap& map,
                                                      const QString& uuid,
                                                      const QString& propId)
{
//    qDebug() << "   |-> " << uuid << "/" << propId;

    for (auto e : map.keys())
    {
//        qDebug() << "   |-> " << e->parentComponentUuid() << "/" <<
//                    e->propertyId();

        if (e->parentComponentUuid() == uuid && e->propertyId() == propId)
        {
//            qDebug() << "   |-> found!";

            return map.value(e);
        }
    }

    return nullptr;
}

void
GtProcessConnectionGraphicsView::updateConnections()
{
    // TODO: optimize for separate update based on input or output changes

    QList<GtProcessPropertyConnectionEntity*> list =
            m_scene->findItems<GtProcessPropertyConnectionEntity*>();

    foreach (GtProcessPropertyConnectionEntity* entity, list)
    {
        GtPropertyConnection* connection = entity->connection();

        if (!connection)
        {
            continue;
        }

        // find output port entity
        GtProcessPropertyPortEntity* outPort =
                findOutputPortEntity(connection->sourceUuid(),
                                     connection->sourceProp());

        entity->setStartPort(outPort);

        // find input port entity
        GtProcessPropertyPortEntity* inPort =
                findInputPortEntity(connection->targetUuid(),
                                    connection->targetProp());

        entity->setEndPort(inPort);

        if (!entity->startPortExists())
        {
            // check view
            if (!m_outputView)
            {
                continue;
            }

            GtProcessConnectionItem* item =
                    m_outputView->itemById(connection->sourceUuid(),
                                           connection->sourceProp());

            if (!item)
            {
                continue;
            }

            QModelIndex index = m_outputView->lastVisibleParentIndex(item);

            if (!index.isValid())
            {
                continue;
            }

            QRect rect = m_outputView->visualRect(index);

            entity->setStartPoint(QPointF(0., rect.y() + rect.height() / 2));
        }

        if (!entity->endPortExists())
        {
            // check view
            if (!m_inputView)
            {
                continue;
            }

            GtProcessConnectionItem* item =
                    m_inputView->itemById(connection->targetUuid(),
                                          connection->targetProp());

            if (!item)
            {
                continue;
            }

            QModelIndex index = m_inputView->lastVisibleParentIndex(item);

            if (!index.isValid())
            {
                continue;
            }

            QRect rect = m_inputView->visualRect(index);

            entity->setEndPoint(QPointF(frameSize().width(),
                                        rect.y() + rect.height() / 2));
        }

        entity->updatePath();
    }

    // iterate over all connection map entries
//    for (auto e : m_connections.keys())
//    {
//        // get entity from map
//        GtProcessPropertyConnectionEntity* entity = m_connections.value(e);

//        // check entity
//        if (entity == Q_NULLPTR)
//        {
//            continue;
//        }


//    }
}

void
GtProcessConnectionGraphicsView::resizeEvent(QResizeEvent* event)
{
    setSceneRect(0, 0, frameSize().width(), frameSize().height());

    QGraphicsView::resizeEvent(event);
}

GtProcessPropertyPortEntity*
GtProcessConnectionGraphicsView::updatePortEntity(GtProcessConnectionItem* item,
        GtProcessPortMap& map,
        int rowHeight,
        int vPos,
        Qt::AlignmentFlag align)
{
    // check item
    if (!item)
    {
        qDebug() << "item == Q_NULLPTR";
        return nullptr;
    }

    // check alignment - only left right supported
    if (align != Qt::AlignRight && align != Qt::AlignLeft)
    {
        qDebug() << "align != Qt::AlignRight || align != Qt::AlignLeft";
        return nullptr;
    }

    // check whether port entity already exists
    GtProcessPropertyPortEntity::PortTypes type =
        GtProcessPropertyPortEntity::INPUT_PORT;

    int hDisplacement = 0;

    if (align == Qt::AlignLeft)
    {
        type = GtProcessPropertyPortEntity::OUTPUT_PORT;
    }
    else
    {
        hDisplacement = frameSize().width();
    }

    GtProcessPropertyPortEntity* retval = nullptr;

    const int portSize = (rowHeight * 3) / 4;
    const int vDisplacement = (rowHeight - portSize) / 2 + 1;

    if (map.contains(item))
    {
        // get existing entity
        retval = map.value(item);

        // set new position
        retval->setPos(hDisplacement, vPos + vDisplacement + portSize / 2);
    }
    else
    {
        // create new entity
        retval = new GtProcessPropertyPortEntity(- portSize / 2, - portSize / 2,
                                                 portSize, portSize,
                                                 type, item);

        retval->setPos(hDisplacement, vPos + vDisplacement + portSize / 2);

        map.insert(item, retval);
        scene()->addItem(retval);
    }

    return retval;
}
