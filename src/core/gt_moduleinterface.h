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

#include "gt_core_exports.h"

#include <QtPlugin>

#include "gt_versionnumber.h"

class QDomElement;

typedef bool (*ConverterFunction)(QDomElement&, const QString&);

void GT_CORE_EXPORT register_converter(const QString& modId,
                                            GtVersionNumber target,
                                            ConverterFunction func);


/**
 * @brief The GtModuleInterface class
 */
class GT_CORE_EXPORT GtModuleInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtModuleInterface() {}

    /**
     * @brief Returns current version number of module.
     * @return version number
     */
    virtual GtVersionNumber version() = 0;

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

    /**
     * @brief Initializes module. Called on application startup.
     */
    virtual void init(){};

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtModuleInterface,
                    "de.dlr.gtlab.GtModuleInterface/1.7")
QT_END_NAMESPACE

#endif // GT_MODULEINTERFACE_H

