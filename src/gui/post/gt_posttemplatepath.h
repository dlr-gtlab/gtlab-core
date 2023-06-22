/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_posttemplatepath.h
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPOSTTEMPLATEPATH_H
#define GTPOSTTEMPLATEPATH_H

#include "gt_object.h"
#include "gt_gui_exports.h"

/**
 * @brief The GtPostTemplatePath class
 */
class GT_GUI_EXPORT GtPostTemplatePath : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief GtPostTemplatePath
     */
    explicit GtPostTemplatePath(const QString& path);

    /**
     * @brief path
     * @return
     */
    QString path();

};

#endif // GTPOSTTEMPLATEPATH_H
