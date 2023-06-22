/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractexternalizationinterface.cpp
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
