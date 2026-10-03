/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "internal/gt_modulemetadata.h"

#include "gt_coreapplication.h"
#include "gt_modules.h"
#include "gt_testhelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

#include <initializer_list>
#include <memory>
#include <type_traits>

static_assert(!std::is_constructible<gt::Modules, const GtModuleLoader*>::value,
              "Module access must be obtained through GtCoreApplication");

namespace
{

    using gt::detail::ModuleMetaData;
    using gt::detail::ModuleMetaMap;

    struct TestDependency
    {
        QString name;
        bool isOptional{false};
    };

    /*
 * @brief Builds module meta data from synthetic plugin meta data
 * @param id Module identification string
 * @param deps Direct dependencies of the module
 * @return Module meta data
 */
    ModuleMetaData makeMeta(const QString& id,
                            std::initializer_list<TestDependency> deps = {})
    {
        QJsonArray depArray;
        for (const auto& dep : deps)
        {
            QJsonObject depObj;
            depObj.insert(QStringLiteral("name"), dep.name);
            depObj.insert(QStringLiteral("version"), QString());
            depObj.insert(QStringLiteral("optional"), dep.isOptional);
            depArray.append(depObj);
        }

        QJsonObject metaData;
        metaData.insert(QStringLiteral("dependencies"), depArray);

        QJsonObject json;
        json.insert(QStringLiteral("IID"), id);
        json.insert(QStringLiteral("MetaData"), metaData);

        ModuleMetaData meta;
        meta.readFromJson(json);
        return meta;
    }

    /*
 * @brief Builds a module meta data map from a list of meta data entries
 * @param metas Module meta data entries
 * @return Module meta data map
 */
    ModuleMetaMap makeMap(std::initializer_list<ModuleMetaData> metas)
    {
        ModuleMetaMap map;
        for (const auto& meta : metas)
        {
            map.insert({meta.moduleId(), meta});
        }
        return map;
    }

    QStringList sorted(QStringList list)
    {
        list.removeDuplicates();
        list.sort();
        return list;
    }

    /*
 * @brief Convenience access to the module ids of a dependency closure
 * @param moduleIds Module identification strings to resolve
 * @param map Module meta data map
 * @return Module ids including the transitive dependency closure
 */
    QStringList closureIds(const QStringList& moduleIds,
                           const ModuleMetaMap& map)
    {
        return gt::detail::dependencyClosure(moduleIds, map).moduleIds;
    }

    /*
 * @brief Convenience access to the unresolved dependencies of a closure
 * @param moduleIds Module identification strings to resolve
 * @param map Module meta data map
 * @return Dependencies that could not be resolved
 */
    QStringList unresolvedDeps(const QStringList& moduleIds,
                               const ModuleMetaMap& map)
    {
        return gt::detail::dependencyClosure(moduleIds, map)
            .unresolvedDependencies;
    }

} // namespace

TEST(GtModuleMetaData, readsIdAndDependenciesFromJson)
{
    const auto meta = makeMeta(QStringLiteral("ModuleA"),
                               {{QStringLiteral("ModuleB"), false},
                                {QStringLiteral("ModuleC"), true}});

    EXPECT_EQ(meta.moduleId(), QStringLiteral("ModuleA"));

    ASSERT_EQ(meta.directDependencies().size(), 2U);
    EXPECT_EQ(meta.directDependencies()[0].name, QStringLiteral("ModuleB"));
    EXPECT_FALSE(meta.directDependencies()[0].optional());
    EXPECT_EQ(meta.directDependencies()[1].name, QStringLiteral("ModuleC"));
    EXPECT_TRUE(meta.directDependencies()[1].optional());
}

TEST(GtModuleMetaData, matchesDependencySupportsLiteralAndRegexPatterns)
{
    using gt::detail::matchesDependency;

    EXPECT_TRUE(matchesDependency(QStringLiteral("ModuleA"),
                                  QStringLiteral("ModuleA")));
    EXPECT_FALSE(matchesDependency(QStringLiteral("ModuleA"),
                                   QStringLiteral("ModuleAB")));

    EXPECT_TRUE(matchesDependency(QStringLiteral("regex:Module.*"),
                                  QStringLiteral("ModuleAB")));
    EXPECT_FALSE(matchesDependency(QStringLiteral("regex:Module.*"),
                                   QStringLiteral("Other")));

    // literal patterns are not interpreted as regex
    EXPECT_FALSE(matchesDependency(QStringLiteral("ModuleA.*"),
                                   QStringLiteral("ModuleAB")));
}

