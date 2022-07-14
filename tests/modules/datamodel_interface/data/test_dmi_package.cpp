/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_package
 * copyright 2009-2019 by DLR
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#include "test_dmi_package.h"

#include "test_dmi_class.h"
#include "gt_abstractdatazone.h"

///generated (v 0.1)
TestDmiPackage::TestDmiPackage() :
    m_link("link", tr("link"), tr("link"), "", this,
           QStringList() << GT_CLASSNAME(GtAbstractDataZone), true)
{
    setObjectName("Test DMI Package");

    TestDmClass* data = new TestDmClass;
    data->setObjectName("Data");
    appendChild(data);
    data->setDefault(true);

    registerProperty(m_link);
}
