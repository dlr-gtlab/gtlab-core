/* GTlab - Gas Turbine laboratory
 * Source File: test_module_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */


#include "test_module_interface.h"

GtVersionNumber
TestModuleInterface::version()
{
    return GtVersionNumber(0, 1, 2, QStringLiteral("desc"));
}

QString
TestModuleInterface::ident() const
{
    return "Test Module Interface";
}

QString
TestModuleInterface::description() const
{
    return "Test Module Interface Description";
}


