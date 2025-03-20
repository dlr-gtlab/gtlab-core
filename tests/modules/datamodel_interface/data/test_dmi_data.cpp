/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_dmi_class.cpp
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
    m_propTypeB("Type B", "Type B brief"),
    m_optionalValue("optionalValue", "Optional Value",
                    "This property is optional"),
    m_fruit("TestEnum", "TestEnum", ""),
    m_electricVoltage("Voltage", "Voltage", "Voltage value to use in Example",
                      GtUnit::Voltage, 1.0),
    m_electricCurrent("Current", "Current", "Current value to use in Example",
                      GtUnit::Current, 1.0),
    m_electricResistance("Resistance", "Resistance",
                         "Resistance value to use in Example",
                         GtUnit::Resistance, 1.0),
    m_electricImpedance("Impedance", "Impedance",
                        "Impedance value to use in Example",
                        GtUnit::Impedance, 1.0),
{
    setObjectName("Internal_Data");
    setFlag(UserDeletable);
    setFlag(UserRenamable);

    registerProperty(m_value);

    GtPropertyStructDefinition stringEntryDef("StringStruct");
    stringEntryDef.defineMember("value", gt::makeOptional(gt::makeStringProperty(""), true));

    m_container.registerAllowedType(stringEntryDef);

    registerPropertyStructContainer(m_container);

    m_mode.registerSubProperty(m_propTypeA);
    m_mode.registerSubProperty(m_propTypeB);

    registerProperty(m_mode);

    m_optionalValue.setOptional(true);

    registerProperty(m_optionalValue);

    m_fruit = Fruit::Apple;

    registerProperty(m_fruit);
}
