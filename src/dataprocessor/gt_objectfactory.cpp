/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QDirIterator>
#include <QCoreApplication>
#include "gt_objectfactory.h"
#include "gt_object.h"
#include "gt_objectgroup.h"
#include "gt_table.h"
#include "gt_tableaxis.h"
#include "gt_tablevalues.h"
//#include "gt_datazone0d.h"
//#include "gt_datazone.h"
#include "gt_label.h"
#include "gt_labeldata.h"
//#include "gt_datazonetable.h"
//#include "gt_datazonetablelist.h"
//#include "gt_abstractdatazone.h"
#include "gt_propertyconnection.h"
#include "gt_tablegroup.h"
//#include "gt_datazonetablemainaxis.h"

GtObjectFactory::GtObjectFactory(QObject* parent) : QObject(parent)
{
    m_knownClasses.insert(GT_CLASSNAME(GtObjectGroup),
                          GT_METADATA(GtObjectGroup));

    m_knownClasses.insert(GT_CLASSNAME(GtTable),
                          GT_METADATA(GtTable));

    m_knownClasses.insert(GT_CLASSNAME(GtTableAxis),
                          GT_METADATA(GtTableAxis));

    m_knownClasses.insert(GT_CLASSNAME(GtTableValues),
                          GT_METADATA(GtTableValues));

    m_knownClasses.insert(GT_CLASSNAME(GtLabel),
                          GT_METADATA(GtLabel));

    m_knownClasses.insert(GT_CLASSNAME(GtLabelData),
                          GT_METADATA(GtLabelData));

    m_knownClasses.insert(GT_CLASSNAME(GtPropertyConnection),
                          GT_METADATA(GtPropertyConnection));             

    m_knownClasses.insert(GT_CLASSNAME(GtTableGroup),
                          GT_METADATA(GtTableGroup));
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
