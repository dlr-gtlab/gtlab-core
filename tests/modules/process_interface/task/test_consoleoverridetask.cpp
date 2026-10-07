/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_consoleoverridetask.h"

#include "gt_structproperty.h"

#include <gt_logging.h>

#include <QObject>

TestConsoleOverrideSolver::TestConsoleOverrideSolver() :
    m_tolerance("tolerance", QObject::tr("Tolerance")),
    m_points("points", QObject::tr("Points"),
             GtPropertyStructContainer::Sequential),
    m_boundaries("boundaries", QObject::tr("Boundaries"),
                 GtPropertyStructContainer::Associative)
{
    setObjectName("Solver");

    registerProperty(m_tolerance);

    GtPropertyStructDefinition pointDefinition("Point");
    pointDefinition.defineMember("pressure", gt::makeDoubleProperty(1.));
    m_points.registerAllowedType(pointDefinition);

    GtPropertyStructDefinition boundaryDefinition("Boundary");
    boundaryDefinition.defineMember("pressure", gt::makeDoubleProperty(1.));
    m_boundaries.registerAllowedType(boundaryDefinition);

    registerPropertyStructContainer(m_points);
    registerPropertyStructContainer(m_boundaries);
}

TestConsoleOverrideTask::TestConsoleOverrideTask() :
    m_iterations("iterations", QObject::tr("Iterations")),
    m_readOnlyValue("readOnlyValue", QObject::tr("Read only value")),
    m_result("result", QObject::tr("Result"))
{
    setObjectName("Console Override Task");

    registerProperty(m_iterations);

    m_readOnlyValue.setReadOnly(true);
    registerProperty(m_readOnlyValue);

    registerMonitoringProperty(m_result);

    // two child objects with identical names to allow testing of the
    // indexed object access ("Solver[0]", "Solver[1]") and the ambiguity
    // error for the unindexed variant
    appendChild(new TestConsoleOverrideSolver);
    appendChild(new TestConsoleOverrideSolver);
}

bool
TestConsoleOverrideTask::runIteration()
{
    if (m_iterations < 0)
    {
        gtError() << QObject::tr("Console override test task run failed: "
                                 "iteration count is negative");
        return false;
    }

    m_result = m_iterations;

    return true;
}
