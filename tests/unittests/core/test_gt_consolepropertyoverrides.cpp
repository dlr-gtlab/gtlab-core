/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_consolepropertyoverrides.h"

#include "gt_object.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_doubleproperty.h"
#include "gt_intproperty.h"

#include <QString>

namespace
{

    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    class OverrideTestSolver : public GtObject
    {
    public:
        explicit OverrideTestSolver(GtObject* parent = nullptr) :
            m_tolerance("tolerance", "Tolerance"),
            m_points("points", "Points", GtPropertyStructContainer::Sequential),
            m_boundaries("boundaries", "Boundaries",
                         GtPropertyStructContainer::Associative)
        {
            setObjectName("Solver");

            registerProperty(m_tolerance);

            GtPropertyStructDefinition pointDefinition("Point");
            pointDefinition.defineMember("pressure",
                                         gt::makeDoubleProperty(1.));
            m_points.registerAllowedType(pointDefinition);

            GtPropertyStructDefinition boundaryDefinition("Boundary");
            boundaryDefinition.defineMember("pressure",
                                            gt::makeDoubleProperty(1.));
            m_boundaries.registerAllowedType(boundaryDefinition);

            registerPropertyStructContainer(m_points);
            registerPropertyStructContainer(m_boundaries);
        }

        GtDoubleProperty m_tolerance;
        GtPropertyStructContainer m_points;
        GtPropertyStructContainer m_boundaries;
    };

    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    class OverrideTestTask : public GtObject
    {
    public:
        OverrideTestTask() :
            m_iterations("iterations", "Iterations"),
            m_readOnlyValue("readOnlyValue", "Read only value"),
            m_result("result", "Result")
        {
            setObjectName("Test Task");

            registerProperty(m_iterations);

            m_readOnlyValue.setReadOnly(true);
            registerProperty(m_readOnlyValue);

            m_result.setMonitoring(true);
            registerProperty(m_result);

            auto* firstSolver = new OverrideTestSolver;
            firstSolver->setObjectName("Solver");
            appendChild(firstSolver);

            auto* secondSolver = new OverrideTestSolver;
            secondSolver->setObjectName("Solver");
            appendChild(secondSolver);

            // single child with a different name (unambiguous access)
            auto* calculator = new OverrideTestSolver;
            calculator->setObjectName("My Calculator");
            appendChild(calculator);

            // unambiguous group with two children of the same name to test
            // deeper object navigation ("Solver Group/My Calculator[1]...")
            auto* group = new GtObject;
            group->setObjectName("Solver Group");

            for (int i = 0; i < 2; i++)
            {
                auto* nested = new OverrideTestSolver;
                nested->setObjectName("Nested Solver");
                group->appendChild(nested);
            }

            appendChild(group);

            // child of a child to test deeper navigation
            m_inner = new GtObject(firstSolver);
            m_inner->setObjectName("Inner");
        }

        OverrideTestSolver* solverAt(int index)
        {
            const auto solvers =
                findDirectChildren<OverrideTestSolver*>("Solver");
            return index < solvers.size() ? solvers.at(index) : nullptr;
        }

        OverrideTestSolver* nestedSolverAt(int index)
        {
            auto* group = findDirectChild<GtObject*>("Solver Group");
            const auto solvers =
                group->findDirectChildren<OverrideTestSolver*>("Nested Solver");
            return index < solvers.size() ? solvers.at(index) : nullptr;
        }

        OverrideTestSolver* calculator()
        {
            return findDirectChild<OverrideTestSolver*>("My Calculator");
        }

        GtObject* inner() const
        {
            return m_inner;
        }

        GtIntProperty m_iterations;
        GtDoubleProperty m_readOnlyValue;
        GtDoubleProperty m_result;

    private:
        GtObject* m_inner{nullptr};
    };

