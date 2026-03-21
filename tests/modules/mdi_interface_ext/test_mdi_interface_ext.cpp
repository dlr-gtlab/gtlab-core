/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_datamodel_interface_ext
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_mdi_package.h"
#include "test_mdi_ext_package.h"
#include "test_mdi_ext_packageui.h"
#include "test_mdi_ext_viewer.h"
#include "test_mdi_ext_externalobjectviewer.h"
#include "test_mdi_ext_externalobjectui.h"

#include "test_dmi_externalobject.h"

/**
 * generated (v 0.1)
 */

#include "test_mdi_interface_ext.h"

GtVersionNumber
TestDatamodelInterfaceExt::version() const
{
    return GtVersionNumber(1, 2, 4);
}

QString
TestDatamodelInterfaceExt::description() const
{
    return "Test Mdi Interface Ext";
}

///gtdatamodelinterface
///generated

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
    retval << GT_METADATA(TestMdiExtExternalObjectViewer);

    return retval;
}

QMap<const char*, QMetaObject>
TestDatamodelInterfaceExt::uiItems()
{
    QMap<const char*, QMetaObject> retval;

    retval.insert(GT_CLASSNAME(TestMdiPackage),
                  GT_METADATA(TestMdiExtPackageUI));

    retval.insert(GT_CLASSNAME(TestDmiExternalObject),
                  GT_METADATA(TestMdiExtExternalObjectUI));

    return retval;
}
