/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSEDITORSCENE_H
#define GTPROCESSEDITORSCENE_H

#include <QPointer>
#include <QPersistentModelIndex>

#include "gt_graphicsscene.h"

class GtTask;
class GtCalculatorEntity;
class GtDataModel;

/**
 * @brief The GtProcessEditorScene class
 */
class GtProcessEditorScene : public GtGraphicsScene
{
    Q_OBJECT

    enum Mode
    {
        M_DEFAULT,
        M_PREVIEW
    };

public:
    /**
     * @brief GtProcessEditorScene
     * @param parent
     */
    GtProcessEditorScene(QObject* parent = 0);

    /**
     * @brief setRoot
     * @param task
     */
    void setRoot(GtTask* task);

public slots:
    /**
     * @brief setPreviewMode
     */
    void setPreviewMode();

protected:
    /**
     * @brief dragEnterEvent
     * @param event
     */
    virtual void dragEnterEvent(
            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

//    /**
//     * @brief mouseReleaseEvent
//     * @param mouseEvent
//     */
//    virtual void mouseReleaseEvent(
//            QGraphicsSceneMouseEvent* mouseEvent) Q_DECL_OVERRIDE;

    /**
     * @brief dragLeaveEvent
     * @param event
     */
    virtual void dragLeaveEvent(
            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

//    /**
//     * @brief dragMoveEvent
//     * @param event
//     */
//    virtual void dragMoveEvent(
//            QGraphicsSceneDragDropEvent* event) Q_DECL_OVERRIDE;

    /**
     * @brief populate
     * @param task
     */
    void populate(GtTask* task);

    /**
     * @brief switchTo
     * @param task
     */
    void switchTo(GtTask* task);

    /**
     * @brief switchMode
     * @param mode
     */
    void switchMode(GtProcessEditorScene::Mode mode);

    template <class T>
    QList<T> findItems()
    {
        QList<T> retval;

        foreach (QGraphicsItem* item, items())
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
    /// Root task
    QPointer<GtTask> m_root;

    /// Current task
    QPointer<GtTask> m_current;

    /// Preview task
    QPointer<GtTask> m_temp;

    /// model
    QPointer<GtDataModel> m_model;

    /// Process Data index
    QPersistentModelIndex m_rootIndex;

    /// Root index
//    QPersistentModelIndex m_rootIndex;

    /// Current editor mode
    GtProcessEditorScene::Mode m_mode;

    /**
     * @brief isMemberOf
     * @param child
     * @param parent
     * @return
     */
    bool isMemberOf(const QModelIndex& child, const QModelIndex& parent);

private slots:
    /**
     * @brief onSwitchItem
     * @param entity
     */
    void onSwitchItem(GtCalculatorEntity* entity);

    /**
     * @brief onRowsAboutToBeInserted
     * @param parent
     * @param start
     * @param end
     */
    void onRowsRemoved(const QModelIndex& parent, int start, int end);

};

#endif // GTPROCESSEDITORSCENE_H
