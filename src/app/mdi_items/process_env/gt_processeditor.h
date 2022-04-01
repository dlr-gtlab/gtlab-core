/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 18.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSEDITOR_H
#define GTPROCESSEDITOR_H

#include <QPointer>

#include "gt_mdiitem.h"

class QPushButton;
class QGraphicsItem;
class GtProcessEditorView;
class GtProcessEditorScene;
class GtTask;
class GtCalculatorEntity;

/**
 * @brief The GtProcessEditor class
 */
class GtProcessEditor : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessEditor
     */
    Q_INVOKABLE GtProcessEditor();

    /**
     * @brief setData
     * @param obj
     */
    void setData(GtObject* obj) override;

    /**
     * @brief objectChangedEvent
     * @param obj
     */
    void objectChangedEvent(GtObject* obj) override;

    /**
     * @brief onStateChangedEvent
     * @param id
     * @param value
     */
//    virtual void onStateChangedEvent(const GtMdiState* state) override;

protected:
    /**
     * @brief initialized
     */
    void initialized() override;

private:
    /// Graphics view
    GtProcessEditorView* m_view;

    /// Graphics scene
    GtProcessEditorScene* m_scene;

    /// Show legend button
    QPushButton* m_showLegendButton;

    /// show grid indicator
//    GtMdiState* m_showLegendState;

    /// Show properties button
    QPushButton* m_showPropertiesButton;

private slots:
    /**
     * @brief toggleLegend
     * @param val
     */
    void toggleLegend(bool val);

    /**
     * @brief goBack
     */
    void goBack();

    /**
     * @brief goForward
     */
    void goForward();

};

#endif // GTPROCESSEDITOR_H
