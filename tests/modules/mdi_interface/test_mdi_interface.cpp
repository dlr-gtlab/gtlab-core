/* GTlab - Gas Turbine laboratory
 * Source File: test_mdi_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include "test_mdi_package.h"
#include "test_mdi_data.h"
#include "test_mdi_packageui.h"
#include "test_mdi_dockwidget.h"
#include "test_mdi_viewer.h"


#include "test_mdi_interface.h"

GtVersionNumber
TestMdiInterface::version()
{
    return GtVersionNumber(1,2,3);
}

QString
TestMdiInterface::ident() const
{
    return "Test Mdi Interface";
}

QString
TestMdiInterface::description() const
{
    return "Test Mdi Interface";
}


///gtdatamodelinterface
///generated

QList<QMetaObject>
TestMdiInterface::data()
{
    QList<QMetaObject> metaData;

    metaData << GT_METADATA(TestMdiData);

    return metaData;
}

QMetaObject
TestMdiInterface::package()
{
    return GT_METADATA(TestMdiPackage);
}

bool
TestMdiInterface::standAlone()
{
    return true;
}

QList<QMetaObject>
TestMdiInterface::mdiItems()
{
    QList<QMetaObject> retval;

    retval << GT_METADATA(TestMdiViewer);

    return retval;
}

QList<QMetaObject>
TestMdiInterface::dockWidgets()
{
    QList<QMetaObject> retval;

    retval << GT_METADATA(TestMdiDockWidget);

    return retval;
}

QMap<const char*, QMetaObject>
TestMdiInterface::uiItems()
{
    QMap<const char*, QMetaObject> retval;

    retval.insert(GT_CLASSNAME(TestMdiPackage), GT_METADATA(TestMdiPackageUI));

    return retval;
}

QList<QMetaObject>
TestMdiInterface::postItems()
{
    QList<QMetaObject> retval{};

    return retval;
}

QList<QMetaObject>
TestMdiInterface::postPlots()
{
    QList<QMetaObject> retval{};

    return retval;
}

