/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfactory.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_object.h"
#include "gt_boolproperty.h"
#include "gt_doubleproperty.h"
#include "gt_intproperty.h"
#include "gt_stringproperty.h"

#include "gt_propertyfactory.h"

GtPropertyFactory::GtPropertyFactory(QObject* parent) : QObject(parent)
{
    m_knownClasses.insert(GT_CLASSNAME(GtBoolProperty),
                          GT_METADATA(GtBoolProperty));

    m_knownClasses.insert(GT_CLASSNAME(GtDoubleProperty),
                          GT_METADATA(GtDoubleProperty));

    m_knownClasses.insert(GT_CLASSNAME(GtIntProperty),
                          GT_METADATA(GtIntProperty));

    m_knownClasses.insert(GT_CLASSNAME(GtStringProperty),
                          GT_METADATA(GtStringProperty));
}

GtPropertyFactory*
GtPropertyFactory::instance()
{
    static GtPropertyFactory* retval = Q_NULLPTR;
    if (retval == Q_NULLPTR)
    {
        retval = new GtPropertyFactory(qApp);
    }
    return retval;
}

GtObject*
GtPropertyFactory::newObject(const QString& /*className*/, GtObject* /*parent*/)
{
    return Q_NULLPTR;
}

GtAbstractProperty*
GtPropertyFactory::newProperty(const QString& className,
                               const QString& id,
                               const QString& name)
{
    GtAbstractProperty* retval = Q_NULLPTR;

    if (m_knownClasses.contains(className))
    {
        const QMetaObject& metaObj = m_knownClasses[className];

        QObject* obj = metaObj.newInstance(Q_ARG(QString, id),
                                           Q_ARG(QString, name));

        if (obj == Q_NULLPTR)
        {
            if (!m_silent)
            {
                qCritical() << QStringLiteral("GtPropertyFactory : ") <<
                               tr("Error creating") <<
                               QStringLiteral(" ") <<
                               className;
                qDebug() << knownClasses();
            }

            return nullptr;
        }

        retval = qobject_cast<GtAbstractProperty*>(obj);

        if (retval == Q_NULLPTR)
        {
            delete obj;

            if (!m_silent)
            {
                qCritical() << QStringLiteral("GtPropertyFactory : ") <<
                               tr("Error casting property") <<
                               QStringLiteral(" ") <<
                               className;
                qDebug() << knownClasses();
            }

            return nullptr;
        }
    }
    else
    {
        if (!m_silent)
        {
            // TODO: uncomment
//            qWarning() << "WARNING: classname not found! (" << className << ")";
        }

        return nullptr;
    }

    return retval;
}
