/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorhelperfactory.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 09.08.2019
 *  Author: Marvin Noethen (AT-TW)
 *  Tel.: +49 2203 601 2692
 */

#include <QCoreApplication>
#include <QDebug>

#include "gt_calculatorhelperfactory.h"

#include "gt_object.h"

GtCalculatorHelperFactory::GtCalculatorHelperFactory(QObject* parent) :
    QObject(parent)
{

}

GtCalculatorHelperFactory*
GtCalculatorHelperFactory::instance()
{
    static GtCalculatorHelperFactory* retval = 0;

    if (retval == 0)
    {
        retval = new GtCalculatorHelperFactory(qApp);
    }

    return retval;
}

GtObject*
GtCalculatorHelperFactory::newCalculatorHelper(const QString& helperClassName,
                                               const QString& objName,
                                               GtObject* parent)
{
    if (helperClassName.isEmpty())
    {
        return Q_NULLPTR;
    }

    if (parent == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    QList<GtObject*> gtObjs = parent->findDirectChildren<GtObject*>(objName);

    QList<GtObject*> founds;

    foreach (GtObject* gtObj, gtObjs)
    {
        if (gtObj != Q_NULLPTR)
        {
            if (QString::fromUtf8(gtObj->metaObject()->className()) ==
                    helperClassName &&
                gtObj->objectName() == objName)
            {
                founds.append(gtObj);
            }
        }
    }

    if (founds.size() == 1)
    {
        return founds.first();
    }
    else if (founds.size() > 1)
    {
        return Q_NULLPTR;
    }

    GtObject* retval = Q_NULLPTR;

    QList<QString> helperClasses = m_calcHelperConnection.value(
                                     QString::fromUtf8(parent->metaObject()->
                                                           className()));
    foreach (QString helperClass, helperClasses)
    {
        if (helperClass == helperClassName)
        {
            retval = newObject(helperClass);

            if (retval == Q_NULLPTR)
            {
                return retval;
            }

            retval->setObjectName(objName);

            if (retval->thread() != parent->thread())
            {
                gt::moveToThread(*retval, parent->thread());
            }

            parent->appendChild(retval);

            return retval;
        }
    }

    return Q_NULLPTR;
}

bool
GtCalculatorHelperFactory::connectHelper(
    const QString& parentClassName, QMetaObject helper)
{
    if (parentClassName.isEmpty())
    {
        return false;
    }

    QList<QString> calcHelper = m_calcHelperConnection.value(parentClassName);

    foreach (QString helperName, calcHelper)
    {
        if (helperName == helper.className())
        {
            return true;
        }
    }

    registerClass(helper);
    calcHelper.append(QString::fromUtf8(helper.className()));

    m_calcHelperConnection.insert(parentClassName, calcHelper);

    return true;
}

QStringList
GtCalculatorHelperFactory::connectedHelper(const QString& parentClassName)
{
    return m_calcHelperConnection.value(parentClassName);
}
