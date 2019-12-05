/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DATAMODELINTERFACE_H
#define GT_DATAMODELINTERFACE_H

#include "gt_datamodel_exports.h"

#include <QtPlugin>

/**
 * @brief The GtDatamodelInterface class
 */
class GT_DATAMODEL_EXPORT GtDatamodelInterface
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
    virtual QList<QMetaObject> data() = 0;

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
                    "de.dlr.gtlab.GtDatamodelInterface/0.1")
QT_END_NAMESPACE

#endif // GT_DATAMODELINTERFACE_H

