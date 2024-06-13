/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processconnectiongraphicsview.h
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTPROCESSCONNECTIONGRAPHICSVIEW_H
#define GTPROCESSCONNECTIONGRAPHICSVIEW_H

#include <QGraphicsView>

#include <QPointer>
#include <QMap>

class GtProcessConnectionView;
class GtProcessPropertyPortEntity;
class GtProcessPropertyConnectionEntity;
class GtProcessConnectionItem;
class GtProcessConnectionScene;
class GtPropertyConnection;
class GtTask;

typedef QMap<GtProcessConnectionItem*, GtProcessPropertyPortEntity*> GtProcessPortMap;
typedef QMap<GtPropertyConnection*, GtProcessPropertyConnectionEntity*> GtProcessConnectionMap;

/**
 * @brief The GtProcessConnectionGraphicsView class
 */
class GtProcessConnectionGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    GtProcessConnectionGraphicsView(GtProcessConnectionView* outputView,
                                    GtProcessConnectionView* inputView,
                                    QWidget* parent = nullptr);

    /**
     * @brief Updates ports based on new tree view visualization.
     * @param Process connection view.
     * @param Paint event.
     */
    void updatePorts(GtProcessConnectionView* view,
                     QPaintEvent* event);

    /**
     * @brief Creates connection entities for given list of property
     * connections.
     * @param List of property connections.
     */
    void createConnections(const QList<GtPropertyConnection*>& list);

    /**
     * @brief Creates connection entity for given property connection.
     * @param Property connection.
     * @return Connection entity pointer.
     */
    GtProcessPropertyConnectionEntity* createConnection(
            GtPropertyConnection* connection);

    /**
     * @brief Returns pointer to connection scene.
     * @return Connection scene pointer.
     */
    GtProcessConnectionScene* connectionScene();

    /**
     * @brief Sets root task.
     * @param Root task.
     */
    void setRoot(GtTask* task);

    /**
     * @brief Returns root task.
     * @return Root task.
     */
    GtTask* root();

    /**
     * @brief Updates connection start and end points based on port positions.
     */
    void updateConnections();

    /**
     * @brief removeAllConnections
     * Remove all graphics entities of connections in the view and their related
     * connection objects
     */
    void removeAllConnections();

protected:
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

private:
    /// Pointer to output view widget.
    QPointer<GtProcessConnectionView> m_outputView;

    /// Pointer to input view widget.
    QPointer<GtProcessConnectionView> m_inputView;

    /// Map of output port entities corresponding to specific connection item.
    GtProcessPortMap m_outputPorts;

    /// Map of input port entities corresponding to specific connection item.
    GtProcessPortMap m_inputPorts;

    /// Map of connection entities corrsponding to specific property connection
    /// items.
//    GtProcessConnectionMap m_connections;

    /// process connection graphics scene.
    GtProcessConnectionScene* m_scene;

    /// Root task
    QPointer<GtTask> m_root;

    /**
     * @brief Updates port entity for given item, width and height.
     * @param Process connection item.
     * @param Row height.
     * @param Entity vertical position.
     * @param Alignment.
     */
    GtProcessPropertyPortEntity* updatePortEntity(GtProcessConnectionItem* item,
                                                  GtProcessPortMap& map,
                                                  int rowHeight,
                                                  int vPos,
                                                  Qt::AlignmentFlag align);

    /**
     * @brief Returns property port entity based on given object uuid and
     * property identification string from output port map. Returns null pointer
     * if no entity found.
     * @param Object UUID.
     * @param Property identification string.
     * @return Property port entity.
     */
    GtProcessPropertyPortEntity* findOutputPortEntity(const QString& uuid,
                                                      const QString& propId);

    /**
     * @brief Returns property port entity based on given object uuid and
     * property identification string from input port map. Returns null pointer
     * if no entity found.
     * @param Object UUID.
     * @param Property identification string.
     * @return Property port entity.
     */
    GtProcessPropertyPortEntity* findInputPortEntity(const QString& uuid,
                                                     const QString& propId);

    /**
     * @brief Returns property port entity based on given object uuid and
     * property identification string from given port map. Returns null pointer
     * if no entity found.
     * @param Process port map.
     * @param Object UUID.
     * @param Property identification string.
     * @return Property port entity.
     */
    GtProcessPropertyPortEntity* findPortEntityHelper(GtProcessPortMap& map,
                                                      const QString &uuid,
                                                      const QString &propId);

};

#endif // GTPROCESSCONNECTIONGRAPHICSVIEW_H
