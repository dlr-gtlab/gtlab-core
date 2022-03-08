/* GTlab - Gas Turbine laboratory
 * Source File: gt_processpropertyportentity.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSPROPERTYPORTENTITY_H
#define GTPROCESSPROPERTYPORTENTITY_H

#include <QGraphicsEllipseItem>
#include <QPointer>

#include <memory>

class QPropertyAnimation;
class QTimer;
class GtProcessPropertyConnectionEntity;
class GtProcessConnectionItem;

/**
 * @brief The GtProcessPropertyPortEntity class
 */
class GtProcessPropertyPortEntity : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    Q_PROPERTY(double scale READ scale WRITE setScale)

public:
    enum PortTypes
    {
        INPUT_PORT = 0,
        OUTPUT_PORT
    };

    /**
     * @brief Constructor.
     * @param Horizontal position.
     * @param Vertical postion.
     * @param Width.
     * @param Height.
     */
    GtProcessPropertyPortEntity(double x, double y, double width,
                                double height,
                                GtProcessPropertyPortEntity::PortTypes typ,
                                GtProcessConnectionItem* item);

    /**
     * @brief Connects port entity to given connection entity.
     * @param Connection entity.
     * @return Whether connection could be established or not.
     */
    bool connectPort(GtProcessPropertyConnectionEntity* connection);

    /**
     * @brief Disconnects port entity from given connection entity.
     * @param Connection entity.
     * @return Whether conneciton could be disconnected or not.
     */
    bool disconnectPort(GtProcessPropertyConnectionEntity* connection);

    /**
     * @brief Returns true if port entity is connected to a connection entity,
     * otherwise returns false.
     * @return Whether port entity is connected or not.
     */
    bool isConnected();

    /**
     * @brief Returns type of port entity.
     * @return Port type.
     */
    GtProcessPropertyPortEntity::PortTypes portType();

    /**
     * @brief hidePortAnim
     * @return QPropertyAnimation
     */
    QPropertyAnimation* hidePortAnim();

    /**
     * @brief highlightPortAnim
     * @return QPropertyAnimation
     */
    QPropertyAnimation* highlightPortAnim();

    /**
     * @brief resetPortAnim
     * @return QPropertyAnimation
     */
    QPropertyAnimation* resetPortAnim();

    /**
     * @brief Returns true if port can be connected to given port, otherwise
     * return false;
     * @param Port entity.
     * @return Whether port can be connected or not.
     */
    bool canConnect(GtProcessPropertyPortEntity* port);

    /**
     * @brief Returns property value.
     * @return Property value.
     */
    QVariant propertyValue();

    /**
     * @brief Returns property className.
     * @return Property class name.
     */
    QString propertyClassName();

    /**
     * @brief Returns component UUID of parent item.
     * If item is a property empty string is returned.
     * @return Parent component UUID.
     */
    QString parentComponentUuid();

    /**
     * @brief Returns property identification string.
     * @return Property identification string.
     */
    QString propertyId();

protected:
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
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    /// Scale.
    double m_scale;

    /// Port type.
    GtProcessPropertyPortEntity::PortTypes m_type;

    /// Animation
    QPointer<QPropertyAnimation> m_anim{nullptr};

    /// Pointer to connection
    QList<GtProcessPropertyConnectionEntity*> m_connections;

    /// Signle shot timer
    std::unique_ptr<QTimer> m_timer{};

    /// Pointer to corresponding process connection item
    GtProcessConnectionItem* m_item;

    /**
     * @brief runAnimation
     * @param enter
     */
    void runAnimation(bool enter = true);

    /**
     * @brief Updates shape based on entity status.
     */
    void updateShape();

};

#endif // GTPROCESSPROPERTYPORTENTITY_H
