/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QIcon>
#include <QPropertyAnimation>
#include <QGraphicsSceneHoverEvent>
#include <QApplication>
#include <QDrag>
#include <QWidget>
#include <QMimeData>
#include <QDebug>

#include "gt_calculatorentity.h"
#include "gt_calculator.h"
#include "gt_task.h"
#include "gt_objectui.h"
#include "gt_application.h"
#include "gt_calculatorpropertyentity.h"
#include "gt_datamodelportentity.h"
#include "gt_taskarrowentity.h"
#include "gt_tasklink.h"
#include "gt_tasklinkentity.h"
#include "gt_datamodel.h"
#include "gt_logging.h"
#include "gt_palette.h"

GtCalculatorEntity::GtCalculatorEntity(GtTask* task,
                                       QGraphicsItem* parent) :
    QGraphicsObject(parent),
    m_calculator(nullptr),
    m_childLeft(nullptr),
    m_childRight(nullptr),
    m_parentEntity(nullptr)
{
    init();
    populateCalculator(task->calculators());
}

GtCalculatorEntity::GtCalculatorEntity(GtCalculator* calc,
                                       GtCalculatorEntity* parent) :
    QGraphicsObject(parent),
    m_calculator(calc),
    m_parentEntity(parent)
{
    init();
    populateCalculator();
}

void
GtCalculatorEntity::init()
{
    m_w = 140;
    m_h = 80;
    m_anim = nullptr;

    if (!isRoot())
    {
        setAcceptHoverEvents(true);
        setFlag(QGraphicsItem::ItemIsPanel, true);
        setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
//        setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        setOpacity(0.7);

        GtObjectUI* oui = gtApp->defaultObjectUI(m_calculator);
        if (oui)
        {
            m_icon = oui->icon(m_calculator).pixmap(16, 16);
        }
    }
}

void
GtCalculatorEntity::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* /*option*/,
                          QWidget* /*widget*/)
{
    if (isRoot())
    {
        return;
    }

    painter->save();

    QPen pen = painter->pen();
    pen.setCosmetic(true);

//    if (isSelected())
//    {
//        pen.setColor(Qt::darkGreen);
//    }

    painter->setPen(pen);

    // title
    if (isSelected())
    {
        if (gtApp->inDarkMode())
        {
            painter->setBrush(QBrush(GtPalette::Color::basicDark()));
        }
        else
        {
            painter->setBrush(QBrush(Qt::white));
        }
    }
    else
    {
        if (gtApp->inDarkMode())
        {
            painter->setBrush(QBrush(Qt::darkGray));
        }
        else
        {
            painter->setBrush(QBrush(Qt::gray));
        }
    }

    QPainterPath titlePath;
    titlePath.setFillRule(Qt::WindingFill);
    titlePath.addRoundedRect(QRect(0, 0, m_w, 20), 5, 5);
    titlePath.addRect(QRect(0, 10, 10, 10));
    titlePath.addRect(QRect(m_w - 10, 10, 10, 10));
    painter->drawPath(titlePath.simplified());

    // footer
    painter->setBrush(QBrush(Qt::lightGray));
    QPainterPath headerPath;
    headerPath.setFillRule(Qt::WindingFill);
    headerPath.addRoundedRect(QRect(0, 20, m_w, m_h - 20), 5, 5);
    headerPath.addRect(QRect(0, 20, 10, 10));
    headerPath.addRect(QRect(m_w - 10, 20, 10, 10));
    painter->drawPath(headerPath.simplified());

    painter->drawPixmap(2, 2, 16, 16, m_icon);
    painter->drawText(QPoint(25, 14), m_calculator->objectName());

    painter->restore();
}

QRectF
GtCalculatorEntity::boundingRect() const
{
    return {0, 0, double(m_w), double(m_h)};
}

int
GtCalculatorEntity::absoluteHeight(int sum)
{
    sum += defaultNodeHeight();
    int sumTmp = sum;

    foreach (GtCalculatorEntity* calc, m_subCalcs)
    {
        int tmp = calc->absoluteHeight(sum);

        if (tmp > sumTmp)
        {
            sumTmp = tmp;
        }
    }

    sum = sumTmp;

    return sum;
}

