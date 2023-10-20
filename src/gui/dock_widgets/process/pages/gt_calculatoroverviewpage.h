/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatoroverviewpage.h
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATOROVERVIEWPAGE_H
#define GTCALCULATOROVERVIEWPAGE_H

#include <QWizardPage>

#include <memory>

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
    explicit GtCalculatorOverviewPage(GtProcessWizard* parent);

    /**
     *
     */
    ~GtCalculatorOverviewPage() override;

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

    ////
    std::unique_ptr<GtCalculatorOverviewModel> m_model;

private slots:
    /**
     * @brief Called after calculator entry was double clicked.
     * @param index Model index corresponding to selected calculator entry.
     */
    void onDoubleClicked(const QModelIndex& index);

};

#endif // GTCALCULATOROVERVIEWPAGE_H
