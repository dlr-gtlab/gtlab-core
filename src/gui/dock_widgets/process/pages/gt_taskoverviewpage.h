/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_taskoverviewpage.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_TASKOVERVIEWPAGE_H
#define GT_TASKOVERVIEWPAGE_H

#include <QWizardPage>

class GtProcessWizard;
class GtProcessOverviewTree;

/**
 * @brief The GtTaskOverviewPage class
 */
class GtTaskOverviewPage : public QWizardPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Pointer to process wizard.
     */
    explicit GtTaskOverviewPage(GtProcessWizard* parent);

    /**
     * @brief validatePage
     * @return
     */
    bool validatePage() override;

    /**
     * @brief isComplete
     * @return
     */
    bool isComplete() const override;

private:
    /// Tree view
    GtProcessOverviewTree* m_view;

    /// Process wizard
    GtProcessWizard* m_wizard;

private slots:
    /**
     * @brief Called after task entry was double clicked.
     * @param index Model index corresponding to selected task entry.
     */
    void onDoubleClicked(const QModelIndex& index);

};

#endif // GT_TASKOVERVIEWPAGE_H