void
GtCalculatorEntity::alignChildren(int maxHeight)
{
    if (m_subCalcs.isEmpty())
    {
        return;
    }

    int sumW = std::accumulate(std::begin(m_subCalcs), std::end(m_subCalcs), 0.0, [](double sum, const GtCalculatorEntity* ce){
        return sum + ce->boundingRect().width();
    });

    sumW += (m_subCalcs.size() - 1) * 60;

    int tmpH = m_h;

    if (m_h < maxHeight)
    {
        tmpH = maxHeight;
    }

    QPoint tmpPos(m_w / 2 - sumW / 2, tmpH + 40);

    foreach (GtCalculatorEntity* ce, m_subCalcs)
    {
        ce->setPos(tmpPos);

        tmpPos = QPoint(tmpPos.x() + ce->boundingRect().width() + 60,
                        tmpPos.y());
    }

    int frh = calculateFirstRowHeight();
    foreach (GtCalculatorEntity* ce, m_subCalcs)
    {
        ce->alignChildren(frh);
    }

    foreach (GtTaskArrowEntity* arrow, m_directConnections)
    {
        arrow->updateShape();
    }
}

void
GtCalculatorEntity::calculatePosition()
{
    if (isRoot())
    {
        setPos(normX() * defaultNodeWidth() + 60., -100.);
    }
    else
    {
        double tmpx = (normX() - m_parentEntity->normX()) * defaultNodeWidth();
        double tmpy = (normY() - m_parentEntity->normY()) * defaultNodeHeight();

        setPos(tmpx, tmpy);
    }
}

void
GtCalculatorEntity::populateCalculator()
{
    // clear
    qDeleteAll(m_subCalcs);
    m_subCalcs.clear();

    if (!m_calculator)
    {
        return;
    }

    // properties
    const QList<GtAbstractProperty*>& props = m_calculator->properties();

    QPoint tmpPos(0, 40);

    foreach (GtAbstractProperty* prop, props)
    {
        auto pe = new GtCalculatorPropertyEntity(prop, this);
        pe->setPos(tmpPos);
        tmpPos = QPoint(tmpPos.x(),
                        tmpPos.y() + pe->boundingRect().height());
    }

    // sub calculators
    QList<GtCalculator*> calcs =
            m_calculator->findDirectChildren<GtCalculator*>();

    calculateSize();

    populateCalculator(calcs);

    // data model port
    auto dmport = new GtDataModelPortEntity(this);
    dmport->setPos(QPoint(m_w - 10, 0));
}

void
GtCalculatorEntity::populateCalculator(const QList<GtCalculator*>& calcs)
{
    foreach (GtCalculator* calc, calcs)
    {
//        GtCalculatorEntity* ce = nullptr;

//        if (GtTaskLink* taskLink = qobject_cast<GtTaskLink*>(calc))
//        {
//            ce = new GtTaskLinkEntity(taskLink, this);
//        }
//        else
//        {
//            ce = new GtCalculatorEntity(calc, this);
//        }

//        connect(ce, SIGNAL(switchTo(GtCalculatorEntity*)),
//                SIGNAL(switchTo(GtCalculatorEntity*)));

//        m_subCalcs << ce;
        if (!appendCalculator(calc))
        {
            gtError() << tr("Could not create calculator entity!");
        }

//        if (ceTmp)
//        {
//            GtTaskArrowEntity* arrow =
//                    new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_DIRECT,
//                                          this);
//            arrow->link(ceTmp, ce);
//            m_directConnections << arrow;
//        }

//        ceTmp = ce;
    }

//    if (!m_subCalcs.isEmpty() && !isRoot())
//    {
//        GtCalculatorEntity* leftChild = m_subCalcs.first();
//        m_childLeft =
//                new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_LEFT_CHILD,
//                                      this);
//        m_childLeft->link(this, leftChild);

//        GtCalculatorEntity* rightChild = m_subCalcs.last();
//        m_childRight =
//                new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_RIGHT_CHILD,
//                                      this);
//        m_childRight->link(rightChild, this);
//    }
    updateArrows();
}

