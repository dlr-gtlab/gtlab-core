/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_taskentity.h"

#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_calculatorentity.h"
#include "gt_taskarrowentity.h"
#include "gt_datamodelportentity.h"
#include "gt_addtaskglobalbutton.h"
#include "gt_taskconnectionentity.h"
#include "gt_taskpipeentity.h"
#include "gt_application.h"
#include "gt_palette.h"

#include <QPainter>
#include <QGraphicsTextItem>
#include <QGraphicsSceneDragDropEvent>
#include <QDebug>

#include <algorithm>

GtTaskEntity::GtTaskEntity(GtTask* task, QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_w(200),
    m_h(150),
    m_nodeSize(1.),
    m_siblingDistance(0.),
    m_treeDistance(0.),
    m_task(task),
    m_rootEntity(nullptr)
{
//    setAcceptDrops(true);
}

void
GtTaskEntity::paint(QPainter* painter,
                    const QStyleOptionGraphicsItem* /*option*/,
                    QWidget* /*widget*/)
{
    painter->save();

    QPen pen = painter->pen();
    pen.setCosmetic(true);
    painter->setPen(pen);

    if (!gtApp->inDarkMode())
    {
        painter->setBrush(QBrush(Qt::white));
    }
    else
    {
        painter->setBrush(QBrush(GtPalette::Color::basicDark()));
    }

    QRectF rect(10., 10., m_w - 20., m_h - 20.);

    painter->drawRoundedRect(rect, 10, 10);

    painter->setRenderHint(QPainter::Antialiasing, false);

    pen.setColor(QColor(200, 200, 255, 125));
    painter->setPen(pen);
    painter->drawLines(m_lines);

    painter->restore();
}

QRectF
GtTaskEntity::boundingRect() const
{
    return {0, 0, double(m_w), double(m_h)};
}

void
GtTaskEntity::populateTask()
{
    // clear
    qDeleteAll(m_subCalcs);
    m_subCalcs.clear();

    // check for datamodel object
    if (!m_task)
    {
        return;
    }

    // invisible root item
    m_rootEntity = new GtCalculatorEntity(m_task, this);

    connect(m_rootEntity, SIGNAL(switchTo(GtCalculatorEntity*)),
            SIGNAL(switchTo(GtCalculatorEntity*)));

    // initialize all calculator nodes
    initializeNodes(m_rootEntity);

    // calculate initial node norms
    calculateInitialNodeNorms(m_rootEntity);

    // calculate final entity positions
    calculateFinalPositions(m_rootEntity);

    // update shapes recursively
    m_rootEntity->updateShape();

    // calculate task entity size
    calculateSize();

    // create pipe (entry to exit)
    auto pipe = new GtTaskPipeEntity(this);
    pipe->updateShape();

    // task title
    auto title = new QGraphicsTextItem(this);
    title->setPlainText(m_task->objectName());
    const int tw = int(title->boundingRect().width());
    title->setPos((m_w / 2) - (tw / 2), m_h - 5);

    // datamodel port
    auto dmport = new GtDataModelPortEntity(this);
    dmport->setPos(QPoint(m_w - 20, 10));

    // add global button
    auto btn = new GtAddTaskGlobalButton(this);
    btn->setPos(10 + btn->boundingRect().width(), 10);

    // connections
//    QList<GtDataModelPortEntity*> dmports = findItems<GtDataModelPortEntity*>();
//    dmports.removeOne(dmport);

//    foreach (GtDataModelPortEntity* dmp, dmports)
//    {
//        GtTaskConnectionEntity* c = new GtTaskConnectionEntity(this);
//        c->setFromPort(dmport);
//        c->setToPort(dmp);
    //    }
}

GtCalculatorEntity*
GtTaskEntity::rootNode()
{
    return m_rootEntity;
}

void
GtTaskEntity::calculateSize()
{
    if (!m_task)
    {
        return;
    }

    if (m_rootEntity->isLeaf())
    {
        m_w = 200;
        m_h = 150;
    }
    else
    {
        m_w = calculateWidth();
        m_h = calculateHeight();
    }

    m_lines.clear();

    int tmpWidth = 20;
    int tmpHeight = 20;

    QRectF rect(10., 10., m_w - 20., m_h - 20.);

    qreal left = int(rect.left()) - (int(rect.left()) % tmpWidth) + tmpWidth;
    qreal top = int(rect.top()) - (int(rect.top()) % tmpHeight) + tmpHeight;

    for (qreal y = top; y < rect.bottom(); y += tmpHeight)
    {
        m_lines.append(QLineF(rect.left() + 1, y, rect.right() - 1, y));
    }

    for (qreal x = left; x < rect.right(); x += tmpWidth)
    {
        m_lines.append(QLineF(x, rect.top() + 1, x, rect.bottom() - 1));
    }
}

int
GtTaskEntity::calculateWidth()
{
    GtCalculatorEntity* mostLeft = m_rootEntity->leftMostLeaf();
    GtCalculatorEntity* mostRight = m_rootEntity->rightMostLeaf();

    double dnw = GtCalculatorEntity::defaultNodeWidth();

    double tmpw = (mostRight->normX() * dnw) -
                  (mostLeft->normX() * dnw) + dnw + 60.;

    return int(tmpw);
}

int
GtTaskEntity::calculateHeight()
{
    QList<GtCalculatorEntity*> calcs = m_rootEntity->subNodes();

    int maxSize = 0;

    foreach (GtCalculatorEntity* calc, calcs)
    {
        int cSize = calc->absoluteHeight(0);
        if (cSize > maxSize)
        {
            maxSize = cSize;
        }
    }

    return maxSize + 30;
}

int
GtTaskEntity::calculateFirstRowHeight()
{
    int maxSize = 0;

    foreach (GtCalculatorEntity* calc, m_subCalcs)
    {
        int cSize = int(calc->boundingRect().height());
        if (cSize > maxSize)
        {
            maxSize = cSize;
        }
    }

    return maxSize;
}

QList<QGraphicsItem*>
GtTaskEntity::allChildItems(QGraphicsItem* item)
{
    {
        QList<QGraphicsItem*> retval;

        if (!item)
        {
            item = this;
        }
        else
        {
            retval << item;
        }

        foreach (QGraphicsItem* c, item->childItems())
        {
            retval.append(allChildItems(c));
        }

        return retval;
    }
}

void
GtTaskEntity::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    qDebug() << "GtTaskEntity::dragEnterEvent";
    event->setAccepted(false);
}

