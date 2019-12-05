/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_abstractobjectfactory.h"
#include "gt_object.h"

GtAbstractObjectFactory::GtAbstractObjectFactory(bool silent) : m_silent(silent)
{
}

GtObject*
GtAbstractObjectFactory::newObject(const QString& className, GtObject* parent)
{
    GtObject* retval = 0;
    if (m_knownClasses.contains(className))
    {
        const QMetaObject& mo = m_knownClasses[className];
        retval = newObject(mo, parent);
        if (retval == 0)
        {
            if (!m_silent)
            {
                qCritical() << "GtObjectFactory : Error creating " << className;
                qDebug() << knownClasses();
            }
            return NULL;
        }
    }
    else
    {
        if (!m_silent)
        {
            // TODO: uncomment
//            qWarning() << "WARNING: classname not found! (" << className << ")";
        }
        return NULL;
    }

    retval->setFactory(this);
    return retval;
}

QString
GtAbstractObjectFactory::superClassName(const QString& className)
{
    if (m_knownClasses.contains(className))
    {
        const QMetaObject& mo = m_knownClasses[className];
        return mo.superClass()->className();
    }

    return QString();
}

QStringList
GtAbstractObjectFactory::classHierarchy(const QString& className)
{
    QStringList retval;

    if (m_knownClasses.contains(className))
    {
        retval << className;
        const QMetaObject& mo = m_knownClasses[className];
        if (const QMetaObject* smo = mo.superClass())
        {
            retval.append(classHierarchy(smo));
        }
    }

    return retval;
}

QStringList
GtAbstractObjectFactory::classHierarchy(const QMetaObject* metaObj)
{
    QStringList retval;

    if (metaObj == Q_NULLPTR)
    {
        return retval;
    }

    retval << metaObj->className();
    retval.append(classHierarchy(metaObj->superClass()));

    return retval;
}

bool
GtAbstractObjectFactory::knownClass(const QString& className)
{
    return m_knownClasses.contains(className);
}

QStringList
GtAbstractObjectFactory::knownClasses() const
{
    return m_knownClasses.keys();
}

bool
GtAbstractObjectFactory::registerClass(QMetaObject metaObj)
{
    QString classname = metaObj.className();

    if (knownClass(classname))
    {
        if (!m_silent)
        {
            qWarning() << "WARNING" << ": " << "class already exists!" << " ("
                       << classname + ")";
        }
        return false;
    }

    m_knownClasses.insert(classname, metaObj);
    return true;
}

bool
GtAbstractObjectFactory::registerClasses(const QList<QMetaObject>& metaData)
{
    if (containsDuplicates(metaData))
    {
        return false;
    }

    foreach (const QMetaObject& metaObj, metaData)
    {
        registerClass(metaObj);
    }

    return true;
}

GtObject*
GtAbstractObjectFactory::newObject(const QMetaObject& metaObj, GtObject* parent)
{
    GtObject* retval = 0;

    retval = qobject_cast<GtObject*>(metaObj.newInstance());
    if (retval == 0)
    {
        return NULL;
    }

    if (parent != 0)
    {
        parent->appendChild(retval);
    }

    return retval;
}

bool
GtAbstractObjectFactory::containsDuplicates(const QList<QMetaObject>& metaData)
{
    foreach (const QMetaObject& mobj, metaData)
    {
        QString classname = mobj.className();
        if (m_knownClasses.contains(classname))
        {
            if (!m_silent)
            {
                qWarning() << "class name already exists!";
            }
            return true;
        }
    }
    return false;
}

bool
GtAbstractObjectFactory::allInvokable(const QList<QMetaObject>& metaData)
{
    foreach (const QMetaObject& mobj, metaData)
    {
//        qDebug() << "mobj = " << mobj.className();
        if (!invokable(mobj))
        {
            return false;
        }
    }
    return true;
}

bool
GtAbstractObjectFactory::invokable(const QMetaObject& metaObj)
{
    GtObject* obj = newObject(metaObj);
    if (obj == NULL)
    {
        if (!m_silent)
        {
            qWarning() << "class " << metaObj.className() << " not invokable!";
        }
        return false;
    }
    else
    {
        delete obj;
    }

    return true;
}

