/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_objectmementodiff_bench
 *
 *  MementoDiffBench - A/B benchmark cases for the memento comparison
 *  implementation. Kept out of the regular unit tests (GTlabUnitTest)
 *  so they stay fast; build and run on demand:
 *    ./build/GTlabProfilingBench --gtest_filter=MementoDiffBench.*
 */

#include "gtest/gtest.h"

#include "gt_objectmementodiff.h"
#include "gt_objectmemento.h"
#include "gt_object.h"
#include "test_gt_object.h"
#include "gt_objectfactory.h"

#include <QUuid>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <vector>

// =====================================================================
// MementoDiffBench - A/B benchmark for memento comparison
//
// Compares two memento comparison strategies for defined cases:
//
//   hash_decision : calculateHashes() + fullHash() comparison. This is the
//                   decision path of the old hash-based implementation.
//                   The code is identical in every build and serves as the
//                   reference measurement.
//   diff_api      : GtObjectMementoDiff construction, i.e. the production
//                   path. Baseline build runs the old algorithm there, the
//                   fastpath build the byte-identical implementation.
//
// Cases:
//   identical    - equal mementos, cold (fresh pair, no cached hashes) and
//                  warm (repeated comparison of the same instances - the
//                  case where the hash cache benefits the old
//                  implementation)
//   deepChange   - one small change in a node deep in the tree
//   manyChanges  - about 10 % of all nodes changed
//   container*   - struct property container entries, one or many entries
//                  changed
//
// Each case reports the diff API in three cache states: cold (fresh pair
// per iteration), warm (same instances on both sides, repeated diff) and
// left-warm (the same left memento instance is diffed on every iteration
// while the right memento is fresh, created outside the timed region).
// Left-warm models a long-lived snapshot re-diffed against a fresh state.
//
// Production relevance of the warm variants: an audit of all
// GtObjectMementoDiff call sites in src/ (console run from memento,
// memento editor drag & drop, command/undo-redo, process dock connection
// editor, import menu, task runner, remote process runner collect,
// finished process loading) shows that every production comparison is
// one-shot with fresh mementos on both sides. The only long-lived
// mementos (the remote process runner's initial task and data snapshots)
// are diffed exactly once per task run, and toMemento() does not populate
// the hash cache, so even there both sides start cold. Re-diffing the same
// instances, i.e. the case where the old implementation's cached hashes
// make repeated comparisons cheap, does not occur in production today.
// The warm numbers document that fast path; if future code re-diffs a
// stable memento tree, a per-instance fullHash cache could be added to
// the fastpath implementation to recover it. The left-warm variant - one
// side reused, the other fresh per iteration - is likewise not exercised
// by any production call site today; it quantifies what a long-lived left
// memento re-diffed per cycle would cost and how each implementation
// reacts to the reused side.
//
// Sizes: small tree = 100 nodes, large tree = 500 nodes (with a wider list
// payload), containers = 500 / 2000 entries.
//
// Machine readable result lines (for A/B joining):
//   BENCH|<case>|payload=<n>|iters=<k>|<variant>|<us per call>us
//
// Environment knobs (for later profiling runs, e.g. valgrind):
//   GT_MEMENTO_BENCH_ITER_MULT - multiplies the cold pair count and the
//                                warm iteration budget (default 1)
//   GT_MEMENTO_BENCH_NODE_MULT - multiplies the tree/entry sizes
//                                (default 1)
//
// Budget: every test stays well below 30 s.
// =====================================================================

namespace {

using SteadyClock = std::chrono::steady_clock;

double
benchUs(SteadyClock::time_point from, SteadyClock::time_point to)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(to - from).count()
           / 1000.0;
}

long long
benchEnvMult(const char* key, long long fallback)
{
    const char* raw = std::getenv(key);
    if (raw == nullptr)
        return fallback;
    char* end = nullptr;
    const long long value = std::strtoll(raw, &end, 10);
    return (end != nullptr && *end == '\0' && value > 0) ? value : fallback;
}

