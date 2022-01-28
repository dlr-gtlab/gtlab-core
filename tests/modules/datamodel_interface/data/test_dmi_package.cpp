/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_package
 * copyright 2009-2019 by DLR
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#include "test_dmi_package.h"
#include "gt_datazone.h"
#include "gt_datazone0d.h"

///generated (v 0.1)
TestDmiPackage::TestDmiPackage() :
    m_test("resultTable", "Result Table", "Link to result table.", "",
           this, QStringList() << GT_CLASSNAME(GtObject))
{
    setObjectName("Test DMI Package");

    registerProperty(m_test);

    GtDataZone* dz = new GtDataZone;
    dz->setObjectName("DZ");
    dz->setDefault(true);
    appendChild(dz);

    GtDataZone0D* dz0D = new GtDataZone0D;
    dz0D->setObjectName("DZ0D");
    dz0D->setDefault(true);
    appendChild(dz0D);
}
