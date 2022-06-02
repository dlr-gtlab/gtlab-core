/* GTlab - Gas Turbine laboratory
 * Source File: test_module_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include "gt_logging.h"

#include "test_module_interface.h"

bool
testConvert(QDomElement& xml, const QString& scope)
{
    gtDebug() << "do convert (" << scope << ")...";

    return true;
}

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

void
TestModuleInterface::init()
{
    // nothing to do here
    register_converter(this->ident(), {0, 0, 2}, testConvert);
    register_converter(this->ident(), {0, 0, 1}, testConvert);
    register_converter(this->ident(), {0, 1, 4}, testConvert);
    register_converter(this->ident(), {2, 4, 0}, testConvert);
}
