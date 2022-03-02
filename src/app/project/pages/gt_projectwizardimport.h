/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTWIZARDIMPORT_H
#define GTPROJECTWIZARDIMPORT_H

#include "gt_abstractprojectwizardpage.h"

/**
 * @brief The GtProjectWizardImport class
 */
class GtProjectWizardImport : public GtAbstractProjectWizardPage
{
public:
    /**
     * @brief GtProjectWizardImport
     * @param provider
     * @param parent
     */
    GtProjectWizardImport(GtProjectProvider* provider,
                          QWidget* parent = nullptr);

};

#endif // GTPROJECTWIZARDIMPORT_H
