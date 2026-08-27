/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_mementoprojectcalculator.h"

#include "gt_coreapplication.h"
#include "gt_project.h"
#include "test_dmi_package.h"

TestMementoProjectCalculator::TestMementoProjectCalculator()
{
    setObjectName(QStringLiteral("Memento Project Modifier"));
    hideLabelProperty(true);
}

bool
TestMementoProjectCalculator::run()
{
    GtProject* project = gtApp ? gtApp->currentProject() : nullptr;
    if (!project)
    {
        gtError() << tr("No current project available");
        return false;
    }

    auto* package = project->findDirectChild<TestDmiPackage*>();
    if (!package)
    {
        gtError() << tr("Current project contains no TestDmiPackage");
        return false;
    }

    package->setMementoTestValue(42);
    return true;
}
