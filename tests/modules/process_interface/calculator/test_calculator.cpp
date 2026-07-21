/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "test_calculator.h"

#include "test_dmi_package.h"

#include "gt_structproperty.h"

TestCalculator::TestCalculator() :
    m_value("value", tr("Value")),
    m_result("result", tr("Result")),
    m_objectLink(QStringLiteral("link"), tr("Link"),
                  tr("Target Component"), QString(), this,
                  QStringList() << GT_CLASSNAME(GtObject), true),
    m_objectPath("prePkg", "TestDmi Package Path",
                 "TestDmi Package Path", "Test DMI Package",
                 this, QStringList() << GT_CLASSNAME(TestDmiPackage)),
    m_inputArgs("inputArgs", "Input Args", GtPropertyStructContainer::Associative)

{
    setObjectName("Test Calculator");
    hideLabelProperty(true);

    registerProperty(m_value);

    registerProperty(m_objectLink, tr("Link Test"));
    registerProperty(m_objectPath, tr("Link Test"));

    registerMonitoringProperty(m_result);

    // Input args
    GtPropertyStructDefinition doubleMapEntry("DoubleStruct");
    doubleMapEntry.defineMember("value", gt::makeDoubleProperty(0.));
    m_inputArgs.registerAllowedType(doubleMapEntry);
    registerPropertyStructContainer(m_inputArgs);
}

bool
TestCalculator::run()
{
    if (projectPath().isEmpty())
    {
        gtError() << "Project Path is empty!";
        return false;
    }

    gtInfo() << "Project Path:" << projectPath();

    // test object path prop
    auto pgk = data<TestDmiPackage*>(m_objectPath);

    if (!pgk)
    {
        gtError() << "Package not found via object-path property.";
        return false;
    }


    gtInfo() << "The objects were found successfully by path and link property";

    // update monitoring property
    gtDebug() << "Value" << m_value;

    m_result.setVal(m_value * m_value);

    GtCalculator::CalculatorInformation info = calculatorInformation();

    gtInfo() << "The author of this calculator is"
             << info.author();
    gtInfo() << "The version of this calculator is"
             << info.version().toString();

    return true;
}
