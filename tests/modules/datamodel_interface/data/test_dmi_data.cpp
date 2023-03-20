/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_class.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "test_dmi_data.h"
#include "gt_structproperty.h"
#include "gt_stringproperty.h"

TestDmiData::TestDmiData() :
    m_value("value", "Value", "Value to use in Example",
            GtUnit::EnergyDensity, 1.0),
    m_container("notes", "notes"),
    m_mode("modeProp", "ModeProp", "A mode-property"),
    m_propTypeA("Type A", "Type A brief"),
    m_propTypeB("Type B", "Type B brief")
{
    setObjectName("Internal_Data");
    setFlag(UserDeletable);
    setFlag(UserRenamable);

    registerProperty(m_value);

    GtPropertyStructDefinition stringEntryDef("StringStruct");
    stringEntryDef.defineMember("value", gt::makeStringProperty(""));

    m_container.registerAllowedType(stringEntryDef);

    registerPropertyStructContainer(m_container);

    m_mode.registerSubProperty(m_propTypeA);
    m_mode.registerSubProperty(m_propTypeB);

    registerProperty(m_mode);
}
