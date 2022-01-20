/* GTlab - Gas Turbine laboratory
 * Source File: gt_importermetadata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_abstractimporter.h"

#include "gt_importermetadata.h"

GtImporterMetaData::GtImporterMetaData()
{
    // nothing to do here
}

GtImporterMetaData::GtImporterMetaData(QMetaObject& importer)
{
    initialize(importer);
}

const QString&
GtImporterMetaData::classname() const
{
    return m_classname;
}

const QString&
GtImporterMetaData::id() const
{
    return m_id;
}

const QStringList&
GtImporterMetaData::targets() const
{
    return m_targets;
}

const QIcon&
GtImporterMetaData::icon() const
{
    return m_icon;
}

void
GtImporterMetaData::initialize(QMetaObject& importer)
{
    // create object from meta data
    QObject* qobj = importer.newInstance();

    // check object
    if (qobj == Q_NULLPTR)
    {
        return;
    }

    // cast object to abstract importer class
    GtAbstractImporter* imp = qobject_cast<GtAbstractImporter*>(qobj);

    // check casted importer object
    if (imp == Q_NULLPTR)
    {
        // object nor an abstract importer -> delete qobject
        delete qobj;
        return;
    }

    // set class name from meta object
    m_classname = importer.className();

    // set identification string from importer class
    m_id = imp->objectName();

    // set targets
    m_targets = imp->targets();

    // set icon
    m_icon = imp->icon();

    // clear object
    delete qobj;
}
