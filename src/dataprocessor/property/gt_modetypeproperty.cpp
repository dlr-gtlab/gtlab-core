/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_modetypeproperty.h"

GtModeTypeProperty::GtModeTypeProperty(const QString& name,
                                       const QString& brief) :
    GtStringProperty(QString(), name, brief, name)
{
    m_storeMemento = false;
}

