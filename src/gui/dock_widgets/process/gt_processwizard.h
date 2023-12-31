/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processwizard.h
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSWIZARD_H
#define GTPROCESSWIZARD_H

#include "gt_gui_exports.h"
#include <QPointer>
#include "gt_wizard.h"

class GtAbstractProcessProvider;
class GtCalculatorProvider;
class GtTaskProvider;
class GtProject;

/**
 * @brief The GtProcessWizard class
 */
class GT_GUI_EXPORT GtProcessWizard : public GtWizard
{
    Q_OBJECT

public:
    enum {
        startCalculatorPage = 0,
        startTaskPage,
        calculatorSettingsPage,
        endPage
    };

    /**
     * @brief Constructor.
     * @param project to add the calculator to
     * @param Calculator provider.
     */
    GtProcessWizard(GtProject* project, GtCalculatorProvider* provider,
                    QWidget* parent = nullptr);

    /**
     * @brief Constructor
     * @param project to add the task to
     * @param Task provider.
     */
    GtProcessWizard(GtProject* project, GtTaskProvider* provider,
                    QWidget* parent = nullptr);

    /**
     * @brief Returns process component provider.
     * @return Process component provider.
     */
    GtAbstractProcessProvider* provider();

    /**
     * @brief scope
     * @return
     */
    GtProject* scope();

    /**
     * @brief setEnableExecutionSettings
     * @param enabled
     */
    void setEnableExecutionSettings(bool enabled);

private:
    /// Provider
    QPointer<GtAbstractProcessProvider> m_provider;

    /// Scope of data representation.
    QPointer<GtProject> m_scope;

    /// Execution settings button
    QPushButton* m_execSettingBtn;

private slots:
    /**
     * @brief Opens execution settings dialog.
     */
    void executionSettings();

};

#endif // GTPROCESSWIZARD_H