void
GtTaskEntity::initializeNodes(GtCalculatorEntity* node, int depth)
{
    Q_ASSERT(node == nullptr);

    node->setNormX(-1.);
    node->setNormY(depth);
    node->setMod(0.);

    foreach (GtCalculatorEntity* child, node->subNodes())
    {
        initializeNodes(child, depth + 1);
    }
}

void
GtTaskEntity::calculateInitialNodeNorms(GtCalculatorEntity* node)
{
    foreach (GtCalculatorEntity* child, node->subNodes())
    {
        calculateInitialNodeNorms(child);
    }

    if (node->isLeaf())
    {
        if (node->isLeftMost())
        {
            node->setNormX(0.);
        }
        else
        {
            double tmpx = node->previousSibling()->normX() + m_nodeSize +
                          m_siblingDistance;
            node->setNormX(tmpx);
        }
    }
    else if (node->subNodeCount() == 1)
    {
        if (node->isLeftMost())
        {
            double tmpx = node->leftMostChild()->normX();
            node->setNormX(tmpx);
        }
        else
        {
            double tmpx = node->previousSibling()->normX() + m_nodeSize +
                          m_siblingDistance;
            node->setNormX(tmpx);
            double tmpmod = tmpx - node->leftMostChild()->normX();
            node->setMod(tmpmod);
        }
    }
    else
    {
        GtCalculatorEntity* leftChild = node->leftMostChild();
        GtCalculatorEntity* rightChild = node->rightMostChild();
        double mid = (leftChild->normX() + rightChild->normX()) / 2;

        if (node->isLeftMost())
        {
            node->setNormX(mid);
        }
        else
        {
            double tmpx = node->previousSibling()->normX() + m_nodeSize +
                          m_siblingDistance;
            node->setNormX(tmpx);
            double tmpmod = tmpx - mid;
            node->setMod(tmpmod);
        }
    }

    if (node->subNodeCount() > 0 && !node->isLeftMost())
    {
        // check for conflicts
        checkForConflicts(node);
    }
}

