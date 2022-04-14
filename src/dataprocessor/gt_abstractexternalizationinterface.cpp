/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractexternalizationinterface.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_abstractexternalizationinterface.h"

#include "gt_externalizationmanager.h"

GtAbstractExternalizationInterface::GtAbstractExternalizationInterface()
{
    gtExternalizationManager->registerExernalizationInterface(this);
}
