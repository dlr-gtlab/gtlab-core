/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelproperty.cpp
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_labelproperty.h"

GtLabelProperty::GtLabelProperty(const QString& ident,
                                 const QString& name,
                                 const QString& brief,
                                 GtObject* parent) :
    GtStringProperty(ident, name, brief, QString()),
    m_parentObj(parent)
{
}

GtObject*
GtLabelProperty::parentObject()
{
    return m_parentObj;
}
