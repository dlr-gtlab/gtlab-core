/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTCONSOLEOVERRIDETASK_H
#define TESTCONSOLEOVERRIDETASK_H

#include "gt_task.h"

#include "gt_doubleproperty.h"
#include "gt_intproperty.h"
#include "gt_object.h"
#include "gt_propertystructcontainer.h"

/**
 * @brief Test object with properties and property containers
 *
 * Used as child of the @ref TestConsoleOverrideTask to test the
 * console property override path resolution (child object properties,
 * sequential and associative property containers). Multiple instances
 * with the same object name ("Solver") are used to test indexed object
 * access.
 */
class TestConsoleOverrideSolver : public GtObject
{
    Q_OBJECT

public:
    Q_INVOKABLE TestConsoleOverrideSolver();

    GtDoubleProperty m_tolerance;

    //! sequential container ("points[2].pressure")
    GtPropertyStructContainer m_points;

    //! associative container ("boundaries[{inlet}].pressure")
    GtPropertyStructContainer m_boundaries;
};

/**
 * @brief Test task with properties for the console "--set" option
 *
 * The task provides
 *   - a simple property ("iterations")
 *   - a read only property ("readOnlyValue")
 *   - a monitoring property ("result")
 *   - two child objects named "Solver" with the properties of
 *     @ref TestConsoleOverrideSolver
 *
 * The execution fails for negative iteration counts, which allows to
 * test that a failed task execution does not save the project.
 */
class TestConsoleOverrideTask final : public GtTask
{
    Q_OBJECT

public:
    Q_INVOKABLE TestConsoleOverrideTask();

    bool runIteration() override;

    GtIntProperty m_iterations;

    GtDoubleProperty m_readOnlyValue;

    GtDoubleProperty m_result;
};

#endif // TESTCONSOLEOVERRIDETASK_H
