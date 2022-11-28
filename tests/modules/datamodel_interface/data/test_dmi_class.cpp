/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_class.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "test_dmi_class.h"

TestDmClass::TestDmClass() :
    m_value("value", "Value", "Value to use in Example",
            GtUnit::EnergyDensity, 1.0)
{
    setObjectName("DMI Object");

    registerProperty(m_value);
}
