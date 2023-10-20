/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processwizardpage.h
 *
 *  Created on: 17.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSWIZARDPAGE_H
#define GTPROCESSWIZARDPAGE_H

#include "gt_gui_exports.h"

#include <QWizardPage>
#include <QPointer>


class GtProcessWizard;
class GtAbstractProcessProvider;
class GtProject;

/**
 * @brief The GtProcessWizardPage class
 */
class GT_GUI_EXPORT GtProcessWizardPage : public QWizardPage
{
    Q_OBJECT

    friend class GtProcessWizard;

public:
    /**
     * @brief provider
     * @return
     */
    GtAbstractProcessProvider* provider();

    /**
     * @brief setProvider
     * @param prov
     */
    void setProvider(GtAbstractProcessProvider* prov);

    /**
     * @brief scope
     * @return
     */
    GtProject* scope();

    /**
     * @brief setScope
     * @param project
     */
    void setScope(GtProject* project);

    /**
     * @brief cleanupPage
     */
    void cleanupPage() override;

protected:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    GtProcessWizardPage();

private:
    /// Calculator or task provider
    QPointer<GtAbstractProcessProvider> m_provider;

    /// Scope of data representation.
    QPointer<GtProject> m_scope;

};

#endif // GTPROCESSWIZARDPAGE_H