GtCalculatorEntity*
GtCalculatorEntity::insertCalculator(GtCalculator* calc,
                                                         int idx)
{
    if (idx < 0 || idx > m_subCalcs.size())
    {
        qWarning() << "index error!"
                   << " (GtCalculatorEntity::insertCalculator)";
        return nullptr;
    }

    GtCalculatorEntity* ce = nullptr;

    if (auto taskLink = qobject_cast<GtTaskLink*>(calc))
    {
        ce = new GtTaskLinkEntity(taskLink, this);
    }
    else
    {
        ce = new GtCalculatorEntity(calc, this);
    }

    connect(ce, SIGNAL(switchTo(GtCalculatorEntity*)),
            SIGNAL(switchTo(GtCalculatorEntity*)));

    m_subCalcs.insert(idx, ce);

    return ce;
}

GtCalculatorEntity*
GtCalculatorEntity::appendCalculator(GtCalculator* calc)
{
    return insertCalculator(calc, m_subCalcs.size());
}

void
GtCalculatorEntity::updateArrows()
{
    const int noc = m_subCalcs.size();

    // direct connections
    if (noc > 1)
    {
        for (int i = 1; i < noc; i++)
        {
            const int noa = m_directConnections.size();
            GtTaskArrowEntity* arrow = nullptr;

            if ((i - 1) >= noa)
            {
                arrow = new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_DIRECT,
                                              this);
                m_directConnections << arrow;
            }
            else
            {
                arrow = m_directConnections[i - 1];
            }

            arrow->link(m_subCalcs[i - 1], m_subCalcs[i]);
        }
    }

    // left - right connections
    if (noc > 0 && !isRoot())
    {
        // left
        GtCalculatorEntity* leftChild = m_subCalcs.first();
        if (!m_childLeft)
        {
            m_childLeft =
                    new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_LEFT_CHILD,
                                          this);
        }
        m_childLeft->link(this, leftChild);

//        // right
        GtCalculatorEntity* rightChild = m_subCalcs.last();
        if (!m_childRight)
        {
            m_childRight =
                    new GtTaskArrowEntity(GtTaskArrowEntity::ARROW_RIGHT_CHILD,
                                          this);
        }
        m_childRight->link(rightChild, this);
    }
}

void
GtCalculatorEntity::calculateSize()
{
    if (!m_calculator)
    {
        return;
    }

    const QList<GtAbstractProperty*>& props = m_calculator->properties();

    if (props.isEmpty())
    {
        m_h = 60;
    }
    else
    {
        m_h = 60 + props.size() * 20;
    }
}

void
GtCalculatorEntity::runEnterAnimation()
{
    if (!m_anim)
    {
        m_anim = new QPropertyAnimation(this, "opacity");
        m_anim->setDuration(300);
        m_anim->setKeyValueAt(0, 0.7);
        m_anim->setKeyValueAt(1, 1);
        connect(m_anim, SIGNAL(finished()), SLOT(onAnimationFinished()),
                Qt::UniqueConnection);
    }

    if (m_anim->direction() == QPropertyAnimation::Backward)
    {
        m_anim->pause();
        m_anim->setDirection(QPropertyAnimation::Forward);
        m_anim->resume();
    }

    if (m_anim->state() != QPropertyAnimation::Running)
    {
        m_anim->start();
    }
}

void
GtCalculatorEntity::runLeaveAnimation()
{
    if (!m_anim)
    {
        return;
    }

    if (m_anim->state() == QPropertyAnimation::Running)
    {
        m_anim->pause();
    }

    if (m_anim->direction() == QPropertyAnimation::Forward)
    {
        m_anim->setDirection(QPropertyAnimation::Backward);
    }

    if (m_anim->state() != QPropertyAnimation::Running)
    {
        m_anim->start();
    }
    else
    {
        m_anim->resume();
    }
}

void
GtCalculatorEntity::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
//    qDebug() << "GtCalculatorEntity::hoverEnterEvent";
    runEnterAnimation();
    QGraphicsObject::hoverEnterEvent(event);
}

