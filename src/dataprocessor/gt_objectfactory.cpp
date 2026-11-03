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
    registerClass(GT_METADATA(GtObjectGroup), builtinModuleId());
    registerClass(GT_METADATA(GtLabel), builtinModuleId());
    registerClass(GT_METADATA(GtLabelData), builtinModuleId());
    registerClass(GT_METADATA(GtPropertyConnection), builtinModuleId());
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
