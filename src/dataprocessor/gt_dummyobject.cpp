/* GTlab - Gas Turbine laboratory
 * Source File: gt_dummyobject.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 02.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_dummyobject.h"

#include "gt_objectmemento.h"

struct GtDummyObject::Impl
{

    /// original class name
    QString origClassName;

    QVector<GtObjectMemento::PropertyData> properties;
    QVector<GtObjectMemento::PropertyData> propertyContainers;
};

GtDummyObject::GtDummyObject(GtObject* parent) :
    GtObject(parent),
    pimpl(std::make_unique<Impl>())
{
}

GtDummyObject::~GtDummyObject() = default;

QString
GtDummyObject::origClassName() const
{
    return pimpl->origClassName;
}

void
GtDummyObject::importMemento(const GtObjectMemento &memento)
{
    pimpl->origClassName = memento.className();
    pimpl->properties = memento.properties;
    pimpl->propertyContainers = memento.dynamicSizeProperties;
}

void
GtDummyObject::exportToMemento(GtObjectMemento &memento) const
{
    memento.setClassName(pimpl->origClassName);
    memento.properties = pimpl->properties;
    memento.dynamicSizeProperties = pimpl->propertyContainers;
}


