/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyinterface
 *
 *  Created on: 02.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROPERTYINTERFACE_H
#define GT_PROPERTYINTERFACE_H

#include "gt_gui_exports.h"

#include <QtPlugin>

class GtAbstractProperty;
class GtPropertyItem;

/**
 * @brief The GtPropertyInterface class
 */
class GT_GUI_EXPORT GtPropertyInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy.
     */
    virtual ~GtPropertyInterface() {}

    /**
     * @brief Returns static meta objects of property item classes.
     * @return list of meta objects of property item classes.
     */
    virtual QMap<const char*, QMetaObject> propertyItems() {
        return {};
    };

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtPropertyInterface,
                    "de.dlr.gtlab.GtPropertyInterface/0.1")
QT_END_NAMESPACE

#endif // GT_PROPERTYINTERFACE_H