void
GtCalculatorEntity::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
//    qDebug() << "GtCalculatorEntity::hoverLeaveEvent";
    runLeaveAnimation();
    QGraphicsObject::hoverLeaveEvent(event);
}

void
GtCalculatorEntity::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    QRectF rect(0., 0., m_w, 20.);

    if (rect.contains(event->pos()))
    {
//        QApplication::setOverrideCursor(Qt::OpenHandCursor);
        setCursor(Qt::OpenHandCursor);
    }
    else
    {
//        QApplication::restoreOverrideCursor();
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsObject::hoverMoveEvent(event);
}

void
GtCalculatorEntity::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QRectF rect(0., 0., m_w, 20.);

    if (rect.contains(event->pos()))
    {
        setCursor(Qt::ClosedHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }

    QGraphicsObject::mousePressEvent(event);
}

void
GtCalculatorEntity::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
        .length() < QApplication::startDragDistance()) {
        return;
    }

    auto drag = new QDrag(event->widget());
    QMimeData* mime = gtDataModel->mimeDataFromObject(m_calculator);
    drag->setMimeData(mime);
    drag->exec();
    setCursor(Qt::OpenHandCursor);
}

void
GtCalculatorEntity::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QRectF rect(0., 0., m_w, 20.);

    if (rect.contains(event->pos()))
    {
        setCursor(Qt::ClosedHandCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }
}

double
GtCalculatorEntity::mod() const
{
    return m_mod;
}

void
GtCalculatorEntity::setMod(double mod)
{
    m_mod = mod;
}

const QList<GtCalculatorEntity*>&
GtCalculatorEntity::subNodes()
{
    return m_subCalcs;
}

int
GtCalculatorEntity::subNodeCount()
{
    return m_subCalcs.size();
}

int
GtCalculatorEntity::childNumber()
{
    if (!m_calculator)
    {
        return 0;
    }

    return m_calculator->childNumber();
}

bool
GtCalculatorEntity::isLeaf()
{
    return m_subCalcs.isEmpty();
}

bool
GtCalculatorEntity::isLeftMost()
{
    if (isRoot())
    {
        return true;
    }

    Q_ASSERT(m_calculator == nullptr);

    return (m_calculator->childNumber() == 0);
}

bool
GtCalculatorEntity::isRightMost()
{
    if (isRoot())
    {
        return true;
    }

    Q_ASSERT(m_calculator == nullptr);

    GtObject* parent = m_calculator->parentObject();

    if (!parent)
    {
        return false;
    }

    QList<GtObject*> olist = parent->findDirectChildren<GtObject*>();

    return (m_calculator == olist.last());
}

GtCalculatorEntity*
GtCalculatorEntity::previousSibling()
{
    Q_ASSERT(m_calculator == nullptr);

    if (!m_parentEntity || isLeftMost())
    {
        return nullptr;
    }

    return m_parentEntity->subNodes()[m_calculator->childNumber() - 1];
}

GtCalculatorEntity*
GtCalculatorEntity::nextSibling()
{
    Q_ASSERT(m_calculator == nullptr);

    if (!m_parentEntity || isRightMost())
    {
        return nullptr;
    }

    return m_parentEntity->subNodes()[m_calculator->childNumber() + 1];
}

GtCalculatorEntity*
GtCalculatorEntity::leftMostSibling()
{
    if (!m_parentEntity)
    {
        return nullptr;
    }

    if (isLeftMost())
    {
        return this;
    }

    return m_parentEntity->subNodes().first();
}

GtCalculatorEntity*
GtCalculatorEntity::leftMostChild()
{
    if (m_subCalcs.isEmpty())
    {
        return nullptr;
    }

    return m_subCalcs.first();
}

GtCalculatorEntity*
GtCalculatorEntity::rightMostChild()
{
    if (m_subCalcs.isEmpty())
    {
        return nullptr;
    }

    return m_subCalcs.last();
}

GtCalculatorEntity*
GtCalculatorEntity::parentNode()
{
    return m_parentEntity;
}

