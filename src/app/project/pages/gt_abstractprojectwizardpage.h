/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROJECTWIZARDPAGE_H
#define GTABSTRACTPROJECTWIZARDPAGE_H

#include <QWizardPage>
#include <QPointer>

class GtProjectProvider;

/**
 * @brief The GtAbstractProjectWizardPage class
 */
class GtAbstractProjectWizardPage : public QWizardPage
{
public:
    /**
     * @brief GtAbstractProjectWizardPage
     * @param parent
     */
    explicit GtAbstractProjectWizardPage(GtProjectProvider* provider,
                                         QWidget* parent = nullptr);

protected:
    /**
     * @brief isValid
     */
    bool isValid();

    /// Project provider
    QPointer<GtProjectProvider> m_provider;

};

#endif // GTABSTRACTPROJECTWIZARDPAGE_H
