/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.01.2016
 *  Author: Maximilian Vieweg (AT-TW)
 *  Tel.: +49 2203 601 4799
 */

#include <QCoreApplication>

#include "gt_posttemplatefactory.h"

#include "gt_stringcontainer.h"

GtPostTemplateFactory::GtPostTemplateFactory(QObject* parent) : QObject(parent)
{
    /* GtStringContainer */
    registerClass(GtStringContainer::staticMetaObject, builtinModuleId());
}

GtPostTemplateFactory*
GtPostTemplateFactory::instance()
{
    static GtPostTemplateFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtPostTemplateFactory(qApp);
    }
    return retval;
}

void
GtPostTemplateFactory::registerPlots(const QList<QMetaObject>& metaObjects,
                                     const QString& moduleId)
{
    foreach (QMetaObject metaObj, metaObjects)
    {
        m_plotIds << metaObj.className();
        registerClass(metaObj, moduleId);
    }
}

const QStringList&
GtPostTemplateFactory::knownPlots()
{
    return m_plotIds;
}
