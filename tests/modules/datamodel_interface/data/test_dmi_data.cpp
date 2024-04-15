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
    m_containerRO("tasks", "tasks (ro)"),
    m_containerHidden("hiddenNotes", "Hidden Notes"),
    m_mode("modeProp", "ModeProp", "A mode-property"),
    m_propTypeA("Type A", "Type A brief"),
    m_propTypeB("Type B", "Type B brief"),
    m_optionalValue("optionalValue", "Optional Value",
                    "This property is optional"),
    m_fruit("TestEnum", "TestEnum", "")
{
    setObjectName("Internal_Data");
    setFlag(UserDeletable);
    setFlag(UserRenamable);

    registerProperty(m_value);

    GtPropertyStructDefinition stringEntryDef("StringStruct");
    stringEntryDef.defineMember("value", gt::makeOptional(gt::makeStringProperty(""), true));
    stringEntryDef.defineMember("name", gt::makeReadOnly(gt::makeStringProperty("")));

    m_container.registerAllowedType(stringEntryDef);
    m_containerRO.registerAllowedType(stringEntryDef);
    m_containerHidden.registerAllowedType(stringEntryDef);

    m_containerRO.setFlags(GtPropertyStructContainer::IsReadOnly);
    m_containerHidden.setFlags(GtPropertyStructContainer::IsHidden);



    registerPropertyStructContainer(m_container);
    registerPropertyStructContainer(m_containerRO);
    registerPropertyStructContainer(m_containerHidden);

    m_mode.registerSubProperty(m_propTypeA);
    m_mode.registerSubProperty(m_propTypeB);

    registerProperty(m_mode);

    m_optionalValue.setOptional(true);

    registerProperty(m_optionalValue);

    m_fruit = Fruit::Apple;

    registerProperty(m_fruit);

    auto& e1 = m_containerRO.newEntry("StringStruct", "entry_1");
    e1.setMemberVal("name", "Speed");
    auto& e2 = m_containerRO.newEntry("StringStruct", "entry_2");
    e2.setMemberVal("name", "Force");
    auto& e3 = m_containerRO.newEntry("StringStruct", "entry_3");
    e3.setMemberVal("name", "Power");
}
