/* GTlab - Gas Turbine laboratory
 * Source File: gt_dummyobject.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 02.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_dummyobject.h"

GtDummyObject::GtDummyObject(GtObject* parent) : GtObject(parent)
{

}

QString
GtDummyObject::origClassName() const
{
    return m_origClassName;
}

void
GtDummyObject::setOrigClassName(const QString& origClassName)
{
    m_origClassName = origClassName;
}

void
GtDummyObject::addDummyProperty(const QString& id, const QString& type,
                                bool optional, bool active, const QVariant& val)
{
    m_d_props << DummyProperty({ id, type, optional, active, val, SIMPLE });
}

void
GtDummyObject::addDummyPropertyList(const QString& id, const QString& type,
                                    bool optional, bool active,
                                    const QVariant& val)
{
    m_d_props << DummyProperty({ id, type, optional, active, val, LIST });
}

const QVector<GtDummyObject::DummyProperty>&
GtDummyObject::dummyProperties() const
{
    return m_d_props;
}
