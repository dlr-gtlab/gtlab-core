/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_mdi_interface
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include "test_mdi_package.h"

///generated (v 0.1)
TestMdiPackage::TestMdiPackage() :
    m_mementoExternalValue("mementoExternalValue", tr("Memento External Value"),
                           tr("Value read by the Memento E2E task"), 2)
{
    setObjectName("Test MDI Package");
    registerProperty(m_mementoExternalValue);
}

int
TestMdiPackage::mementoExternalValue() const
{
    return m_mementoExternalValue.getVal();
}