TEST(GtModuleMetaData, getMatchedModuleIdsResolvesPatternsAgainstKnownModules)
{
    const auto map = makeMap({makeMeta(QStringLiteral("ModuleA")),
                              makeMeta(QStringLiteral("ModuleB")),
                              makeMeta(QStringLiteral("Other"))});

    EXPECT_EQ(gt::detail::getMatchedModuleIds(QStringLiteral("ModuleA"), map),
              QStringList{QStringLiteral("ModuleA")});
    EXPECT_EQ(sorted(gt::detail::getMatchedModuleIds(
                  QStringLiteral("regex:Module.*"), map)),
              sorted(QStringList{QStringLiteral("ModuleA"),
                                 QStringLiteral("ModuleB")}));
    EXPECT_TRUE(gt::detail::getMatchedModuleIds(QStringLiteral("Missing"), map)
                    .isEmpty());
}

TEST(GtModuleMetaData, dependencyClosureResolvesTransitiveDependencies)
{
    const auto map =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")}}),
                 makeMeta(QStringLiteral("B"), {{QStringLiteral("C")}}),
                 makeMeta(QStringLiteral("C")), makeMeta(QStringLiteral("D"))});

    EXPECT_EQ(closureIds({QStringLiteral("A")}, map),
              (QStringList{QStringLiteral("A"), QStringLiteral("B"),
                           QStringLiteral("C")}));

    EXPECT_EQ(closureIds({QStringLiteral("B")}, map),
              (QStringList{QStringLiteral("B"), QStringLiteral("C")}));

    EXPECT_EQ(closureIds({QStringLiteral("C")}, map),
              QStringList{QStringLiteral("C")});

    EXPECT_EQ(closureIds({QStringLiteral("A"), QStringLiteral("D")}, map),
              (QStringList{QStringLiteral("A"), QStringLiteral("B"),
                           QStringLiteral("C"), QStringLiteral("D")}));

    // the complete closure could be resolved
    EXPECT_FALSE(gt::detail::dependencyClosure({QStringLiteral("A")}, map)
                     .isIncomplete());
}

TEST(GtModuleMetaData, dependencyClosureKeepsMissingRequiredDependencies)
{
    const auto map =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")}}),
                 makeMeta(QStringLiteral("B"), {{QStringLiteral("Missing")}})});

    // the meta data of "Missing" is not available. Its own dependencies
    // cannot be resolved, but the module itself must not be dropped
    EXPECT_EQ(closureIds({QStringLiteral("A")}, map),
              (QStringList{QStringLiteral("A"), QStringLiteral("B"),
                           QStringLiteral("Missing")}));

    // the closure is incomplete, because the dependencies of "Missing"
    // are unknown
    EXPECT_EQ(unresolvedDeps({QStringLiteral("A")}, map),
              QStringList{QStringLiteral("Missing")});
}

TEST(GtModuleMetaData, dependencyClosureHandlesOptionalDependencies)
{
    const auto withoutOptional =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")},
                                                {QStringLiteral("Opt"), true}}),
                 makeMeta(QStringLiteral("B"))});

    // optional dependencies are only part of the closure if the module
    // is available (module loader semantics). They do not make the
    // closure incomplete.
    EXPECT_EQ(closureIds({QStringLiteral("A")}, withoutOptional),
              (QStringList{QStringLiteral("A"), QStringLiteral("B")}));

    EXPECT_TRUE(
        unresolvedDeps({QStringLiteral("A")}, withoutOptional).isEmpty());

    const auto withOptional =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")},
                                                {QStringLiteral("Opt"), true}}),
                 makeMeta(QStringLiteral("B")),
                 makeMeta(QStringLiteral("Opt"), {{QStringLiteral("OptDep")}}),
                 makeMeta(QStringLiteral("OptDep"))});

    EXPECT_EQ(closureIds({QStringLiteral("A")}, withOptional),
              (QStringList{QStringLiteral("A"), QStringLiteral("B"),
                           QStringLiteral("Opt"), QStringLiteral("OptDep")}));
}

