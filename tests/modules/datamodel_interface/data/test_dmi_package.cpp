/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_package
 * copyright 2009-2019 by DLR
 * 
 * Created on: 27.05.2020
 * Author: S. Reitenbach 
 */

#include "test_dmi_package.h"

#include "test_dmi_data.h"
#include "test_dmi_externalobject.h"

///generated (v 0.1)
TestDmiPackage::TestDmiPackage() :
    m_link("link", tr("link"), tr("link"), "", this,
           QStringList() << GT_CLASSNAME(GtObject), true)
{
    setObjectName("Test DMI Package");

    auto* data = new TestDmiData;
    data->setDefault(true);
    appendChild(data);

    auto* extData = new TestDmiExternalObject;
    extData->setDefault(true);
    appendChild(extData);

    registerProperty(m_link);
}