void
GtTaskEntity::calculateFinalPositions(GtCalculatorEntity* node, double modSum)
{
    node->setNormX(node->normX() + modSum);
    modSum += node->mod();

    node->calculatePosition();

    foreach (GtCalculatorEntity* child , node->subNodes())
    {
        calculateFinalPositions(child, modSum);
    }
}

void
GtTaskEntity::checkForConflicts(GtCalculatorEntity* node)
{
    double minDistance = m_treeDistance + m_nodeSize;
    double shiftValue = 0.;

    QMap<int, double> nodeContour;

    leftContour(node, 0., nodeContour);

    GtCalculatorEntity* sibling = node->leftMostSibling();

    while (sibling && sibling != node)
    {
        QMap<int, double> siblingContour;
        rightContour(sibling, 0., siblingContour);

        int maxlvl = qMin(siblingContour.size(), nodeContour.size());

        for (int level = node->normY() + 1; level <= maxlvl; level++)
        {
            double distance = nodeContour[level] - siblingContour[level];

            if (distance + shiftValue < minDistance)
            {
                shiftValue = minDistance - distance;
            }
        }

        if (shiftValue > 0)
        {
            node->setNormX(node->normX() + shiftValue);
            node->setMod(node->mod() + shiftValue);

            centerNodesBetween(node, sibling);

            shiftValue = 0;
        }

        sibling = sibling->nextSibling();
    }
}

void
GtTaskEntity::leftContour(GtCalculatorEntity* node, double modSum,
                          QMap<int, double>& nodeContour)
{
    if (!nodeContour.contains(node->normY()))
    {
        nodeContour.insert(node->normY(), node->normX() + modSum);
    }
    else
    {
        double nodey = node->normY();
        double nodex = node->normX();
        nodeContour[nodey] = qMin(nodeContour.value(node->normY()),
                                  nodex + modSum);
    }

    modSum += node->mod();
    foreach (GtCalculatorEntity* child, node->subNodes())
    {
        leftContour(child, modSum, nodeContour);
    }
}

void
GtTaskEntity::rightContour(GtCalculatorEntity* node, double modSum,
                           QMap<int, double>& nodeContour)
{
    if (!nodeContour.contains(node->normY()))
    {
        nodeContour.insert(node->normY(), node->normX() + modSum);
    }
    else
    {
        double nodey = node->normY();
        double nodex = node->normX();
        nodeContour[nodey] = qMax(nodeContour.value(node->normY()),
                                  nodex + modSum);
    }

    modSum += node->mod();
    foreach (GtCalculatorEntity* child, node->subNodes())
    {
        rightContour(child, modSum, nodeContour);
    }
}

void
GtTaskEntity::centerNodesBetween(GtCalculatorEntity* leftNode,
                                 GtCalculatorEntity* rightNode)
{
    int leftIndex = rightNode->childNumber();
    int rightIndex = leftNode->childNumber();

    int numNodesBetween = (rightIndex - leftIndex) - 1;

    if (numNodesBetween > 0)
    {
        double distanceBetweenNodes = (leftNode->normX() - rightNode->normX()) /
                                      (numNodesBetween + 1);

        int count = 1;
        for (int i = leftIndex + 1; i < rightIndex; i++)
        {
            GtCalculatorEntity* middleNode =
                    leftNode->parentNode()->subNodes()[i];

            double desiredX = rightNode->normX() +
                              (distanceBetweenNodes * count);
            double offset = desiredX - middleNode->normX();

            middleNode->setNormX(middleNode->normX() + offset);
            middleNode->setMod(middleNode->mod() + offset);

            count++;
        }

        checkForConflicts(leftNode);
    }
}

double
GtTaskEntity::maxContourValue(const QMap<int, double>& map)
{
    return *std::max_element(std::begin(map), std::end(map));
}