    /**
 * @brief Returns the uuid string of an object enclosed in "{}"
 *
 * The GTlab uuid string already contains the braces, the path syntax
 * uses them as UUID marker, therefore both forms are accepted.
 */
    QString uuidSegment(const QString& uuid)
    {
        if (uuid.startsWith(QLatin1Char('{')) &&
            uuid.endsWith(QLatin1Char('}')))
        {
            return uuid;
        }

        return QStringLiteral("{%1}").arg(uuid);
    }

    /**
 * @brief Applies the overrides in the given order, like the console does
 *
 * @return empty string on success, otherwise the error of the first failure
 */
    QString applyAll(GtObject& root,
                     const QList<gt::console::PropertyOverride>& overrides)
    {
        for (const gt::console::PropertyOverride& override : overrides)
        {
            const QString error = gt::console::applyPropertyOverride(
                root, override.path, override.value);

            if (!error.isEmpty())
            {
                return error;
            }
        }

        return {};
    }

} // namespace

TEST(console_property_overrides, parse_valid_arguments)
{
    QStringList errors;

    const auto overrides = gt::console::parsePropertyOverrides(
        {"iterations=100", "a=1=2", "Solver/My Calculator[1].x=0.5"}, &errors);

    EXPECT_TRUE(errors.isEmpty());
    ASSERT_EQ(overrides.size(), 3);

    EXPECT_EQ(overrides[0].path, "iterations");
    EXPECT_EQ(overrides[0].value, "100");

    // split at the first '=', the remainder is the raw value
    EXPECT_EQ(overrides[1].path, "a");
    EXPECT_EQ(overrides[1].value, "1=2");

    EXPECT_EQ(overrides[2].path, "Solver/My Calculator[1].x");
    EXPECT_EQ(overrides[2].value, "0.5");
}

TEST(console_property_overrides, parse_invalid_arguments)
{
    QStringList errors;

    const auto overrides =
        gt::console::parsePropertyOverrides({"invalid", "=5"}, &errors);

    EXPECT_TRUE(overrides.isEmpty());
    EXPECT_EQ(errors.size(), 2);
}

TEST(console_property_overrides, property_on_root)
{
    OverrideTestTask task;

    EXPECT_TRUE(
        gt::console::applyPropertyOverride(task, "iterations", "42").isEmpty());
    EXPECT_EQ(task.m_iterations.getVal(), 42);
}

TEST(console_property_overrides, read_only_property_is_rejected)
{
    OverrideTestTask task;

    const QString error =
        gt::console::applyPropertyOverride(task, "readOnlyValue", "42");

    EXPECT_FALSE(error.isEmpty());
    EXPECT_DOUBLE_EQ(task.m_readOnlyValue.getVal(), 0.0);
}

TEST(console_property_overrides, monitoring_property_is_rejected)
{
    OverrideTestTask task;

    const QString error =
        gt::console::applyPropertyOverride(task, "result", "42");

    EXPECT_FALSE(error.isEmpty());
    EXPECT_DOUBLE_EQ(task.m_result.getVal(), 0.0);
}

TEST(console_property_overrides, invalid_value_conversion_is_rejected)
{
    OverrideTestTask task;

    const QString error =
        gt::console::applyPropertyOverride(task, "iterations", "abc");

    EXPECT_FALSE(error.isEmpty());
    EXPECT_EQ(task.m_iterations.getVal(), 0);
}

TEST(console_property_overrides, property_on_child_object)
{
    OverrideTestTask task;

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, "My Calculator.tolerance", "2.5")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(task.calculator()->m_tolerance.getVal(), 2.5);
}

TEST(console_property_overrides, ambiguous_object_name_is_rejected)
{
    OverrideTestTask task;

    const QString error =
        gt::console::applyPropertyOverride(task, "Solver.tolerance", "1.0");

    EXPECT_FALSE(error.isEmpty());
    EXPECT_TRUE(error.contains("ambiguous"));
}

