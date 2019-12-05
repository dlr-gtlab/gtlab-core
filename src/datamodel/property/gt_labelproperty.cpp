/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelproperty.cpp
 * copyright 2009-2016 by DLR
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
