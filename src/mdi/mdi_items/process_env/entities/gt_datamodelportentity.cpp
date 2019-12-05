/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_datamodelportentity.h"

GtDataModelPortEntity::GtDataModelPortEntity(QGraphicsItem* parent) :
    GtPropertyPortEntity(parent)
{
    m_fillColor = QColor(235, 145, 40);
}

