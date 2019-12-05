/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskoverviewpage.h
 * copyright 2009-2017 by DLR
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
    GtTaskOverviewPage(GtProcessWizard* parent);

    /**
     * @brief validatePage
     * @return
     */
    virtual bool validatePage() Q_DECL_OVERRIDE;

    /**
     * @brief isComplete
     * @return
     */
    virtual bool isComplete() const Q_DECL_OVERRIDE;

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
