/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatoroverviewpage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOROVERVIEWPAGE_H
#define GTCALCULATOROVERVIEWPAGE_H

#include <QWizardPage>

class GtProcessWizard;
class GtProcessOverviewTree;
class GtCalculatorOverviewModel;

/**
 * @brief The GtCalculatorOverviewPage class
 */
class GtCalculatorOverviewPage : public QWizardPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    GtCalculatorOverviewPage(GtProcessWizard* parent);

    /**
     *
     */
    virtual ~GtCalculatorOverviewPage() Q_DECL_OVERRIDE;

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

    ////
    GtCalculatorOverviewModel* m_model;

private slots:
    /**
     * @brief Called after calculator entry was double clicked.
     * @param index Model index corresponding to selected calculator entry.
     */
    void onDoubleClicked(const QModelIndex& index);

};

#endif // GTCALCULATOROVERVIEWPAGE_H
