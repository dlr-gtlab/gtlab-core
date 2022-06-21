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

QList<VersionUpdateRoutine>
TestModuleInterface::updateRoutines() const
{
    QList<VersionUpdateRoutine> retval;

    retval.append({{0, 0, 2}, testConvert});
    retval.append({{0, 0, 1}, testConvert});
    retval.append({{0, 1, 0}, testConvert});
    retval.append({{0, 1, 2, "desc"}, testConvert});

    return retval;
}

void
TestModuleInterface::init()
{
    // nothing to do here
}
