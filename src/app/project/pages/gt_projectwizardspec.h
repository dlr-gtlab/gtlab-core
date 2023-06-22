/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTWIZARDSPEC_H
#define GTPROJECTWIZARDSPEC_H

#include "gt_abstractprojectwizardpage.h"

class GtProjectSpecWidget;

/**
 * @brief The GtProjectWizardSpec class
 */
class GtProjectWizardSpec : public GtAbstractProjectWizardPage
{
    Q_OBJECT

public:
    /**
     * @brief GtProjectWizardSpec
     * @param provider
     * @param parent
     */
    explicit GtProjectWizardSpec(GtProjectProvider* provider,
                                 QWidget* parent = nullptr);

    /**
     * @brief isComplete
     * @return
     */
    bool isComplete() const override;

    /**
     * @brief validatePage
     * @return
     */
    bool validatePage() override;

private:
    /// Project specification widget.
    GtProjectSpecWidget* m_specWidget;

};

#endif // GTPROJECTWIZARDSPEC_H
