/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processqueuewidget.h
 *
 *  Created on: 13.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTPROCESSQUEUEWIDGET_H
#define GTPROCESSQUEUEWIDGET_H

#include <QWidget>

class QPushButton;
class QListView;
class GtProcessQueueModel;

/**
 * @brief The GtProcessQueueWidget class
 */
class GtProcessQueueWidget : public QWidget
{
public:
    /**
     * @brief GtProcessQueueWidget
     * @param model
     */
    explicit GtProcessQueueWidget(GtProcessQueueModel* model);

    /**
     *  Destructor: deletes model
     */
    ~GtProcessQueueWidget() override;

private:
    /// Up Button
    QPushButton* m_upButton;

    /// Down Button
    QPushButton* m_downButton;

    /// Remove Button
    QPushButton* m_removeButton;

    /// View
    QListView* m_view;

    /// Model
    GtProcessQueueModel* m_model;

private slots:
    /**
     * @brief Action when up button is toggled
     */
    void onMoveUp();

    /**
     * @brief Action when down button is toggled
     */
    void onMoveDown();

    /**
     * @brief Action when remove button is toggled
     */
    void onRemove();

    /**
     * @brief Update button states if selected task has changed
     */
    void onSelectionChanged();

    /**
     * @brief Disable all buttons and clear selection
     */
    void clearSelection();

    /**
     * @brief Custom context menu implementation
     */
    void onContextMenuRequest(const QPoint &pos);
};

#endif // GTPROCESSQUEUEWIDGET_H
