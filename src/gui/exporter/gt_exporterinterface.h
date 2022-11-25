/* GTlab - Gas Turbine laboratory
 * Source File: gt_exporterinterface.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPORTERINTERFACE_H
#define GTEXPORTERINTERFACE_H

#include "gt_gui_exports.h"

#include <QtPlugin>

/**
 * @brief The GtExporterInterface class
 */
class GT_GUI_EXPORT GtExporterInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy
     */
    virtual ~GtExporterInterface() {}

    /**
     * @brief Returns static meta objects of exporter classes.
     * @return List including meta objects of exporter classes.
     */
    virtual QList<QMetaObject> exporter() const  {
        return {};
    }

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtExporterInterface,
                    "de.dlr.gtlab.GtExporterInterface/0.1")
QT_END_NAMESPACE

#endif // GTEXPORTERINTERFACE_H
