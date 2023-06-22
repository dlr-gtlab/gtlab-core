/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyfactory.cpp
 *
 *  Created on: 08.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_boolproperty.h"
#include "gt_doubleproperty.h"
#include "gt_intproperty.h"
#include "gt_stringproperty.h"

#include "gt_propertyfactory.h"

template <class T>
void GtPropertyFactory::registerQObjectClass(){

    auto factory = [this](const QString& id, const QString& name) -> GtAbstractProperty* {

        const QMetaObject& metaObj = T::staticMetaObject;

        std::unique_ptr<QObject> obj(metaObj.newInstance(
                                        Q_ARG(QString, id),
                                        Q_ARG(QString, name)));


        GtAbstractProperty* retval = nullptr;
        if (obj)
        {
            retval = qobject_cast<GtAbstractProperty*>(obj.get());
        }

        if (!retval && !m_silent)
        {
            gtWarning().medium() 
                        << QStringLiteral("GtPropertyFactory:")
                        << tr("Error casting property '%1'!").arg(T::staticMetaObject.className());
            gtWarning().medium() 
                        << tr("Known property classes:") << knownClasses();
        }

        obj.release();
        return retval;
    };


    registerProperty(T::staticMetaObject.className(), std::move(factory));
}

GtPropertyFactory::GtPropertyFactory(QObject* parent) : QObject(parent)
{
    registerQObjectClass<GtBoolProperty>();
    registerQObjectClass<GtDoubleProperty>();
    registerQObjectClass<GtIntProperty>();
    registerQObjectClass<GtStringProperty>();
}

GtPropertyFactory::~GtPropertyFactory() = default;

GtPropertyFactory*
GtPropertyFactory::instance()
{
    static GtPropertyFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtPropertyFactory(qApp);
    }
    return retval;
}


QStringList GtPropertyFactory::knownClasses() const
{
    return m_knownFactories.keys();
}

GtAbstractProperty*
GtPropertyFactory::newProperty(const QString& className,
                               const QString& id,
                               const QString& name) const
{
    if (m_knownFactories.contains(className))
    {
        return m_knownFactories[className](id, name);
    }
    else
    {
        if (!m_silent)
        {
            qWarning() << "WARNING: classname not found! (" << className << ")";
        }

        return nullptr;
    }
}

GtPropertyFactory&
GtPropertyFactory::registerProperty(const QString &className,
                                    FactoryFunction factory)
{
    if (m_knownFactories.contains(className))
    {
        gtWarning().noquote().nospace()
            << "Overwriting already registered Property type '"
            << className << " in GtPropertyFactory.";
    }

    m_knownFactories.insert(className, std::move(factory));

    return *this;
}

GtPropertyFactory &GtPropertyFactory::unregisterProperty(const QString &className)
{
    if (m_knownFactories.contains(className))
    {
        m_knownFactories.remove(className);
    }

    return *this;
}
