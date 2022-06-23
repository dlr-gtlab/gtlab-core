/* GTlab - Gas Turbine laboratory
 * Source File: test_datamodel_interface_ext
 * copyright 2009-2019 by DLR
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_mdi_package.h"
#include "test_mdi_ext_package.h"
#include "test_mdi_ext_packageui.h"
#include "test_mdi_ext_viewer.h"

/**
 * generated (v 0.1)
 */

#include "test_mdi_interface_ext.h"

GtVersionNumber
TestDatamodelInterfaceExt::version()
{
    return GtVersionNumber(1, 2, 4);
}

QString
TestDatamodelInterfaceExt::ident() const
{
    return "Test Mdi Interface Ext";
}

QString
TestDatamodelInterfaceExt::description() const
{
    return "Test Mdi Interface Ext";
}

///gtdatamodelinterface
///generated

QList<QMetaObject>
TestDatamodelInterfaceExt::data()
{
    QList<QMetaObject> metaData{};

    return metaData;
}

QMetaObject
TestDatamodelInterfaceExt::package()
{
    return GT_METADATA(TestMdiExtPackage);
}

bool
TestDatamodelInterfaceExt::standAlone()
{
    return false;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::mdiItems()
{
    QList<QMetaObject> retval{};

    retval << GT_METADATA(TestMdiExtViewer);

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::dockWidgets()
{
    QList<QMetaObject> retval{};

    return retval;
}

QMap<const char*, QMetaObject>
TestDatamodelInterfaceExt::uiItems()
{
    QMap<const char*, QMetaObject> retval;

    retval.insert(GT_CLASSNAME(TestMdiPackage), GT_METADATA(TestMdiExtPackageUI));

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::postItems()
{
    QList<QMetaObject> retval{};

    return retval;
}

QList<QMetaObject>
TestDatamodelInterfaceExt::postPlots()
{
    QList<QMetaObject> retval{};

    return retval;
}
