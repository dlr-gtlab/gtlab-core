/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorsettingspage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORSETTINGSPAGE_H
#define GTCALCULATORSETTINGSPAGE_H

#include <QWizardPage>
#include <QPointer>
#include "gt_gui_exports.h"

class GtProcessWizard;
class GtPropertyTreeView;
class GtProcessComponent;
class GtProject;

/**
 * @brief The GtCalculatorSettingsPage class
 */
class GT_GUI_EXPORT GtCalculatorSettingsPage : public QWizardPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    GtCalculatorSettingsPage(GtProject* project, GtProcessWizard* parent);

    /**
     * @brief initializePage
     */
    virtual void initializePage() override;

protected:
    /**
     * @brief showEvent
     * @param e
     */
    void showEvent(QShowEvent* e) override;

private:
    /// Process wizard
    GtProcessWizard* m_wizard;

    /// Property view
    GtPropertyTreeView* m_view;

    /// Process component
    QPointer<GtProcessComponent> m_component;

private slots:
    /**
     * @brief onComponentDataChange
     */
    void onComponentDataChange();

};

#endif // GTCALCULATORSETTINGSPAGE_H
