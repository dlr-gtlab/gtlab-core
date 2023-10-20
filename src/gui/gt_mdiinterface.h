/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMDIINTERFACE_H
#define GTMDIINTERFACE_H

#include "gt_gui_exports.h"

#include <QtPlugin>

#include "gt_objectui.h"

/**
 * @brief The GtMdiInterface class
 */
class GT_GUI_EXPORT GtMdiInterface
{
public:
    /** Virtual destructor to keep compilers happy */
    virtual ~GtMdiInterface() {}

    /**
     * @brief Returns static meta objects of mdi item classes.
     * @return list including meta objects
     */
    virtual QList<QMetaObject> mdiItems() {
        return {};
    };

    /**
     * @brief Returns static meta objects of dockwidget classes.
     * @return list including meta objects
     */
    virtual QList<QMetaObject> dockWidgets() {
        return {};
    };

    /**
     * @brief uiItems
     * @return
     */
    virtual QMap<const char*, QMetaObject> uiItems() {
        return {};
    };

    /**
     * @brief postItems
     * @return
     */
    virtual QList<QMetaObject> postItems() {
        return {};
    };

    /**
     * @brief postPlots
     * @return
     */
    virtual QList<QMetaObject> postPlots() {
        return {};
    };

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtMdiInterface,
                    "de.dlr.gtlab.GtMdiInterface/0.1")
QT_END_NAMESPACE

#endif // GTMDIINTERFACE_H
