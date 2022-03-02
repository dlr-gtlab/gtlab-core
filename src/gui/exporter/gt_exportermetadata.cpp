/* GTlab - Gas Turbine laboratory
 * Source File: gt_exportermetadata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_abstractexporter.h"

#include "gt_exportermetadata.h"

GtExporterMetaData::GtExporterMetaData()
{
    // nothing to do here
}

GtExporterMetaData::GtExporterMetaData(QMetaObject& exporter)
{
    initialize(exporter);
}

const QString&
GtExporterMetaData::classname() const
{
    return m_classname;
}

const QString&
GtExporterMetaData::id() const
{
    return m_id;
}

const QStringList&
GtExporterMetaData::targets() const
{
    return m_targets;
}

const QIcon&
GtExporterMetaData::icon() const
{
    return m_icon;
}

void
GtExporterMetaData::initialize(QMetaObject& exporter)
{
    // create object from meta data
    QObject* qobj = exporter.newInstance();

    // check object
    if (!qobj)
    {
        return;
    }

    // cast object to abstract exporter class
    GtAbstractExporter* exp = qobject_cast<GtAbstractExporter*>(qobj);

    // check casted exporter object
    if (!exp)
    {
        // object nor an abstract exporter -> delete qobject
        delete qobj;
        return;
    }

    // set class name from meta object
    m_classname = exporter.className();

    // set identification string from exporter class
    m_id = exp->objectName();

    // set targets
    m_targets = exp->targets();

    // set icon
    m_icon = exp->icon();

    // clear object
    delete qobj;
}
