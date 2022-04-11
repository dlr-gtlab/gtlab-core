/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTWIZARDSTART_H
#define GTPROJECTWIZARDSTART_H

#include "gt_abstractprojectwizardpage.h"

/**
 * @brief The GtProjectWizardStart class
 */
class GtProjectWizardStart : public GtAbstractProjectWizardPage
{
public:
    /**
     * @brief GtProjectWizardStart
     * @param provider
     * @param parent
     */
    explicit GtProjectWizardStart(GtProjectProvider* provider,
                                  QWidget* parent = nullptr);

};

#endif // GTPROJECTWIZARDSTART_H