TEST(GtModuleMetaData, dependencyClosureResolvesRegexDependencies)
{
    const auto map = makeMap(
        {makeMeta(QStringLiteral("A"), {{QStringLiteral("regex:B.*")}}),
         makeMeta(QStringLiteral("B1")), makeMeta(QStringLiteral("B2"))});

    EXPECT_EQ(closureIds({QStringLiteral("A")}, map),
              (QStringList{QStringLiteral("A"), QStringLiteral("B1"),
                           QStringLiteral("B2")}));

    EXPECT_TRUE(unresolvedDeps({QStringLiteral("A")}, map).isEmpty());

    // unmatched regex dependencies cannot be materialized to a concrete
    // module id and are therefore not part of the closure. They are
    // reported as unresolved dependencies, because their position in the
    // dependency graph is unknown.
    const auto noMatch = makeMap(
        {makeMeta(QStringLiteral("A"), {{QStringLiteral("regex:Z.*")}})});

    EXPECT_EQ(closureIds({QStringLiteral("A")}, noMatch),
              QStringList{QStringLiteral("A")});

    EXPECT_EQ(unresolvedDeps({QStringLiteral("A")}, noMatch),
              QStringList{QStringLiteral("regex:Z.*")});

    const auto result =
        gt::detail::dependencyClosure({QStringLiteral("A")}, noMatch);
    EXPECT_TRUE(result.isIncomplete());
}

TEST(GtModuleMetaData, dependencyClosureHandlesCycles)
{
    const auto map =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")}}),
                 makeMeta(QStringLiteral("B"), {{QStringLiteral("A")}})});

    EXPECT_EQ(closureIds({QStringLiteral("A")}, map),
              (QStringList{QStringLiteral("A"), QStringLiteral("B")}));
}

TEST(GtModuleMetaData, dependencyClosurePassesThroughUnknownModules)
{
    // the meta data of the unknown module "X" is not available, therefore
    // its dependencies cannot be resolved
    EXPECT_EQ(closureIds({QStringLiteral("X")}, {}),
              QStringList{QStringLiteral("X")});

    EXPECT_EQ(unresolvedDeps({QStringLiteral("X")}, {}),
              QStringList{QStringLiteral("X")});

    EXPECT_TRUE(gt::detail::dependencyClosure({}, {}).moduleIds.isEmpty());
    EXPECT_FALSE(gt::detail::dependencyClosure({}, {}).isIncomplete());
}

TEST(GtModuleMetaData, missingRootsAreUnresolvedAlongsideKnownGraphs)
{
    const auto map =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")}}),
                 makeMeta(QStringLiteral("B"))});
    const auto result = gt::detail::dependencyClosure(
        {QStringLiteral("X"), QStringLiteral("A"), QStringLiteral("X")}, map);
    EXPECT_EQ(result.moduleIds,
              (QStringList{QStringLiteral("A"), QStringLiteral("B"),
                           QStringLiteral("X")}));
    EXPECT_EQ(result.unresolvedDependencies, QStringList{QStringLiteral("X")});
    EXPECT_TRUE(result.isIncomplete());
}

TEST(GtModuleMetaData, createAdjacencyMatrixBuildsGraphOfLoadedModules)
{
    const auto map =
        makeMap({makeMeta(QStringLiteral("A"), {{QStringLiteral("B")}}),
                 makeMeta(QStringLiteral("B")), makeMeta(QStringLiteral("D"))});

    const auto matrix =
        gt::detail::createAdjacencyMatrix({QStringLiteral("A")}, map);

    ASSERT_EQ(matrix.count(QStringLiteral("A")), 1U);
    EXPECT_EQ(matrix.at(QStringLiteral("A")), QStringList{QStringLiteral("B")});
    ASSERT_EQ(matrix.count(QStringLiteral("B")), 1U);
    EXPECT_TRUE(matrix.at(QStringLiteral("B")).isEmpty());
    EXPECT_EQ(matrix.count(QStringLiteral("D")), 0U);
}

namespace
{

