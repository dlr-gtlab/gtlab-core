/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorhelperfactory.cpp
 *
 *  Created on: 09.08.2019
 *  Author: Marvin Noethen (AT-TW)
 *  Tel.: +49 2203 601 2692
 */

#include <QCoreApplication>

#include "gt_calculatorhelperfactory.h"

#include "gt_object.h"

GtCalculatorHelperFactory::GtCalculatorHelperFactory(QObject* parent) :
    QObject(parent)
{

}

GtCalculatorHelperFactory*
GtCalculatorHelperFactory::instance()
{
    static GtCalculatorHelperFactory* retval = nullptr;

    if (!retval)
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
        return nullptr;
    }

    if (!parent)
    {
        return nullptr;
    }

    QList<GtObject*> gtObjs = parent->findDirectChildren<GtObject*>(objName);

    QList<GtObject*> founds;

    foreach (GtObject* gtObj, gtObjs)
    {
        if (gtObj)
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
        return nullptr;
    }

    GtObject* retval = nullptr;

    QList<QString> helperClasses = m_calcHelperConnection.value(
                                     QString::fromUtf8(parent->metaObject()->
                                                           className()));
    foreach (QString helperClass, helperClasses)
    {
        if (helperClass == helperClassName)
        {
            retval = newObject(helperClass);

            if (!retval)
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

    return nullptr;
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

    if (std::any_of(std::begin(calcHelper), std::end(calcHelper), [&helper](const QString& helperName) {
            return helperName == helper.className();
        }))
    {
        return true;
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
