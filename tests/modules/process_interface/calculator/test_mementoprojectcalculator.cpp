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

#include <QDir>
#include <QFile>

TestMementoProjectCalculator::TestMementoProjectCalculator() :
    m_target("target", tr("Target"), tr("Package to modify"), this,
             {GT_CLASSNAME(TestDmiPackage)}),
    m_inputValue("inputValue", tr("Input Value"),
                 tr("Value added to the external project value"), 40),
    m_sideEffectFile("sideEffectFile", tr("Side Effect File"),
                     tr("Optional file written in the working directory")),
    m_observedValue("observedValue", tr("Observed Value"),
                    tr("Value reported by the operation event test"))
{
    setObjectName(QStringLiteral("Memento Project Modifier"));
    registerProperty(m_target);
    registerProperty(m_inputValue);
    registerProperty(m_sideEffectFile);
    registerMonitoringProperty(m_observedValue);
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
    setProgress(50);
    m_observedValue.setVal(target->mementoTestValue());

    if (!m_sideEffectFile.getVal().isEmpty())
    {
        QFile file(QDir::current().filePath(m_sideEffectFile.getVal()));
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text) ||
            file.write("side effect\n") < 0)
        {
            gtError() << tr("Cannot write side-effect file '%1'")
                             .arg(file.fileName());
            return false;
        }
    }

    gtInfo() << "Memento system test calculator completed";
    return true;
}
