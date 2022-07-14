#include "test_calculator.h"
#include "gt_datazone.h"

#include "test_dmi_package.h"

TestCalculator::TestCalculator() :
    m_value("value", "value"),
    m_result("result", "result"),
    m_objectLink(QStringLiteral("link"), tr("Link"),
                  tr("Target Component"), QString(), this,
                  QStringList() << GT_CLASSNAME(GtDataZone) ),
    m_objectPath("prePkg", "TestDmi Package Path",
                 "TestDmi Package Path", "Test DMI Package",
                 this, QStringList() << GT_CLASSNAME(TestDmiPackage))
{
    setObjectName("Test Calculator");
    hideLabelProperty(true);

    registerProperty(m_value);

    registerProperty(m_objectLink, tr("Link Test"));
    registerProperty(m_objectPath, tr("Link Test"));

    registerMonitoringProperty(m_result);
}

bool
TestCalculator::run()
{
    auto pgk = data<TestDmiPackage*>(m_objectPath);

    if (!pgk)
    {
        gtError() << "Package not found via object-path property.";
        return false;
    }

    auto dz = data<GtDataZone*>(m_objectLink);

    if (!dz)
    {
        gtError() << "Datazone not found via object-link property.";
        return false;
    }

    gtInfo() << "The objects were found successfully by path and link property";

    m_result.setVal(m_value * m_value);

    return true;
}
