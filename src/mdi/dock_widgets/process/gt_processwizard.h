/* GTlab - Gas Turbine laboratory
 * Source File: gt_processwizard.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSWIZARD_H
#define GTPROCESSWIZARD_H

#include "gt_mdi_exports.h"
#include <QWizard>
#include <QPointer>

class GtAbstractProcessProvider;
class GtCalculatorProvider;
class GtTaskProvider;
class GtProject;

/**
 * @brief The GtProcessWizard class
 */
class GT_MDI_EXPORT GtProcessWizard : public QWizard
{
    Q_OBJECT

public:
    enum {
        startCalculatorPage = 0,
        startTaskPage,
        calculatorSettingsPage,
        endPage,
    };

    /**
     * @brief Constructor.
     * @param Calculator provider.
     */
    GtProcessWizard(GtProject* project, GtCalculatorProvider* provider);

    /**
     * @brief Constructor
     * @param Task provider.
     */
    GtProcessWizard(GtProject* project, GtTaskProvider* provider);

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
