/* GTlab - Gas Turbine laboratory
 * Source File: gt_initmoduleinterface.h
 * copyright 2009-2019 by DLR
 *
 *  Created on: 16.08.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_INITMODULEINTERFACE_H
#define GT_INITMODULEINTERFACE_H

#include "gt_datamodel_exports.h"

#include <QtPlugin>

/**
 * @brief The GtInitModuleInterface class
 */
class GT_DATAMODEL_EXPORT GtInitModuleInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtInitModuleInterface() {}

    /**
     * @brief Initializes module. Called on application startup.
     */
    virtual void init() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtInitModuleInterface,
                    "de.dlr.gtlab.GtInitModuleInterface/1.0")

#endif // GT_INITMODULEINTERFACE_H
