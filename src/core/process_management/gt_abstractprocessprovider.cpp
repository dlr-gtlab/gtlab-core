/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractprocessprovider.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_processfactory.h"
#include "gt_processcomponent.h"
#include "gt_objectmemento.h"

#include "gt_abstractprocessprovider.h"

GtAbstractProcessProvider::GtAbstractProcessProvider() :
    m_component(Q_NULLPTR)
{

}

GtObjectMemento
GtAbstractProcessProvider::componentData()
{
    if (m_component == Q_NULLPTR)
    {
        return GtObjectMemento();
    }

    return m_component->toMemento();
}

bool
GtAbstractProcessProvider::setComponentData(const GtObjectMemento& memento)
{
    if (m_component == Q_NULLPTR)
    {
        return false;
    }

    m_component->fromMemento(memento);

    return true;
}

bool
GtAbstractProcessProvider::initComponent(const QString& classname)
{
    if (m_component != Q_NULLPTR)
    {
        if (m_component->metaObject()->className() == classname)
        {
            return true;
        }

        delete m_component;
    }

    GtObject* obj = gtProcessFactory->newObject(classname);

    if (obj == Q_NULLPTR)
    {
        gtDebug() << "could not create new object" <<
                     " (GtAbstractProcessProvider::initComponent)";
        gtDebug() << "  |-> " << classname;
        return false;
    }

    m_component = qobject_cast<GtProcessComponent*>(obj);

    if (m_component == Q_NULLPTR)
    {
        gtDebug() << "could not cast new object" <<
                     " (GtAbstractProcessProvider::initComponent)";
        delete obj;
        return false;
    }

    m_component->setParent(this);

    return true;
}

bool
GtAbstractProcessProvider::initComponent(const QMetaObject* metaObj)
{
    return initComponent(metaObj->className());
}

bool
GtAbstractProcessProvider::initComponent(const QMetaObject& metaObj)
{
    return initComponent(metaObj.className());
}

bool
GtAbstractProcessProvider::componentInitialized()
{
    return (m_component != Q_NULLPTR);
}

QString
GtAbstractProcessProvider::componentClassName()
{
    if (m_component == Q_NULLPTR)
    {
        return QString();
    }

    return m_component->metaObject()->className();
}