TEST(console_property_overrides, indexed_child_objects)
{
    OverrideTestTask task;

    EXPECT_TRUE(
        gt::console::applyPropertyOverride(task, "Solver[0].tolerance", "1.0")
            .isEmpty());
    EXPECT_TRUE(
        gt::console::applyPropertyOverride(task, "Solver[1].tolerance", "2.0")
            .isEmpty());

    EXPECT_DOUBLE_EQ(task.solverAt(0)->m_tolerance.getVal(), 1.0);
    EXPECT_DOUBLE_EQ(task.solverAt(1)->m_tolerance.getVal(), 2.0);

    // explicit [0] is also valid if there is only one matching child
    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, "My Calculator[0].tolerance", "3.0")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(task.calculator()->m_tolerance.getVal(), 3.0);

    // index out of range
    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "Solver[2].tolerance", "4.0")
            .isEmpty());
}

TEST(console_property_overrides, uuid_object_segment)
{
    OverrideTestTask task;

    OverrideTestSolver* secondSolver = task.solverAt(1);
    const QString uuid = uuidSegment(secondSolver->uuid());

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, QStringLiteral("%1.tolerance").arg(uuid), "7.0")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(secondSolver->m_tolerance.getVal(), 7.0);

    // a uuid segment can also be used inside an object path
    OverrideTestSolver* nested = task.nestedSolverAt(1);
    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task,
                    QStringLiteral("Solver Group/%1.tolerance")
                        .arg(uuidSegment(nested->uuid())),
                    "6.0")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(nested->m_tolerance.getVal(), 6.0);

    // no implicit UUID detection: the plain uuid (without braces) is no
    // valid object name
    const QString bareUuid = uuid.mid(1, uuid.size() - 2);
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, QStringLiteral("%1.tolerance").arg(bareUuid), "8.0")
                     .isEmpty());
}

TEST(console_property_overrides, nested_object_paths)
{
    OverrideTestTask task;

    // duplicated names are ambiguous at every level of the path
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver Group/Nested Solver.tolerance", "1.5")
                     .isEmpty());

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, "Solver Group/Nested Solver[1].tolerance", "2.5")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(task.nestedSolverAt(1)->m_tolerance.getVal(), 2.5);

    // containers inside a nested object
    OverrideTestSolver* nested = task.nestedSolverAt(0);
    nested->m_points.newEntry("Point");
    nested->m_points.newEntry("Point");
    nested->m_boundaries.newEntry("Boundary", "inlet");

    EXPECT_TRUE(
        gt::console::applyPropertyOverride(
            task, "Solver Group/Nested Solver[0].points[1].pressure", "3.5")
            .isEmpty());
    EXPECT_DOUBLE_EQ(nested->m_points.at(1).getMemberVal<double>("pressure"),
                     3.5);

    EXPECT_TRUE(
        gt::console::applyPropertyOverride(
            task, "Solver Group/Nested Solver[0].boundaries[{inlet}].pressure",
            "4.5")
            .isEmpty());
    EXPECT_DOUBLE_EQ(
        nested->m_boundaries.at(0).getMemberVal<double>("pressure"), 4.5);
}

TEST(console_property_overrides, deeper_navigation_with_slash)
{
    OverrideTestTask task;

    // "." never navigates objects, so this must fail even though
    // "Inner" is a child of "Solver"
    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "Solver.Inner.someProp", "1")
            .isEmpty());

    EXPECT_TRUE(task.inner() != nullptr);
}

TEST(console_property_overrides, sequential_property_container)
{
    OverrideTestTask task;

    OverrideTestSolver* solver = task.solverAt(0);
    solver->m_points.newEntry("Point");
    solver->m_points.newEntry("Point");
    solver->m_points.newEntry("Point");

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, "Solver[0].points[2].pressure", "420000")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(solver->m_points.at(2).getMemberVal<double>("pressure"),
                     420000.0);

    // index out of range
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver[0].points[3].pressure", "1")
                     .isEmpty());

    // associative selector on a sequential container is rejected
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver[0].points[{foo}].pressure", "1")
                     .isEmpty());
}