template <typename Fn>
double
benchMeanUsPerCall(int warmup, int iterations, Fn fn)
{
    for (int i = 0; i < warmup; ++i)
        fn();

    size_t acc = 0;
    const auto t0 = SteadyClock::now();
    for (int i = 0; i < iterations; ++i)
        acc += fn();
    const auto t1 = SteadyClock::now();

    volatile size_t sink = acc; // prevents dead code elimination
    Q_UNUSED(sink);

    return benchUs(t0, t1) / double(iterations);
}

// Warm benchmark: a probe call determines how many iterations fit into the
// budget, then the mean is measured over those iterations.
template <typename Fn>
double
benchWarmUsPerCall(int warmup, double budgetUs, int minIters, int maxIters,
                   int& usedIters, Fn fn)
{
    for (int i = 0; i < warmup; ++i)
        fn();

    size_t acc = 0;
    const auto p0 = SteadyClock::now();
    acc = fn();
    const auto p1 = SteadyClock::now();
    volatile size_t sink = acc;
    Q_UNUSED(sink);

    const double probeUs = std::max(benchUs(p0, p1), 1.0);
    usedIters = qBound<int>(minIters, int(budgetUs / probeUs), maxIters);
    return benchMeanUsPerCall(0, usedIters, fn);
}

// Warm benchmark for the one-sided warm case: the same left memento
// instance is diffed on every iteration (any state it gained from earlier
// diffs persists), while prepareRight() creates a fresh right memento on
// every iteration - outside the timed region, mirroring diff_api_cold,
// where pair creation is not timed. The mean is taken over the diff
// construction only.
template <typename PrepareRight>
double
benchWarmLeftWarmUsPerCall(int warmup, double budgetUs, int minIters,
                           int maxIters, int& usedIters,
                           const GtObjectMemento& left,
                           PrepareRight prepareRight)
{
    for (int i = 0; i < warmup; ++i)
    {
        const GtObjectMemento right = prepareRight();
        GtObjectMementoDiff diff(left, right);
        volatile size_t sink =
            diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
        Q_UNUSED(sink);
    }

    // probe one diff against a fresh right memento to size the budget
    const GtObjectMemento probeRight = prepareRight();
    const auto p0 = SteadyClock::now();
    {
        GtObjectMementoDiff diff(left, probeRight);
        volatile size_t sink =
            diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
        Q_UNUSED(sink);
    }
    const auto p1 = SteadyClock::now();
    const double probeUs = std::max(benchUs(p0, p1), 1.0);
    usedIters = qBound<int>(minIters, int(budgetUs / probeUs), maxIters);

    double sumUs = 0.0;
    size_t acc = 0;
    for (int i = 0; i < usedIters; ++i)
    {
        const GtObjectMemento right = prepareRight();
        const auto d0 = SteadyClock::now();
        GtObjectMementoDiff diff(left, right);
        const auto d1 = SteadyClock::now();
        acc += diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
        sumUs += benchUs(d0, d1);
    }
    volatile size_t sink = acc;
    Q_UNUSED(sink);
    return sumUs / double(usedIters);
}

void
benchEmit(const char* caseName, long long payload, long long iters,
          const char* variant, double usPerCall)
{
    std::ostringstream os;
    os << "BENCH|" << caseName << "|payload=" << payload << "|iters=" << iters
       << '|' << variant << '|' << std::fixed << std::setprecision(3)
       << usPerCall << "us";
    std::cout << os.str() << std::endl;
}

// Correctness check: both implementations must detect the same changes.
void
benchCheckDiff(const char* caseName, const GtObjectMemento& left,
               const GtObjectMemento& right, bool expectChanges,
               const QString& expectedToken)
{
    GtObjectMementoDiff diff(left, right);
    const QByteArray xml = diff.toByteArray();
    if (expectChanges)
    {
        EXPECT_FALSE(diff.isNull()) << caseName;
        EXPECT_GT(diff.numberOfDiffSteps(), 0) << caseName;
        if (!expectedToken.isEmpty())
            EXPECT_TRUE(xml.contains(expectedToken.toUtf8())) << caseName;
    }
    else
    {
        EXPECT_TRUE(diff.isNull()) << caseName;
        EXPECT_EQ(diff.numberOfDiffSteps(), 0) << caseName;
    }
}

