/* GTlab - Gas Turbine laboratory
 * Source File: test_datamodel_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_dmi_package.h"
#include "test_dmi_class.h"
#include "test_dmi_packageui.h"

/**
 * generated (v 0.1)
 */

#include "test_datamodel_interface.h"

GtVersionNumber
TestDatamodelInterface::version()
{
    return 1;
}

QString
TestDatamodelInterface::ident() const
{
    return "Test Datamodel Interface";
}

QString
TestDatamodelInterface::description() const
{
    return "Test Datamodel Interface";
}


///gtdatamodelinterface
///generated

QList<QMetaObject>
TestDatamodelInterface::data()
{
    QList<QMetaObject> metaData;

    metaData << GT_METADATA(TestDmiClass);

    return metaData;
}

QMetaObject
TestDatamodelInterface::package()
{
    return GT_METADATA(TestDmiPackage);
}

bool
TestDatamodelInterface::standAlone()
{
    return true;
}

QList<QMetaObject>
TestDatamodelInterface::mdiItems()
{
    QList<QMetaObject> retval;

    return retval;
}

QList<QMetaObject>
TestDatamodelInterface::dockWidgets()
{
    QList<QMetaObject> retval;

    return retval;
}

QMap<const char*, QMetaObject>
TestDatamodelInterface::uiItems()
{
    QMap<const char*, QMetaObject> retval;

    retval.insert(GT_CLASSNAME(TestDmiPackage), GT_METADATA(TestDmiPackageUI));

    return retval;
}

QList<QMetaObject>
TestDatamodelInterface::postItems()
{
    QList<QMetaObject> retval;

    return retval;
}

QList<QMetaObject>
TestDatamodelInterface::postPlots()
{
    QList<QMetaObject> retval;

    return retval;
}
