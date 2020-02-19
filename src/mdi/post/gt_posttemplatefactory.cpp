/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.01.2016
 *  Author: Maximilian Vieweg (AT-TW)
 *  Tel.: +49 2203 601 4799
 */

#include <QCoreApplication>

#include "gt_posttemplatefactory.h"

#include "gt_posttemplate.h"
#include "gt_stringcontainer.h"

GtPostTemplateFactory::GtPostTemplateFactory(QObject* parent) : QObject(parent)
{
    /* GtStringContainer */
    m_knownClasses.insert(GtStringContainer::staticMetaObject.className(),
                          GtStringContainer::staticMetaObject);
}

GtPostTemplateFactory*
GtPostTemplateFactory::instance()
{
    static GtPostTemplateFactory* retval = 0;
    if (retval == 0)
    {
        retval = new GtPostTemplateFactory(qApp);
    }
    return retval;
}

void
GtPostTemplateFactory::registerPlots(QList<QMetaObject> metaObjects)
{
    foreach (QMetaObject metaObj, metaObjects)
    {
        m_plotIds << metaObj.className();
        registerClass(metaObj);
    }
}

const QStringList&
GtPostTemplateFactory::knownPlots()
{
    return m_plotIds;
}