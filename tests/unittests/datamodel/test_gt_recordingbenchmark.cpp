/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

/*
 * Timing and memory comparison for opt-in recording (issue #1600).
 *
 * Disabled by default; run with
 *   GTlabUnitTest --gtest_also_run_disabled_tests
 *                 --gtest_filter=*RecordingBenchmark*
 *
 * The file also compiles without the recording API, so the same benchmark
 * can be run on a baseline build. Results are printed, not asserted.
 */

#include "gtest/gtest.h"

#include "test_gt_recordingbenchmark.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"

#if __has_include("gt_recordingsession.h")
  #define GT_HAS_RECORDING 1
  #include "gt_recordingobserver.h"
  #include "gt_recordingsession.h"
  #include <future>
  #include <thread>
#endif

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <functional>
#include <vector>

#ifdef _WIN32
  #ifndef NOMINMAX
    #define NOMINMAX
  #endif
  #include <windows.h>
  #include <psapi.h>
  #pragma comment(lib, "psapi.lib")
#endif

namespace
{

volatile double g_sink = 0.0;

/// Private memory of the process in bytes (0 if unknown)
std::size_t
privateMemory()
{
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX counters{};
    if (GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),
            sizeof(counters)))
    {
        return counters.PrivateUsage;
    }
#endif
    return 0;
}

/// Best of several runs in nanoseconds per iteration
double
nsPerIteration(long iterations, const std::function<void(long)>& work)
{
    double best = 1e300;
    for (int run = 0; run < 5; ++run)
    {
        const auto start = std::chrono::steady_clock::now();
        work(iterations);
        const auto end = std::chrono::steady_clock::now();
        const double ns =
            std::chrono::duration<double, std::nano>(end - start).count();
        best = std::min(best, ns / iterations);
    }
    return best;
}

void
report(const char* scenario, const char* operation, double ns)
{
    std::printf("[benchmark] %-28s %-22s %8.2f ns/op\n", scenario, operation,
                ns);
}

/// Measures reads, variant reads and writes on one registered property
void
measureAccess(const char* scenario, RecordingBenchmarkObject& obj)
{
    // read through a volatile pointer so that loads cannot be hoisted
    GtDoubleProperty* volatile prop = obj.props[0].get();

    report(scenario, "getVal()", nsPerIteration(20000000, [&](long n) {
               double sum = 0.0;
               for (long i = 0; i < n; ++i)
                   sum += prop->getVal();
               g_sink = sum;
           }));

    report(scenario, "valueToVariant()", nsPerIteration(2000000, [&](long n) {
               GtAbstractProperty* abstractProp = prop;
               double sum = 0.0;
               for (long i = 0; i < n; ++i)
               {
                   sum += abstractProp->valueToVariant().toDouble();
               }
               g_sink = sum;
           }));

    report(scenario, "setVal()", nsPerIteration(2000000, [&](long n) {
               for (long i = 0; i < n; ++i)
                   prop->setVal(static_cast<double>(i));
           }));
}

#ifdef GT_HAS_RECORDING
class NoOpObserver : public GtRecordingObserver
{
};

template <typename Work>
void
inSession(const GtObject& root, Work&& work)
{
    GtObject component;
    NoOpObserver observer;
    GtRecordingSession session{component, observer,
                               QList<const GtObject*>{&root}};
    session.run([&]() {
        work();
        return true;
    });
}
#endif

} // namespace

TEST(RecordingBenchmark, DISABLED_propertyAccess)
{
    RecordingBenchmarkObject obj;
    RecordingBenchmarkObject unrelated;

    measureAccess("no session", obj);

#ifdef GT_HAS_RECORDING
    {
        // a session runs on another thread; this thread has none
        std::promise<void> measured;
        std::promise<void> running;
        std::thread other([&]() {
            inSession(unrelated, [&]() {
                running.set_value();
                measured.get_future().wait();
            });
        });
        running.get_future().wait();
        measureAccess("session on other thread", obj);
        measured.set_value();
        other.join();
    }

    inSession(unrelated,
              [&]() { measureAccess("session, object unobserved", obj); });

    inSession(obj, [&]() { measureAccess("session, object observed", obj); });
#endif
}

TEST(RecordingBenchmark, DISABLED_objectConstructionAndMemento)
{
    constexpr int count = 100000;
    gtObjectFactory->registerClass(RecordingBenchmarkObject::staticMetaObject);

    std::vector<std::unique_ptr<RecordingBenchmarkObject>> objects;
    objects.reserve(count);

    const std::size_t memBefore = privateMemory();
    const auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < count; ++i)
    {
        objects.push_back(std::make_unique<RecordingBenchmarkObject>());
    }
    const auto end = std::chrono::steady_clock::now();
    const std::size_t memAfter = privateMemory();

    const double constructMs =
        std::chrono::duration<double, std::milli>(end - start).count();
    std::printf("[benchmark] construct %d objects (10 props): %8.1f ms, "
                "%7.1f bytes/object\n",
                count, constructMs,
                static_cast<double>(memAfter - memBefore) / count);

    // memento round trip of a tree as a stand-in for project loading
    RecordingBenchmarkObject root;
    for (int i = 0; i < 20000; ++i)
    {
        root.appendChild(objects[i].release());
    }
    GtObjectMemento memento = root.toMemento();

    double bestMs = 1e300;
    for (int run = 0; run < 3; ++run)
    {
        const auto s = std::chrono::steady_clock::now();
        std::unique_ptr<GtObject> restored{memento.restore(gtObjectFactory)};
        const auto e = std::chrono::steady_clock::now();
        ASSERT_TRUE(restored);
        bestMs = std::min(
            bestMs, std::chrono::duration<double, std::milli>(e - s).count());
    }
    std::printf("[benchmark] restore memento with 20000 objects: %8.1f ms\n",
                bestMs);

    gtObjectFactory->unregisterClass(
        RecordingBenchmarkObject::staticMetaObject);
}
