/* GTlab - Gas Turbine laboratory
 * Source File: gtd_graphicsscene.cpp
 * copyright 2009-2013 by DLR
 *
 *  Created on: 19.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#include "gt_graphicsscene.h"
#include <QtGui>
#include "gt_graphicsview.h"
#include "gt_grid.h"

GtGraphicsScene::GtGraphicsScene(QObject *parent) :
    QGraphicsScene(parent)
{
}

