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
    return (parent) ? makeCalculatorHelper(helperClassName, objName, *parent)
                    : nullptr;
}

GtObject*
GtCalculatorHelperFactory::makeCalculatorHelper(const QString& helperClassName,
                                                const QString& objName,
                                                GtObject& parent)
{
    if (helperClassName.isEmpty())
    {
        return nullptr;
    }

    QList<GtObject*> found = parent.findDirectChildren<GtObject*>(objName);
    found.erase(std::remove_if(found.begin(), found.end(),
                               [&helperClassName](GtObject* o) {
                                   assert(o);
                                   return o->metaObject()->className() !=
                                          helperClassName;
                               }),
                found.end());

    if (found.size() == 1)
    {
        return found.first();
    }
    if (found.size() > 1)
    {
        return nullptr;
    }

    const QList<QString> helperClasses = m_calcHelperConnection.value(
        QString::fromUtf8(parent.metaObject()->className()));

    for (const QString& helperClass : helperClasses)
    {
        if (helperClass == helperClassName)
        {
            auto retval = makeObject(helperClass);
            if (!retval) return nullptr;

            retval->setObjectName(objName);

            if (retval->thread() != parent.thread())
            {
                gt::moveToThread(*retval, parent.thread());
            }

            if (!parent.appendChild(retval.get())) return nullptr;

            return retval.release();
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
