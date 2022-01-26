/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTABSTRACTWIZARD_H
#define GTABSTRACTWIZARD_H

#include "gt_gui_exports.h"

#include <QWizard>


/**
 * @brief The GtAbstractWizard class
 */
class GT_GUI_EXPORT GtAbstractWizard : public QWizard
{
public:
    GtAbstractWizard();
};

#endif // GTABSTRACTWIZARD_H
