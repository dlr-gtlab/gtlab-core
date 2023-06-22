/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_importerinterface
 *
 *  Created on: 04.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_IMPORTERINTERFACE_H
#define GT_IMPORTERINTERFACE_H

#include "gt_gui_exports.h"

#include <QtPlugin>

class GT_GUI_EXPORT GtImporterInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy
     */
    virtual ~GtImporterInterface() {}

    /**
     * @brief Returns static meta objects of importer classes.
     * @return List including meta objects of importer classes.
     */
    virtual QList<QMetaObject> importer() const {
        return {};
    };

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtImporterInterface,
                    "de.dlr.gtlab.GtImporterInterface/0.1")
QT_END_NAMESPACE

#endif // GT_IMPORTERINTERFACE_H
