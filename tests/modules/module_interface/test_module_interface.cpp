/* GTlab - Gas Turbine laboratory
 * Source File: test_module_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include <QDomElement>

#include "gt_logging.h"

#include "test_module_interface.h"

#include "gt_functional_interface.h"

bool
testConvert(QDomElement& xml, const QString& scope)
{
    gtDebug() << "do convert (" << scope << ")...";

    // rename GtDataZone object if name is equal to "DataZone"
    QDomElement obj = xml.firstChildElement(QStringLiteral("object"));
    while (!obj.isNull())
    {
        if (obj.attribute("class") == "GtDataZone" &&
                obj.attribute("name") == "DataZone")
        {
            obj.setAttribute("name", "FancyDataZone");
        }

        obj = obj.nextSiblingElement(QStringLiteral("object"));
    }

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

QList<VersionUpgradeRoutine>
TestModuleInterface::upgradeRoutines() const
{
    QList<VersionUpgradeRoutine> retval;

    retval.append({{0, 0, 2}, testConvert});
    retval.append({{0, 0, 1}, testConvert});
    retval.append({{0, 1, 0}, testConvert});
    retval.append({{0, 1, 2, "desc"}, testConvert});

    return retval;
}

double mySquare(double x)
{
    return x*x;
}

QList<gtlab::InterfaceFunction> TestModuleInterface::sharedFunctions() const
{
    auto fun = gtlab::interface::make_interface_function("mySquare", mySquare,
                                              "Returns the square of x");

    return {fun};
}