// ------------------------------------------------------------------
// Tree builder: TestSpecialGtObject nodes (~14 properties each,
// including sub-properties, list properties and member vectors)
// ------------------------------------------------------------------

void
configureBenchNode(TestSpecialGtObject* node, int id, int listLen)
{
    node->setBool(id % 2 == 0);
    node->setDouble(id * 0.5);
    node->setInt(id);
    node->setLabel(QString("label%1").arg(id));
    node->setMode(QString("mode%1").arg(id % 7));
    node->setString(QString("str%1").arg(id)); // regex: letters and digits only
    node->setLink(QString("link%1").arg(id));
    node->setFile(QString("file%1.dat").arg(id));
    node->setVariant(QString("variant%1").arg(id));

    QVector<double> list;
    list.reserve(listLen);
    for (int k = 0; k < listLen; ++k)
        list.append(id * 0.125 + k * 0.015625);
    node->m_doubleListProp.setVal(list);

    node->doubleVec.clear();
    for (int k = 0; k < listLen / 2 + 1; ++k)
        node->doubleVec.append(id * 0.5 - k * 0.25);

    node->stringList.clear();
    for (int k = 0; k < 3; ++k)
        node->stringList << QString("s%1_%2").arg(id).arg(k);
}

GtObject*
buildBenchTree(int numNodes, int childrenPerNode, int listLen)
{
    auto* root = new TestSpecialGtObject();
    configureBenchNode(root, 0, listLen);
    root->setUuid(QUuid::createUuid().toString(QUuid::Id128));

    std::vector<TestSpecialGtObject*> frontier;
    frontier.push_back(root);
    size_t head = 0;
    int created = 1;
    while (created < numNodes && head < frontier.size())
    {
        TestSpecialGtObject* parent = frontier[head++];
        for (int c = 0; c < childrenPerNode && created < numNodes; ++c)
        {
            auto* child = new TestSpecialGtObject();
            configureBenchNode(child, created, listLen);
            child->setUuid(QUuid::createUuid().toString(QUuid::Id128));
            if (parent->appendChild(child))
            {
                frontier.push_back(child);
                ++created;
            }
            else
            {
                delete child;
            }
        }
    }
    return root;
}

TestSpecialGtObject*
benchNodeAtDepth(GtObject* root, int depth)
{
    std::function<TestSpecialGtObject* (GtObject*, int)> walk =
        [&](GtObject* node, int d) -> TestSpecialGtObject* {
        if (d == depth)
            return dynamic_cast<TestSpecialGtObject*>(node);
        for (auto* child : node->findDirectChildren())
            if (TestSpecialGtObject* hit = walk(child, d + 1))
                return hit;
        return nullptr;
    };
    return walk(root, 0);
}

void
applyDeepBenchChange(GtObject* root)
{
    TestSpecialGtObject* target = benchNodeAtDepth(root, 3);
    if (target == nullptr)
        target = dynamic_cast<TestSpecialGtObject*>(root);
    if (target == nullptr)
        return;
    target->setDouble(target->getDouble() + 0.001);
    target->setString(QString("str%1x").arg(target->getInt()));
}

int
applyManyBenchChanges(GtObject* root)
{
    int changed = 0;
    int index = 0;
    std::function<void(GtObject*)> walk = [&](GtObject* node) {
        if (index++ % 10 == 5)
        {
            if (TestSpecialGtObject* t = dynamic_cast<TestSpecialGtObject*>(node))
            {
                t->setBool(!t->getBool());
                t->setInt(t->getInt() + 1);
                t->setLabel(QString("label%1").arg(t->getInt()) + "chg");
                ++changed;
            }
        }
        for (auto* child : node->findDirectChildren())
            walk(child);
    };
    walk(root);
    return changed;
}

