/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectioneditor.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCONNECTIONEDITOR_H
#define GTPROCESSCONNECTIONEDITOR_H

#include <QDialog>

#include <QPointer>

class GtTask;
class GtProcessConnectionModel;
class GtProcessConnectionView;
class GtProcessConnectionGraphicsView;
class GtProcessPropertyPortEntity;
class GtObjectMemento;

/**
 * @brief The GtProcessConnectionEditor class
 */
class GtProcessConnectionEditor : public QDialog
{
public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    explicit GtProcessConnectionEditor(GtTask* task,
                                       QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~GtProcessConnectionEditor();

    /**
     * @brief Returns connection data in form of task memento.
     * @return Connection data.
     */
    GtObjectMemento connectionData();

private:
    /// Root task
    QPointer<GtTask> m_task;

    /// Output connection model
    GtProcessConnectionModel* m_outputModel;

    /// Input connection model
    GtProcessConnectionModel* m_inputModel;

    /// Output connection view
    GtProcessConnectionView* m_outputView;

    /// Input connection view
    GtProcessConnectionView* m_inputView;

    /// Connection graphics view
    GtProcessConnectionGraphicsView* m_connectionView;

    /**
     * @brief fillData
     */
    void fillData();

};

#endif // GTPROCESSCONNECTIONEDITOR_H
