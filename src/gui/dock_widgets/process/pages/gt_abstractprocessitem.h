/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractprocessitem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROCESSITEM_H
#define GTABSTRACTPROCESSITEM_H

#include "gt_object.h"

/**
 * @brief The GtAbstractProcessItem class
 */
class GtAbstractProcessItem : public GtObject
{
    Q_OBJECT

protected:
    /**
     * @brief Constructor.
     * @param Parent.
     */
    explicit GtAbstractProcessItem(QObject* /*parent*/ = nullptr) {}

};

#endif // GTABSTRACTPROCESSITEM_H