struct BenchPair
{
    GtObject* tree = nullptr;
    GtObjectMemento left;
    GtObjectMemento right;

    ~BenchPair()
    {
        delete tree;
    }
};

BenchPair
makeBenchPair(int numNodes, int childrenPerNode, int listLen, int changeMode)
{
    BenchPair pair;
    pair.tree = buildBenchTree(numNodes, childrenPerNode, listLen);
    pair.left = pair.tree->toMemento();
    if (changeMode == 1)
        applyDeepBenchChange(pair.tree);
    else if (changeMode == 2)
        applyManyBenchChanges(pair.tree);
    pair.right = pair.tree->toMemento();
    return pair;
}

struct BenchTreeConfig
{
    int nodes;
    int childrenPerNode;
    int listLen;
};

void
runTreeBench(const char* caseName, BenchTreeConfig cfg, int changeMode,
             const QString& expectedToken)
{
    const long long iterMult = benchEnvMult("GT_MEMENTO_BENCH_ITER_MULT", 1);
    cfg.nodes *= int(benchEnvMult("GT_MEMENTO_BENCH_NODE_MULT", 1));

    const int coldPairs = int(50 * iterMult);
    const double warmBudgetUs = 3.0e6 * double(iterMult);

    // correctness on one representative pair
    {
        BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                       cfg.listLen, changeMode);
        EXPECT_FALSE(pair.left.isNull());
        EXPECT_FALSE(pair.right.isNull());
        benchCheckDiff(caseName, pair.left, pair.right, changeMode != 0,
                       expectedToken);
    }

    // hash decision, cold: fresh pair per measurement, no cached hashes
    {
        std::vector<double> times;
        times.reserve(coldPairs);
        size_t acc = 0;
        for (int i = 0; i < coldPairs; ++i)
        {
            BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                           cfg.listLen, changeMode);
            const auto t0 = SteadyClock::now();
            pair.left.calculateHashes();
            pair.right.calculateHashes();
            acc += (pair.left.fullHash() == pair.right.fullHash()) ? 1 : 0;
            const auto t1 = SteadyClock::now();
            times.push_back(benchUs(t0, t1));
        }
        volatile size_t sink = acc;
        Q_UNUSED(sink);
        benchEmit(caseName, cfg.nodes, coldPairs, "hash_decision_cold",
                  std::accumulate(times.begin(), times.end(), 0.0) / times.size());
    }

    // hash decision, warm: same instances, hashes cached after warmup
    {
        BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                       cfg.listLen, changeMode);
        int iters = 0;
        const double mean = benchWarmUsPerCall(
            3, warmBudgetUs, 100, 100000, iters, [&pair]() -> size_t {
                pair.left.calculateHashes();
                pair.right.calculateHashes();
                return (pair.left.fullHash() == pair.right.fullHash()) ? 1 : 0;
            });
        benchEmit(caseName, cfg.nodes, iters, "hash_decision_warm", mean);
    }

    // diff api, cold: fresh pair per measurement
    {
        std::vector<double> times;
        times.reserve(coldPairs);
        size_t acc = 0;
        for (int i = 0; i < coldPairs; ++i)
        {
            BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                           cfg.listLen, changeMode);
            const auto t0 = SteadyClock::now();
            GtObjectMementoDiff diff(pair.left, pair.right);
            acc += diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
            const auto t1 = SteadyClock::now();
            times.push_back(benchUs(t0, t1));
        }
        volatile size_t sink = acc;
        Q_UNUSED(sink);
        benchEmit(caseName, cfg.nodes, coldPairs, "diff_api_cold",
                  std::accumulate(times.begin(), times.end(), 0.0) / times.size());
    }

    // diff api, warm: same instances, repeated diff construction.
    // No production call site re-diffs the same instances (all pairs are
    // one-shot, see "Production relevance of the warm variants" above);
    // this case documents the hash cache fast path of the old
    // implementation.
    {
        BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                       cfg.listLen, changeMode);
        int iters = 0;
        const double mean = benchWarmUsPerCall(
            3, warmBudgetUs, 100, 20000, iters, [&pair]() -> size_t {
                GtObjectMementoDiff diff(pair.left, pair.right);
                return diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
            });
        benchEmit(caseName, cfg.nodes, iters, "diff_api_warm", mean);
    }

    // diff api, left warm: the same left memento instance is diffed on
    // every iteration while a fresh right memento is taken from the same
    // tree state each iteration (created outside the timed region,
    // mirroring diff_api_cold). The fresh memento carries the same uuids
    // and property values as the original right side, but no cached
    // hashes, so only the left side may benefit from hash caching.
    // Models a long-lived snapshot re-diffed against a fresh state; no
    // production call site does this today (see "Production relevance of
    // the warm variants").
    {
        GtObject* leftTree =
            buildBenchTree(cfg.nodes, cfg.childrenPerNode, cfg.listLen);
        GtObjectMemento left = leftTree->toMemento();
        if (changeMode == 1)
            applyDeepBenchChange(leftTree);
        else if (changeMode == 2)
            applyManyBenchChanges(leftTree);

        // correctness on the exact left-warm setup
        benchCheckDiff(caseName, left, leftTree->toMemento(), changeMode != 0,
                       expectedToken);

        int iters = 0;
        const double mean = benchWarmLeftWarmUsPerCall(
            3, warmBudgetUs, 100, 20000, iters, left,
            [leftTree]() { return leftTree->toMemento(); });
        benchEmit(caseName, cfg.nodes, iters, "diff_api_left_warm", mean);
        delete leftTree;
    }
}

