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
    m_link("link", tr("link"), tr("link"), "", this,
           QStringList() << GT_CLASSNAME(GtAbstractDataZone), true)
{
    setObjectName("Test DMI Package");

    GtDataZone* dz = new GtDataZone;
    dz->setObjectName("DataZone");
    appendChild(dz);
    dz->setDefault(true);

    GtDataZone0D* dz0d = new GtDataZone0D;
    dz0d->setObjectName("DataZone0D");
    appendChild(dz0d);
    dz0d->setDefault(true);

    registerProperty(m_link);
}
