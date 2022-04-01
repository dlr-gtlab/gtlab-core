/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
