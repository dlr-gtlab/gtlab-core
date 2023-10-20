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


GtObjectFactory::GtObjectFactory(QObject* parent) : QObject(parent)
{
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
