/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTWIZARDMODULES_H
#define GTPROJECTWIZARDMODULES_H

#include "gt_abstractprojectwizardpage.h"

class GtCheckableStringListModel;

/**
 * @brief The GtProjectWizardModules class
 */
class GtProjectWizardModules : public GtAbstractProjectWizardPage
{
public:
    /**
     * @brief GtProjectWizardModules
     * @param parent
     */
    explicit GtProjectWizardModules(GtProjectProvider* provider,
                                    QWidget* parent = nullptr);

    /**
     * @brief ~GtProjectWizardModules
     */
    ~GtProjectWizardModules() override;

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
    GtCheckableStringListModel* m_model;

};

#endif // GTPROJECTWIZARDMODULES_H
