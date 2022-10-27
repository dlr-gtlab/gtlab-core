/* GTlab - Gas Turbine laboratory
 * Source File: test_module_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include <QDomElement>

#include "gt_logging.h"
#include "gt_commandlinefunction.h"
#include "gt_commandlineparser.h"

#include "test_module_interface.h"

#include "gt_functional_interface.h"

#include <iostream>

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

double
mySquare(double x)
{
    return x*x;
}

QList<gt::InterfaceFunction>
TestModuleInterface::sharedFunctions() const
{
    auto fun = gt::interface::makeInterfaceFunction(
                   "mySquare", mySquare, "Returns the square of x");

    return {fun};
}

namespace test_module_batch_command {


void
showHelp()
{
    std::cout << "For the test function there is a custom parsing implmented"
              << std::endl;

    std::cout << std::endl;

    std::cout << "The function test_function can be used to join its arguments"
              << " with the # symbol between the arguments" << std::endl;

    std::cout << "Therefore use the function like this:" << std::endl;
    std::cout << "\t test_function <Argument1> <Argument2> ... <ArgumentN>"
              << std::endl;
}

int
mainFun(QStringList const& args)
{
    GtCommandLineParser p;
    p.addHelpOption();


    if (!p.parse(args))
    {
        gtError() << "Empty argument list";
        return -1;
    }

    if (p.helpOption())
    {
        showHelp();
        return 0;
    }

    gtInfo() << args.join("#");
    return 0;
}

}
QList<GtCommandLineFunction>
TestModuleInterface::commandLineFunctions() const
{
    /// The constructor
    auto fun = gt::commandline::makeCommandLineFunction(
                "test_function", test_module_batch_command::mainFun,
                "combines arguments");
    /// demonstration to set options by a chain of setters
    fun.setUseDefaultHelp(false).setArgs({}).setOptions({});

    return {fun};
}

TestModuleInterface::MetaInformation
TestModuleInterface::metaInformation() const
{
    MetaInformation m;
    m.author = "Awesome author";
    m.authorContact = "test@gtlab.de";
    m.licenseShort = "TestModule without public access";

    return m;
}

void
TestModuleInterface::onLoad()
{

}
