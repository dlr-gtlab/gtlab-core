/* GTlab - Gas Turbine laboratory
 * Source File: test_datamodel_interface_ext
 * copyright 2009-2019 by DLR
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_dmi_package.h"
#include "test_dmi_ext_package.h"
#include "test_dmi_ext_packageui.h"

/**
 * generated (v 0.1)
 */

#include "test_datamodel_interface_ext.h"

GtVersionNumber
TestDatamodelInterfaceExt::version()
{
    return 1;
}

QString
TestDatamodelInterfaceExt::ident() const
{
    return "Test Datamodel Interface Ext";
}

QString
TestDatamodelInterfaceExt::description() const
{
    return "Test Datamodel Interface Ext";
}


///gtdatamodelinterface
///generated

QList<QMetaObject>
TestDatamodelInterfaceExt::data()
{
    QList<QMetaObject> metaData;

    return metaData;
}

QMetaObject
TestDatamodelInterfaceExt::package()
{
    return GT_METADATA(TestDmiExtPackage);
}

bool
TestDatamodelInterfaceExt::standAlone()
{
    return false;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::mdiItems()
{
    QList<QMetaObject> retval;

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::dockWidgets()
{
    QList<QMetaObject> retval;

    return retval;
}

QMap<const char*, QMetaObject>
TestDatamodelInterfaceExt::uiItems()
{
    QMap<const char*, QMetaObject> retval;

    retval.insert(GT_CLASSNAME(TestDmiPackage), GT_METADATA(TestDmiExtPackageUI));

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::postItems()
{
    QList<QMetaObject> retval;

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::postPlots()
{
    QList<QMetaObject> retval;

    return retval;
}
