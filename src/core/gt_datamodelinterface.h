/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DATAMODELINTERFACE_H
#define GT_DATAMODELINTERFACE_H

#include "gt_core_exports.h"

#include "gt_globals.h"

#include <QtPlugin>

/**
 * @brief The GtDatamodelInterface class
 */
class GT_CORE_EXPORT GtDatamodelInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtDatamodelInterface() {}

    /**
     * @brief Returns static meta objects of datamodel package.
     * @return package meta object
     */
    virtual QMetaObject package() = 0;

    /**
     * @brief Returns static meta objects of datamodel classes.
     * @return list including meta objects
     */
    virtual QList<QMetaObject> data() {
        return {};
    }

    /**
     * @brief Returns true if module is a stand alone module with own
     * data model structure. Otherwise module only extends the overall
     * application with additional functionalities like classes, calculators
     * or graphical user interfaces.
     * @return Stand alone indicator.
     */
    virtual bool standAlone() = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtDatamodelInterface,
                    "de.dlr.gtlab.GtDatamodelInterface/2.1")
QT_END_NAMESPACE

GT_OLD_INTERFACES(GtDatamodelInterface,
                  "de.dlr.gtlab.GtDatamodelInterface/0.1")

#endif // GT_DATAMODELINTERFACE_H

