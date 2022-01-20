/* GTlab - Gas Turbine laboratory
 * Source File: gt_networkinterface
 * copyright 2009-2017 by DLR
 *
 *  Created on: 25.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_NETWORKINTERFACE_H
#define GT_NETWORKINTERFACE_H

#include "gt_core_exports.h"

#include <QtPlugin>

/**
 * @brief The GtNetworkInterface class
 */
class GT_CORE_EXPORT GtNetworkInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy
     */
    virtual ~GtNetworkInterface() {}

    /**
     * @brief Returns identification string of access point.
     * @return Identification string of access point.
     */
    virtual QString accessId() = 0;

    /**
     * @brief Returns meta object of GtAbstractAccessDataConnection class.
     * @return Meta object of GtAbstractAccessDataConnection class.
     */
    virtual QMetaObject accessConnection() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtNetworkInterface,
                    "de.dlr.gtlab.GtNetworkInterface/0.1")
QT_END_NAMESPACE

#endif // GT_NETWORKINTERFACE_H
