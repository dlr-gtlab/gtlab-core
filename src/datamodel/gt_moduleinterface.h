/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_MODULEINTERFACE_H
#define GT_MODULEINTERFACE_H

#include "gt_datamodel_exports.h"

#include <QtPlugin>

/**
 * @brief The GtModuleInterface class
 */
class GT_DATAMODEL_EXPORT GtModuleInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtModuleInterface() {}

    /**
     * @brief Returns current version number of module.
     * @return version number
     */
    virtual int version() = 0;

    /**
     * @brief Returns module identification string.
     * @return identification string
     */
    virtual QString ident() const = 0;

    /**
     * @brief Returns module description
     * @return description
     */
    virtual QString description() const = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtModuleInterface,
                    "de.dlr.gtlab.GtModuleInterface/1.2")
QT_END_NAMESPACE

#endif // GT_MODULEINTERFACE_H

