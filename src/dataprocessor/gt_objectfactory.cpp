/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include "gt_objectfactory.h"
#include "gt_object.h"
#include "gt_objectgroup.h"
#include "gt_label.h"
#include "gt_labeldata.h"
#include "gt_propertyconnection.h"
#include "gt_stringproperty.h"
#include "gt_boolproperty.h"
#include "gt_intproperty.h"
#include "gt_doubleproperty.h"


GtObjectFactory::GtObjectFactory(QObject* parent) : QObject(parent)
{
    registerPropertyFactory(
        QStringLiteral("gtlab.core.string"), GtStringProperty::staticMetaObject,
        [](const QString& id) { return new GtStringProperty(id, id); });
    registerPropertyFactory(
        QStringLiteral("gtlab.core.bool"), GtBoolProperty::staticMetaObject,
        [](const QString& id) { return new GtBoolProperty(id, id); });
    registerPropertyFactory(
        QStringLiteral("gtlab.core.int"), GtIntProperty::staticMetaObject,
        [](const QString& id) { return new GtIntProperty(id, id); });
    registerPropertyFactory(
        QStringLiteral("gtlab.core.double"), GtDoubleProperty::staticMetaObject,
        [](const QString& id) { return new GtDoubleProperty(id, id); });
    m_knownClasses.insert(GT_CLASSNAME(GtObjectGroup),
                          GT_METADATA(GtObjectGroup));

    m_knownClasses.insert(GT_CLASSNAME(GtLabel),
                          GT_METADATA(GtLabel));

    m_knownClasses.insert(GT_CLASSNAME(GtLabelData),
                          GT_METADATA(GtLabelData));

    m_knownClasses.insert(GT_CLASSNAME(GtPropertyConnection),
                          GT_METADATA(GtPropertyConnection));             
}

GtObjectFactory*
GtObjectFactory::instance()
{
    static GtObjectFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtObjectFactory(qApp);
    }
    return retval;
}
