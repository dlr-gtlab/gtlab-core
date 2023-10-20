/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_label.cpp
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_label.h"

GtLabel::GtLabel(const QString& id)
{
    setObjectName(id);

    setFlag(GtObject::UserRenamable, true);
}