GtCalculatorEntity*
GtCalculatorEntity::leftMostLeaf()
{
    GtCalculatorEntity* tmpNode = this;

    foreach (GtCalculatorEntity* child, m_subCalcs)
    {
        GtCalculatorEntity* leftmost = child->leftMostLeaf();
        if (leftmost->normX() < tmpNode->normX())
        {
            tmpNode = leftmost;
        }
    }

    return tmpNode;
}

GtCalculatorEntity*
GtCalculatorEntity::rightMostLeaf()
{
    GtCalculatorEntity* tmpNode = this;

    foreach (GtCalculatorEntity* child, m_subCalcs)
    {
        GtCalculatorEntity* rightmost = child->rightMostLeaf();
        if (rightmost->normX() > tmpNode->normX())
        {
            tmpNode = rightmost;
        }
    }

    return tmpNode;
}

bool
GtCalculatorEntity::isRoot()
{
    return (m_calculator == nullptr);
}

void
GtCalculatorEntity::debugNode()
{
    if (isRoot())
    {
        qDebug() << " ROOT - (" << m_normX << ", " << m_normY << ")";
    }
    else
    {
        qDebug() << m_calculator->objectName() << " - (" << m_normX
                 << ", " << m_normY << ")";
    }

    foreach (GtCalculatorEntity* child, m_subCalcs)
    {
        child->debugNode();
    }
}

QString
GtCalculatorEntity::toString()
{
    if (isRoot())
    {
        return QStringLiteral("ROOT");
    }

    return m_calculator->objectName();
}

double
GtCalculatorEntity::defaultNodeWidth()
{
    return 200.;
}

double
GtCalculatorEntity::defaultNodeHeight()
{
    return 150.;
}

void
GtCalculatorEntity::updateShape()
{
    foreach (GtTaskArrowEntity* entity, m_directConnections)
    {
        entity->updateShape();
    }

    foreach (GtCalculatorEntity* calc, m_subCalcs)
    {
        calc->updateShape();
    }

    if (m_childLeft)
    {
        m_childLeft->updateShape();
    }

    if (m_childRight)
    {
        m_childRight->updateShape();
    }
}

void
GtCalculatorEntity::setToPreviewMode()
{
    foreach (GtTaskArrowEntity* entity, m_directConnections)
    {
        entity->setToPreviewMode();
    }

    if (m_childLeft)
    {
        m_childLeft->setToPreviewMode();
    }

    if (m_childRight)
    {
        m_childRight->setToPreviewMode();
    }

    foreach (GtCalculatorEntity* calc, m_subCalcs)
    {
        calc->setToPreviewMode();
    }
}

void
GtCalculatorEntity::addChild(GtObject* obj)
{
    if (obj)
    {
        auto calc = qobject_cast<GtCalculator*>(obj);

        if (!calc)
        {
            delete obj;
            return;
        }

        QModelIndex idx = gtDataModel->appendChild(calc, m_calculator);

        if (!idx.isValid())
        {
            delete obj;
            return;
        }

        qDebug() << "GtCalculatorEntity::addChild";


        if (!appendCalculator(calc))
        {
            gtError() << tr("Could not create calculator entity!");
            return;
        }

//        updateArrows();
    }
}

int
GtCalculatorEntity::normY() const
{
    return m_normY;
}

void
GtCalculatorEntity::setNormY(int normY)
{
    m_normY = normY;
}

double
GtCalculatorEntity::normX() const
{
    return m_normX;
}

void
GtCalculatorEntity::setNormX(double normX)
{
    m_normX = normX;
}

int
GtCalculatorEntity::calculateFirstRowHeight()
{
    int maxSize = 0;

    foreach (GtCalculatorEntity* calc, m_subCalcs)
    {
        int cSize = calc->boundingRect().height();
        if (cSize > maxSize)
        {
            maxSize = cSize;
        }
    }

    return maxSize;
}

void
GtCalculatorEntity::onAnimationFinished()
{
    if (!m_anim)
    {
        return;
    }

    if (m_anim->direction() == QPropertyAnimation::Forward)
    {
        return;
    }

    if (m_anim->state() == QPropertyAnimation::Stopped)
    {
        if (m_anim->currentTime() == 0)
        {
            m_anim->deleteLater();
        }
    }
}