TEST(console_property_overrides, associative_property_container)
{
    OverrideTestTask task;

    OverrideTestSolver* solver = task.solverAt(1);
    solver->m_boundaries.newEntry("Boundary", "inlet");

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    task, "Solver[1].boundaries[{inlet}].pressure", "99000")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(
        solver->m_boundaries.at(0).getMemberVal<double>("pressure"), 99000.0);

    // unknown entry id
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver[1].boundaries[{outlet}].pressure", "1")
                     .isEmpty());

    // index selector on an associative container is rejected
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver[1].boundaries[0].pressure", "1")
                     .isEmpty());
}

TEST(console_property_overrides, container_on_root_without_object_path)
{
    OverrideTestTask task;

    // the calculator itself owns no container, but the solvers do; use a
    // task with a container directly on the root by reusing the solver as
    // root object
    OverrideTestSolver solver;
    solver.m_points.newEntry("Point");
    solver.m_points.newEntry("Point");
    solver.m_boundaries.newEntry("Boundary", "inlet");

    EXPECT_TRUE(
        gt::console::applyPropertyOverride(solver, "points[1].pressure", "5.0")
            .isEmpty());
    EXPECT_DOUBLE_EQ(solver.m_points.at(1).getMemberVal<double>("pressure"),
                     5.0);

    EXPECT_TRUE(gt::console::applyPropertyOverride(
                    solver, "boundaries[{inlet}].pressure", "6.0")
                    .isEmpty());
    EXPECT_DOUBLE_EQ(solver.m_boundaries.at(0).getMemberVal<double>("pressure"),
                     6.0);
}

TEST(console_property_overrides, unresolved_paths_are_rejected)
{
    OverrideTestTask task;

    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "doesNotExist.tolerance", "1")
            .isEmpty());

    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "My Calculator.doesNotExist", "1")
                     .isEmpty());

    EXPECT_FALSE(gt::console::applyPropertyOverride(task, "doesNotExist", "1")
                     .isEmpty());

    // a container entry without trailing property id is invalid
    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "Solver[0].points[0]", "1")
            .isEmpty());
}

TEST(console_property_overrides, malformed_paths_are_rejected)
{
    OverrideTestTask task;

    EXPECT_FALSE(gt::console::applyPropertyOverride(task, "", "1").isEmpty());
    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "Solver..tolerance", "1")
            .isEmpty());
    EXPECT_FALSE(
        gt::console::applyPropertyOverride(task, "Solver[0.tolerance", "1")
            .isEmpty());
    EXPECT_FALSE(gt::console::applyPropertyOverride(
                     task, "Solver.tolerance.extra.more", "1")
                     .isEmpty());
}

TEST(console_property_overrides, repeated_overrides_last_value_wins)
{
    OverrideTestTask task;

    QList<gt::console::PropertyOverride> overrides;
    overrides.append(gt::console::PropertyOverride{"iterations", "1"});
    overrides.append(gt::console::PropertyOverride{"iterations", "2"});
    overrides.append(
        gt::console::PropertyOverride{"My Calculator[0].tolerance", "3"});

    EXPECT_TRUE(applyAll(task, overrides).isEmpty());
    EXPECT_EQ(task.m_iterations.getVal(), 2);
    EXPECT_DOUBLE_EQ(task.calculator()->m_tolerance.getVal(), 3.0);
}

TEST(console_property_overrides, override_list_stops_at_first_error)
{
    OverrideTestTask task;

    QList<gt::console::PropertyOverride> overrides;
    overrides.append(gt::console::PropertyOverride{"iterations", "5"});
    overrides.append(gt::console::PropertyOverride{"doesNotExist", "1"});
    overrides.append(gt::console::PropertyOverride{"iterations", "9"});

    EXPECT_FALSE(applyAll(task, overrides).isEmpty());

    // the first override was applied, the last one was not executed
    EXPECT_EQ(task.m_iterations.getVal(), 5);
}
