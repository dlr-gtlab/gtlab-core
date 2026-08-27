/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_mementoprojectcalculator.h"

#include "gt_coreapplication.h"
#include "gt_project.h"
#include "test_dmi_package.h"
#include "test_mdi_package.h"

TestMementoProjectCalculator::TestMementoProjectCalculator() :
    m_target("target", tr("Target"), tr("Package to modify"), this,
             {GT_CLASSNAME(TestDmiPackage)}),
    m_inputValue("inputValue", tr("Input Value"),
                 tr("Value added to the external project value"), 40)
{
    setObjectName(QStringLiteral("Memento Project Modifier"));
    registerProperty(m_target);
    registerProperty(m_inputValue);
    hideLabelProperty(true);
}

bool
TestMementoProjectCalculator::run()
{
    auto* target = data<TestDmiPackage*>(m_target);
    if (!target)
    {
        gtError() << tr("No TestDmiPackage target available");
        return false;
    }

    GtProject* project = gtApp ? gtApp->currentProject() : nullptr;
    if (!project)
    {
        gtError() << tr("No current project available");
        return false;
    }

    auto* externalPackage = project->findDirectChild<TestMdiPackage*>();
    if (!externalPackage)
    {
        gtError() << tr("Current project contains no TestMdiPackage");
        return false;
    }

    target->setMementoTestValue(m_inputValue.getVal() +
                                externalPackage->mementoExternalValue());
    return true;
}