    /*
 * Fixture: batch application loading real plugin binaries via the public API.
 */
    class GtModulesTest : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            m_previousModuleDirs = qgetenv("GTLAB_MODULE_DIRS");
            const QString moduleDirs =
                QFileInfo(QString::fromUtf8(FOOTPRINT_ModuleA_PATH))
                    .absolutePath() +
                QDir::listSeparator() +
                QFileInfo(QString::fromUtf8(FOOTPRINT_ModuleB_PATH))
                    .absolutePath();
            qputenv("GTLAB_MODULE_DIRS", moduleDirs.toUtf8());
            m_previousConfigHome = qgetenv("XDG_CONFIG_HOME");
            qputenv("XDG_CONFIG_HOME",
                    gtTestHelper->newTempDir().absolutePath().toUtf8());

            m_app = std::make_unique<GtCoreApplication>(
                QCoreApplication::instance(),
                GtCoreApplication::AppMode::Batch);
            m_app->init();
        }

        void TearDown() override
        {
            m_app.reset();
            if (m_previousModuleDirs.isNull())
                qunsetenv("GTLAB_MODULE_DIRS");
            else
                qputenv("GTLAB_MODULE_DIRS", m_previousModuleDirs);

            if (m_previousConfigHome.isNull())
            {
                qunsetenv("XDG_CONFIG_HOME");
            }
            else
            {
                qputenv("XDG_CONFIG_HOME", m_previousConfigHome);
            }
        }

        std::unique_ptr<GtCoreApplication> m_app;
        QByteArray m_previousConfigHome;
        QByteArray m_previousModuleDirs;
    };

} // namespace

TEST_F(GtModulesTest, successfulLoadPublishesMetadataAndFailedLoadsPreserveIt)
{
    // Initialize the real subsystem without loading directory modules.
    EXPECT_FALSE(m_app->loadSingleModule(QStringLiteral("/missing/module")));
    const auto modules = m_app->modules();
    const auto before = modules.requirementsFor({QStringLiteral("ModuleA")});
    EXPECT_EQ(before.moduleIds, QStringList{QStringLiteral("ModuleA")});
    ASSERT_TRUE(before.isComplete());
    EXPECT_TRUE(modules.version(QStringLiteral("ModuleA")).isNull());
    ASSERT_TRUE(m_app->loadSingleModule(
        QString::fromUtf8(FOOTPRINT_SuccessfulOverride_PATH)));
    const auto overridden =
        modules.requirementsFor({QStringLiteral("ModuleA")});
    EXPECT_EQ(overridden.moduleIds, (QStringList{QStringLiteral("ModuleA"),
                                                 QStringLiteral("ModuleB")}));
    ASSERT_TRUE(overridden.isComplete());

    // These are real plugin binaries with valid metadata. Their required
    // dependency is unavailable, so loading fails after reading metadata.
    EXPECT_FALSE(m_app->loadSingleModule(
        QString::fromUtf8(FOOTPRINT_FailedOverride_PATH)));
    const auto after = modules.requirementsFor({QStringLiteral("ModuleA")});
    EXPECT_EQ(after.moduleIds, overridden.moduleIds);
    EXPECT_EQ(after.unresolved, overridden.unresolved);
    EXPECT_EQ(modules.version(QStringLiteral("ModuleA")),
              GtVersionNumber(1, 2, 3));

    EXPECT_FALSE(
        m_app->loadSingleModule(QString::fromUtf8(FOOTPRINT_FailedNew_PATH)));
    const auto missing = modules.requirementsFor({QStringLiteral("FailedNew")});
    EXPECT_EQ(missing.moduleIds, QStringList{QStringLiteral("FailedNew")});
    EXPECT_EQ(missing.unresolved, QStringList{QStringLiteral("FailedNew")});
    EXPECT_TRUE(modules.version(QStringLiteral("FailedNew")).isNull());
}

TEST(GtModules, withoutModuleSubsystemRequirementsAreUnresolved)
{
    const gt::Modules modules;

    EXPECT_TRUE(modules.version(QStringLiteral("ModuleA")).isNull());

    const auto requirements = modules.requirementsFor(
        {QStringLiteral("ModuleA"), QStringLiteral("ModuleA")});

    EXPECT_EQ(requirements.moduleIds, QStringList{QStringLiteral("ModuleA")});
    EXPECT_EQ(requirements.unresolved, QStringList{QStringLiteral("ModuleA")});
    EXPECT_FALSE(requirements.isComplete());

    EXPECT_TRUE(modules.requirementsFor({}).isComplete());
}
