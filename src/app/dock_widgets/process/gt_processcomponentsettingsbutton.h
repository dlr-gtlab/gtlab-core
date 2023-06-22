/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcomponentsettingsbutton.h
 *
 *  Created on: 13.10.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSCOMPONENTSETTINGSBUTTON_H
#define GT_PROCESSCOMPONENTSETTINGSBUTTON_H

#include <QPushButton>

#include <QPointer>

#include "gt_processcomponent.h"

/**
 * @brief The GtProcessComponentSettingsButton class
 */
class GtProcessComponentSettingsButton : public QPushButton
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GtProcessComponentSettingsButton();

    /**
     * @brief Sets process component.
     * @param Process component pointer.
     */
    void setProcessComponent(GtProcessComponent* pc);

    /**
     * @brief Returns true if process component has a custom wizard.
     * @return Custom wizard indicator.
     */
    bool hasCustomWizard();

private:
    /// Pointer to process component object
    QPointer<GtProcessComponent> m_pc;

    /// Pointer to root task of process component object
    QPointer<GtTask> m_task;

private slots:
    /**
     * @brief Updates button state based on process component information.
     */
    void updateState();

    /**
     * @brief Opens process component wizard for selected process component
     * object.
     */
    void openProcessComponentWizard();

};

#endif // GT_PROCESSCOMPONENTSETTINGSBUTTON_H
