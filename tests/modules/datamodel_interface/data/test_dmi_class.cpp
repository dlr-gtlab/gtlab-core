/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_class.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "test_dmi_class.h"


#include "gt_datazone.h"
#include "gt_datazone0d.h"

TestDmClass::TestDmClass()
{
    setObjectName("DMI Object");

    // datazone
    auto* dz = new GtDataZone;
    dz->setObjectName("DataZone");
    appendChild(dz);
    dz->setDefault(true);

    auto data = dz->fetchData();
    QMap<QString, QVector<double>> values;
    values.insert("my_param", {0.1234, 42, -1});
    data.setData1D({"my_param"}, {0, 1, 2}, "axis", values, {"[m]"});

    auto* dzEmpty = new GtDataZone;
    dzEmpty->setObjectName("DataZone-Empty");
    appendChild(dzEmpty);
    dzEmpty->setDefault(true);

    // datazone 0d
    auto* dz0d = new GtDataZone0D;
    dz0d->setObjectName("DataZone0D");
    appendChild(dz0d);
    dz0d->setDefault(true);

    auto data0D = dz0d->fetchData();
    data0D.setData({"my_param"}, {0.1234}, {"[K]"});

    auto* dz0dEmpty = new GtDataZone0D;
    dz0dEmpty->setObjectName("DataZone0D-Empty");
    appendChild(dz0dEmpty);
    dz0dEmpty->setDefault(true);
}