// ------------------------------------------------------------------
// Property container bench: TestObject root with struct entries
// ------------------------------------------------------------------

struct BenchContainerPair
{
    TestObject* obj = nullptr;
    GtObjectMemento left;
    GtObjectMemento right;

    ~BenchContainerPair()
    {
        delete obj;
    }
};

TestObject*
buildBenchContainer(int entries)
{
    auto* obj = new TestObject();
    for (int i = 0; i < entries; ++i)
        obj->addEnvironmentVar(QString("var%1").arg(i),
                               QString("value%1_").arg(i) + QString(16, 'x'));
    return obj;
}

void
applyContainerBenchChange(TestObject* obj, int entries, int changeMode)
{
    if (changeMode == 1)
        obj->environmentVars.at(entries / 2).setMemberVal(
            "value", QString("CHANGED_%1").arg(entries / 2));
    else if (changeMode == 2)
        for (int i = 0; i < entries; ++i)
            if (i % 10 == 3)
                obj->environmentVars.at(i).setMemberVal(
                    "value", QString("CHANGED_%1").arg(i));
}

BenchContainerPair
makeContainerPair(int entries, int changeMode)
{
    BenchContainerPair pair;
    pair.obj = buildBenchContainer(entries);
    pair.left = pair.obj->toMemento();
    applyContainerBenchChange(pair.obj, entries, changeMode);
    pair.right = pair.obj->toMemento();
    return pair;
}

