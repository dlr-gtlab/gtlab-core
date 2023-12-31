/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTWIZARD_H
#define GTPROJECTWIZARD_H

#include <QPointer>
#include "gt_wizard.h"

class GtProjectProvider;

/**
 * @brief The GtProjectWizard class
 */
class GtProjectWizard : public GtWizard
{
    Q_OBJECT

public:
    enum {
        StartPage = 0,
        SpecPage,
        ModulesPage,
        ImportPage
    };

    /**
     * @brief Constructor.
     * @param provider Project provider.
     * @param parent widget.
     */
    explicit GtProjectWizard(GtProjectProvider* provider,
                             QWidget* parent = nullptr);

private:
    /// Project provider
    QPointer<GtProjectProvider> m_provider;

};

#endif // GTPROJECTWIZARD_H
