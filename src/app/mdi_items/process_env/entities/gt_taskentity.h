/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKENTITY_H
#define GTTASKENTITY_H

#include <QGraphicsObject>
#include <QPointer>

class GtTask;
class GtCalculator;
class GtCalculatorEntity;

class GtTaskEntity : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit GtTaskEntity(GtTask* task, QGraphicsItem* parent = nullptr);

    virtual void paint(QPainter* painter,
                       const QStyleOptionGraphicsItem* option,
                       QWidget* widget = nullptr) override;

    /**
    * @brief boundingRect
    * @return
    */
    virtual QRectF boundingRect() const override;

    /**
     * @brief populateTask
     */
    void populateTask();

    /**
     * @brief rootNode
     * @return
     */
    GtCalculatorEntity* rootNode();

protected:
    /**
     * @brief calculateSize
     */
    void calculateSize();

    /**
     * @brief calculateWidth
     * @return
     */
    int calculateWidth();

    /**
     * @brief calculateHeight
     * @return
     */
    int calculateHeight();

    /**
     * @brief calculateHeight
     * @return
     */
    int calculateFirstRowHeight();

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

    template <class T>
    QList<T> findAllItems()
    {
        QList<T> retval;

        foreach (QGraphicsItem* item, allChildItems())
        {
            T obj = dynamic_cast<T>(item);

            if (obj)
            {
                retval.append(obj);
            }
        }

        return retval;
    }

    /**
     * @brief allChildItems
     * @param item
     * @return
     */
    QList<QGraphicsItem*> allChildItems(QGraphicsItem* item = nullptr);

    /**
     * @brief dragEnterEvent
     * @param event
     */
    virtual void dragEnterEvent(
            QGraphicsSceneDragDropEvent* event) override;


private:
    /// width
    int m_w;

    /// height
    int m_h;

    /// Default node size
    double m_nodeSize;

    ///
    double m_siblingDistance;

    ///
    double m_treeDistance;

    QVector<QLineF> m_lines;

    /// task
    QPointer<GtTask> m_task;

    ///
    QList<GtCalculatorEntity*> m_subCalcs;

    /// Root entity (invisible)
    GtCalculatorEntity* m_rootEntity;

    /**
     * @brief Initializes nodes for given calculator entity and depth.
     * @param node - Calculator entity
     * @param depth - Depth of entity
     */
    void initializeNodes(GtCalculatorEntity* node, int depth = 0);

    /**
     * @brief Calculates initial node norms.
     * @param node - Calculator entity
     */
    void calculateInitialNodeNorms(GtCalculatorEntity* node);

    /**
     * @brief calculateFinalPositions
     * @param node
     * @param modSum
     */
    void calculateFinalPositions(GtCalculatorEntity* node, double modSum = 0.);

    /**
     * @brief checkForConflicts
     * @param node
     */
    void checkForConflicts(GtCalculatorEntity* node);

    /**
     * @brief leftContour
     * @param node
     * @param modSum
     * @param nodeContour
     */
    void leftContour(GtCalculatorEntity* node, double modSum,
                     QMap<int, double>& nodeContour);

    /**
     * @brief rightContour
     * @param node
     * @param modSum
     * @param nodeContour
     */
    void rightContour(GtCalculatorEntity* node, double modSum,
                      QMap<int, double>& nodeContour);

    /**
     * @brief centerNodesBetween
     * @param leftNode
     * @param rightNode
     */
    void centerNodesBetween(GtCalculatorEntity* leftNode,
                            GtCalculatorEntity* rightNode);

    /**
     * @brief maxContourValue
     * @param map
     * @return
     */
    double maxContourValue(const QMap<int, double>& map);

signals:
    /**
     * @brief switchTo
     * @param entity
     */
    void switchTo(GtCalculatorEntity* entity);

};

#endif // GTTASKENTITY_H
