/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORENTITY_H
#define GTCALCULATORENTITY_H

#include <QGraphicsObject>
#include <QPointer>

class QPropertyAnimation;
class GtCalculator;
class GtTask;
class GtTaskArrowEntity;
class GtObject;

class GtCalculatorEntity : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief GtCalculatorEntity
     * @param task
     * @param parent
     */
    GtCalculatorEntity(GtTask* task,
                       QGraphicsItem* parent);

    /**
     * @brief GtCalculatorEntity
     * @param calc
     * @param parent
     */
    GtCalculatorEntity(GtCalculator* calc,
                       GtCalculatorEntity* parent);

    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /**
    * @brief boundingRect
    * @return
    */
    QRectF boundingRect() const override;

    /**
     * @brief absoluteHeight
     * @return
     */
    int absoluteHeight(int sum);

    /**
     * @brief alignChildren
     */
    void alignChildren(int maxHeight);

//    /**
//     * @brief calculateInitialX
//     */
//    void calculateInitialX();

    /**
     * @brief initializeNode
     * @param depth
     */
//    void initializeNode(int depth = 0);

    /**
     * @brief calculatePosition
     */
    void calculatePosition();

    /**
     * @brief normX
     * @return
     */
    double normX() const;

    /**
     * @brief setNormX
     * @param normX
     */
    void setNormX(double normX);

    /**
     * @brief normY
     * @return
     */
    int normY() const;

    /**
     * @brief setNormY
     * @param normY
     */
    void setNormY(int normY);

    /**
     * @brief mod
     * @return
     */
    double mod() const;

    /**
     * @brief setMod
     * @param mod
     */
    void setMod(double mod);

    /**
     * @brief subNodes
     * @return
     */
    const QList<GtCalculatorEntity*>& subNodes();

    /**
     * @brief subNodeCount
     * @return
     */
    int subNodeCount();

    /**
     * @brief childNumber
     * @return
     */
    int childNumber();

    /**
     * @brief isLeaf
     * @return
     */
    bool isLeaf();

    /**
     * @brief isLeftMost
     * @return
     */
    bool isLeftMost();

    /**
     * @brief isRightMost
     * @return
     */
    bool isRightMost();

    /**
     * @brief previousSibling
     * @return
     */
    GtCalculatorEntity* previousSibling();

    /**
     * @brief nextSibling
     * @return
     */
    GtCalculatorEntity* nextSibling();

    /**
     * @brief previousSibling
     * @return
     */
    GtCalculatorEntity* leftMostSibling();

    /**
     * @brief leftMostChild
     * @return
     */
    GtCalculatorEntity* leftMostChild();

    /**
     * @brief rightMostChild
     * @return
     */
    GtCalculatorEntity* rightMostChild();

    /**
     * @brief parentNode
     * @return
     */
    GtCalculatorEntity* parentNode();

    /**
     * @brief leftMostLeaf
     * @return
     */
    GtCalculatorEntity* leftMostLeaf();

    /**
     * @brief rightMostLeaf
     * @return
     */
    GtCalculatorEntity* rightMostLeaf();

    /**
     * @brief isRoot
     * @return
     */
    bool isRoot();

    /**
     * @brief debugNode
     */
    void debugNode();

    /**
     * @brief toString
     * @return
     */
    QString toString();

    /**
     * @brief defaultNodeWidth
     * @return
     */
    static double defaultNodeWidth();

    /**
     * @brief defaultNodeHeight
     * @return
     */
    static double defaultNodeHeight();

    /**
     * @brief updateShape
     */
    void updateShape();

    /**
     * @brief setToPreviewMode
     */
    void setToPreviewMode();

    /**
     * @brief addChild
     * @param obj
     */
    void addChild(GtObject* obj);

protected:
    /// width
    int m_w;

    /// height
    int m_h;

    /// calculator
    QPointer<GtCalculator> m_calculator;

    /// icon
    QPixmap m_icon;

    /**
     * @brief populateCalculator
     */
    void populateCalculator();

    /**
     * @brief populateCalculator
     * @param calcs
     */
    void populateCalculator(const QList<GtCalculator*>& calcs);

    /**
     * @brief insertCalculator
     * @param calc
     */
    GtCalculatorEntity* insertCalculator(GtCalculator* calc, int idx);

    /**
     * @brief appendCalculator
     * @param calc
     * @return
     */
    GtCalculatorEntity* appendCalculator(GtCalculator* calc);

    /**
     * @brief updateArrows
     */
    void updateArrows();

    /**
     * @brief calculateSize
     */
    void calculateSize();

    /**
     * @brief runEnterAnimation
     */
    void runEnterAnimation();

    /**
     * @brief runLeaveAnimation
     */
    void runLeaveAnimation();

    /**
     * @brief hoverEnterEvent
     * @param event
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief hoverLeaveEvent
     * @param event
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief hoverMoveEvent
     * @param event
     */
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    template <class T>
    QList<T> findItems()
    {
        QList<T> retval;

        foreach (QGraphicsItem* item, childItems())
        {
            T obj = dynamic_cast<T>(item);

            if (obj)
            {
                retval.append(obj);
            }
        }

        return retval;
    }

private:
    /// Normalized horizontal position within the tree structure
    double m_normX;

    /// Normalized vertical position within the tree structure
    int m_normY;

    /// mod
    double m_mod;

    ///
    QPointer<QPropertyAnimation> m_anim;

    ///
    QList<GtCalculatorEntity*> m_subCalcs;

    ///
    QList<GtTaskArrowEntity*> m_directConnections;

    ///
    QPointer<GtTaskArrowEntity> m_childLeft;

    ///
    QPointer<GtTaskArrowEntity> m_childRight;

    /**
     * @brief m_parentEntity
     */
    GtCalculatorEntity* m_parentEntity;

    /**
     * @brief calculateHeight
     * @return
     */
    int calculateFirstRowHeight();

    /**
     * @brief init
     */
    void init();

private slots:
    /**
     * @brief onAnimationFinished
     */
    void onAnimationFinished();

signals:
    /**
     * @brief switchTo
     * @param entity
     */
    void switchTo(GtCalculatorEntity* entity);

};

#endif // GTCALCULATORENTITY_H