void
runContainerBench(const char* caseName, int entries, int changeMode)
{
    const long long iterMult = benchEnvMult("GT_MEMENTO_BENCH_ITER_MULT", 1);
    entries *= int(benchEnvMult("GT_MEMENTO_BENCH_NODE_MULT", 1));

    const int coldPairs = int(50 * iterMult);
    const double warmBudgetUs = 3.0e6 * double(iterMult);

    // correctness
    {
        BenchContainerPair pair = makeContainerPair(entries, changeMode);
        EXPECT_FALSE(pair.left.isNull());
        EXPECT_FALSE(pair.right.isNull());
        benchCheckDiff(caseName, pair.left, pair.right, changeMode != 0,
                       QStringLiteral("environmentVars"));
    }

    // hash decision, cold: fresh pair per measurement, no cached hashes
    {
        std::vector<double> times;
        times.reserve(coldPairs);
        size_t acc = 0;
        for (int i = 0; i < coldPairs; ++i)
        {
            BenchContainerPair pair = makeContainerPair(entries, changeMode);
            const auto t0 = SteadyClock::now();
            pair.left.calculateHashes();
            pair.right.calculateHashes();
            acc += (pair.left.fullHash() == pair.right.fullHash()) ? 1 : 0;
            const auto t1 = SteadyClock::now();
            times.push_back(benchUs(t0, t1));
        }
        volatile size_t sink = acc;
        Q_UNUSED(sink);
        benchEmit(caseName, entries, coldPairs, "hash_decision_cold",
                  std::accumulate(times.begin(), times.end(), 0.0) / times.size());
    }

    // hash decision, warm: same instances, hashes cached after warmup
    {
        BenchContainerPair pair = makeContainerPair(entries, changeMode);
        int iters = 0;
        const double mean = benchWarmUsPerCall(
            3, warmBudgetUs, 100, 100000, iters, [&pair]() -> size_t {
                pair.left.calculateHashes();
                pair.right.calculateHashes();
                return (pair.left.fullHash() == pair.right.fullHash()) ? 1 : 0;
            });
        benchEmit(caseName, entries, iters, "hash_decision_warm", mean);
    }

    // diff api, cold: fresh pair per measurement
    {
        std::vector<double> times;
        times.reserve(coldPairs);
        size_t acc = 0;
        for (int i = 0; i < coldPairs; ++i)
        {
            BenchContainerPair pair = makeContainerPair(entries, changeMode);
            const auto t0 = SteadyClock::now();
            GtObjectMementoDiff diff(pair.left, pair.right);
            acc += diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
            const auto t1 = SteadyClock::now();
            times.push_back(benchUs(t0, t1));
        }
        volatile size_t sink = acc;
        Q_UNUSED(sink);
        benchEmit(caseName, entries, coldPairs, "diff_api_cold",
                  std::accumulate(times.begin(), times.end(), 0.0) / times.size());
    }

    // diff api, warm: same instances, repeated diff construction
    {
        BenchContainerPair pair = makeContainerPair(entries, changeMode);
        int iters = 0;
        const double mean = benchWarmUsPerCall(
            3, warmBudgetUs, 100, 20000, iters, [&pair]() -> size_t {
                GtObjectMementoDiff diff(pair.left, pair.right);
                return diff.isNull() ? 0 : size_t(diff.numberOfDiffSteps());
            });
        benchEmit(caseName, entries, iters, "diff_api_warm", mean);
    }

    // diff api, left warm: the same left memento instance is diffed on
    // every iteration while a fresh right memento is taken from the same
    // container state each iteration (created outside the timed region,
    // mirroring diff_api_cold). The fresh memento carries the same uuid
    // and property values as the original right side, but no cached
    // hashes, so only the left side may benefit from hash caching.
    // Reduced budget and iteration floor keep the wall time in check for
    // the 2000-entry case.
    {
        TestObject* leftObj = buildBenchContainer(entries);
        GtObjectMemento left = leftObj->toMemento();
        applyContainerBenchChange(leftObj, entries, changeMode);

        // correctness on the exact left-warm setup
        benchCheckDiff(caseName, left, leftObj->toMemento(), changeMode != 0,
                       QStringLiteral("environmentVars"));

        int iters = 0;
        const double mean = benchWarmLeftWarmUsPerCall(
            3, warmBudgetUs / 2.0, 25, 20000, iters, left,
            [leftObj]() { return leftObj->toMemento(); });
        benchEmit(caseName, entries, iters, "diff_api_left_warm", mean);
        delete leftObj;
    }
}

