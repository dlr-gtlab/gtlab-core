/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_graphicsscene.cpp
 *
 *  Created on: 19.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#include "gt_graphicsscene.h"
#include <QtGui>
#include "gt_graphicsview.h"
#include "gt_grid.h"

GtGraphicsScene::GtGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
    GT_REMOVAL_GUARD(2, 2, "remove the entire class");
}

