/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_factorygroup.h"
#include "gt_object.h"
#include "gt_logging.h"

#include <algorithm>

GtObject*
GtFactoryGroup::newObject(const QString& className, GtObject* parent)
{
    GtObject* retval = 0;

    foreach (GtAbstractObjectFactory* factory, m_factories)
    {
        if (factory->knownClass(className))
        {
            retval = factory->newObject(className, parent);
            if (retval == 0)
            {
                gtError() << QObject::tr("GtFactoryGroup : Error creating ")
                          << className;
                return NULL;
            }
            break;
        }
    }

    if (retval)
    {
        retval->setFactory(this);
    }

    return retval;
}

bool
GtFactoryGroup::knownClass(const QString& className) const
{
    return std::any_of(std::begin(m_factories), std::end(m_factories),
                       [&className](const GtAbstractObjectFactory* factory) {
        return factory->knownClass(className);
    });
}

QString
GtFactoryGroup::superClassName(const QString& className) const
{
    foreach (GtAbstractObjectFactory* factory, m_factories)
    {
        QString str = factory->superClassName(className);
        if (!str.isEmpty())
        {
            return str;
        }
    }

    return QString();
}

QStringList
GtFactoryGroup::knownClasses() const
{
    QStringList retval;

    foreach (GtAbstractObjectFactory* factory, m_factories)
    {
        retval.append(factory->knownClasses());
    }

    return retval;
}

bool
GtFactoryGroup::registerClass(QMetaObject /*metaObj*/)
{
    gtWarning() << QObject::tr("register class failed!");
    return false;
}

bool
GtFactoryGroup::registerClasses(const QList<QMetaObject>& /*metaData*/)
{
    gtWarning() << QObject::tr("register class failed!");
    return false;
}

bool
GtFactoryGroup::containsDuplicates(const QList<QMetaObject>& metaData) const
{
    return std::any_of(std::begin(m_factories), std::end(m_factories),
                       [&metaData](const GtAbstractObjectFactory* factory) {
        return factory->containsDuplicates(metaData);
    });
}

bool
GtFactoryGroup::invokable(const QMetaObject& metaObj) const
{
    return std::any_of(std::begin(m_factories), std::end(m_factories),
                       [&metaObj](const GtAbstractObjectFactory* factory) {
        return factory->invokable(metaObj);
    });
}

QStringList
GtFactoryGroup::classHierarchy(const QString& className)
{
    QStringList retval;

    foreach (GtAbstractObjectFactory* factory, m_factories)
    {
        retval = factory->classHierarchy(className);

        if (!retval.isEmpty())
        {
            return retval;
        }
    }

    return retval << className;
}

bool
GtFactoryGroup::addFactory(GtAbstractObjectFactory* factory)
{
    if (m_factories.contains(factory))
    {
        return false;
    }

    m_factories.push_back(factory);

    return true;
}