// Fast path cost: the strict areByteIdentical check without any hash
// computation on a byte-identical (unmodified) pair.
// cold: fresh pair per measurement (pair creation not timed), only the
//       comparison is timed; warm: the same pair instance is compared
//       repeatedly.
void
runFastpathCostBench(const char* caseName, BenchTreeConfig cfg)
{
    const long long iterMult = benchEnvMult("GT_MEMENTO_BENCH_ITER_MULT", 1);
    cfg.nodes *= int(benchEnvMult("GT_MEMENTO_BENCH_NODE_MULT", 1));

    const int coldPairs = int(50 * iterMult);
    const double warmBudgetUs = 3.0e6 * double(iterMult);

    // correctness on one representative pair
    {
        BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                       cfg.listLen, 0);
        EXPECT_FALSE(pair.left.isNull());
        EXPECT_FALSE(pair.right.isNull());
        EXPECT_TRUE(GtObjectMementoDiff::areByteIdentical(pair.left,
                                                          pair.right));
        benchCheckDiff(caseName, pair.left, pair.right, false, QString());
    }

    // fast path, cold: fresh pair per measurement
    {
        std::vector<double> times;
        times.reserve(coldPairs);
        size_t acc = 0;
        for (int i = 0; i < coldPairs; ++i)
        {
            BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                           cfg.listLen, 0);
            const auto t0 = SteadyClock::now();
            acc += GtObjectMementoDiff::areByteIdentical(pair.left,
                                                         pair.right) ? 1 : 0;
            const auto t1 = SteadyClock::now();
            times.push_back(benchUs(t0, t1));
        }
        volatile size_t sink = acc;
        Q_UNUSED(sink);
        benchEmit(caseName, cfg.nodes, coldPairs, "fastpath_cold",
                  std::accumulate(times.begin(), times.end(), 0.0)
                  / times.size());
    }

    // fast path, warm: same pair instance compared repeatedly
    {
        BenchPair pair = makeBenchPair(cfg.nodes, cfg.childrenPerNode,
                                       cfg.listLen, 0);
        int iters = 0;
        const double mean = benchWarmUsPerCall(
            3, warmBudgetUs, 100, 100000, iters, [&pair]() -> size_t {
                return GtObjectMementoDiff::areByteIdentical(pair.left,
                                                             pair.right)
                           ? 1 : 0;
            });
        benchEmit(caseName, cfg.nodes, iters, "fastpath_warm", mean);
    }
}

} // anonymous namespace

class MementoDiffBench : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
            gtObjectFactory->registerClass(TestSpecialGtObject::staticMetaObject);
        if (!gtObjectFactory->knownClass("TestObject"))
            gtObjectFactory->registerClass(TestObject::staticMetaObject);
    }
};

// identical mementos: cold (fresh pair, no cached hashes) + warm (same
// instances, repeated comparisons)
TEST_F(MementoDiffBench, identicalSmall)
{
    runTreeBench("identical", {100, 3, 8}, 0, QString());
}

TEST_F(MementoDiffBench, identicalLarge)
{
    runTreeBench("identical", {500, 3, 32}, 0, QString());
}

// one small change in a node at depth 3
TEST_F(MementoDiffBench, deepChangeSmall)
{
    runTreeBench("deepChange", {100, 3, 8}, 1, QStringLiteral("doubleProp"));
}

TEST_F(MementoDiffBench, deepChangeLarge)
{
    runTreeBench("deepChange", {500, 3, 32}, 1, QStringLiteral("doubleProp"));
}

// ~10 % of all nodes changed (bool + int + label)
TEST_F(MementoDiffBench, manyChangesSmall)
{
    runTreeBench("manyChanges", {100, 3, 8}, 2, QStringLiteral("labelProp"));
}

TEST_F(MementoDiffBench, manyChangesLarge)
{
    runTreeBench("manyChanges", {500, 3, 32}, 2, QStringLiteral("labelProp"));
}

// struct property container, one entry changed (of 500)
TEST_F(MementoDiffBench, containerDeep)
{
    runContainerBench("containerDeep", 500, 1);
}

// struct property container, ~10 % of entries changed (of 2000)
TEST_F(MementoDiffBench, containerMany)
{
    runContainerBench("containerMany", 2000, 2);
}

// areByteIdentical fast path on identical mementos: cold + warm
TEST_F(MementoDiffBench, fastpathCostSmall)
{
    runFastpathCostBench("fastpathCost", {100, 3, 8});
}

TEST_F(MementoDiffBench, fastpathCostLarge)
{
    runFastpathCostBench("fastpathCost", {500, 3, 32});
}

