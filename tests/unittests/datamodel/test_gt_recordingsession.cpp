/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_recording.h"

#include <stdexcept>

namespace
{

struct Tree
{
    RecordingTestObject root;
    RecordingTestObject* child{new RecordingTestObject(&root)};
    RecordingTestObject unrelated;
    GtObject component;
};

} // namespace

TEST(GtRecordingSession, noSessionIsActiveByDefault)
{
    EXPECT_EQ(GtRecordingSession::current(), nullptr);
}

TEST(GtRecordingSession, runCallsLifecycleInOrderAndActivatesSession)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    GtRecordingSession* activeInWork = nullptr;
    EXPECT_TRUE(session.run([&]() {
        activeInWork = GtRecordingSession::current();
        return true;
    }));

    EXPECT_EQ(activeInWork, &session);
    EXPECT_EQ(GtRecordingSession::current(), nullptr);
    EXPECT_EQ(observer.kinds(), (QStringList{"started", "finished"}));
    EXPECT_TRUE(observer.entries().last().success);
    EXPECT_FALSE(session.id().isNull());
    EXPECT_TRUE(session.parentId().isNull());
    EXPECT_EQ(&session.component(), &t.component);
}

TEST(GtRecordingSession, failedWorkIsReported)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    EXPECT_FALSE(session.run([]() { return false; }));
    EXPECT_EQ(observer.kinds(), (QStringList{"started", "finished"}));
    EXPECT_FALSE(observer.entries().last().success);
}

TEST(GtRecordingSession, exceptionFinishesRestoresAndRethrows)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    EXPECT_THROW(
        session.run([]() -> bool { throw std::runtime_error("work failed"); }),
        std::runtime_error);

    EXPECT_EQ(GtRecordingSession::current(), nullptr);
    EXPECT_EQ(observer.kinds(), (QStringList{"started", "finished"}));
    EXPECT_FALSE(observer.entries().last().success);
}

TEST(GtRecordingSession, callbacksRunWithoutActiveSession)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    session.run([&]() {
        GtRecordingSession::current()->notifyPropertyRead(t.root, t.root.value);
        return true;
    });

    const auto entries = observer.entries();
    ASSERT_EQ(entries.size(), 3);
    for (const auto& e : entries)
    {
        EXPECT_EQ(e.activeSession, nullptr) << e.kind.toStdString();
    }
}

TEST(GtRecordingSession, eventsAreFilteredByObservedTreeAndMask)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root),
                               GtRecordingSession::PropertyRead};

    session.run([&]() {
        auto* s = GtRecordingSession::current();
        EXPECT_TRUE(s->isObserved(t.root));
        EXPECT_TRUE(s->isObserved(*t.child));
        EXPECT_FALSE(s->isObserved(t.unrelated));

        s->notifyPropertyRead(t.root, t.root.value);
        s->notifyPropertyRead(*t.child, t.child->value);
        s->notifyPropertyRead(t.unrelated, t.unrelated.value);
        // masked out
        s->notifyPropertyWritten(t.root, t.root.value);
        return true;
    });

    const auto entries = observer.entries();
    ASSERT_EQ(observer.kinds(),
              (QStringList{"started", "read", "read", "finished"}));
    EXPECT_EQ(entries[1].object, &t.root);
    EXPECT_EQ(entries[1].property, &t.root.value);
    EXPECT_EQ(entries[2].object, t.child);
    EXPECT_EQ(entries[2].property, &t.child->value);
}

TEST(GtRecordingSession, nestedSessionReplacesAndRestoresParent)
{
    Tree t;
    RecordingTestObserver parentObserver;
    RecordingTestObserver childObserver;
    GtRecordingSession parent{t.component, parentObserver, roots(&t.root)};
    GtRecordingSession child{t.component, childObserver, roots(&t.root)};

    parent.run([&]() {
        child.run([&]() {
            GtRecordingSession::current()->notifyPropertyRead(t.root,
                                                              t.root.value);
            return true;
        });

        EXPECT_EQ(GtRecordingSession::current(), &parent);
        GtRecordingSession::current()->notifyPropertyRead(*t.child,
                                                          t.child->value);
        return true;
    });

    EXPECT_EQ(child.parentId(), parent.id());
    EXPECT_EQ(childObserver.kinds(),
              (QStringList{"started", "read", "finished"}));
    EXPECT_EQ(childObserver.entries()[1].object, &t.root);
    EXPECT_EQ(parentObserver.kinds(),
              (QStringList{"started", "read", "finished"}));
    EXPECT_EQ(parentObserver.entries()[1].object, t.child);
}

TEST(GtRecordingSession, nestedStartAndFinishDoNotReachParent)
{
    Tree t;
    RecordingTestObserver parentObserver;
    RecordingTestObserver childObserver;
    GtRecordingSession parent{t.component, parentObserver, roots(&t.root)};
    GtRecordingSession child{t.component, childObserver, roots(&t.root)};

    parent.run([&]() { return child.run([]() { return true; }); });

    EXPECT_EQ(childObserver.entries()[0].activeSession, nullptr);
    EXPECT_EQ(childObserver.entries()[1].activeSession, nullptr);
    EXPECT_EQ(parentObserver.kinds(), (QStringList{"started", "finished"}));
}

TEST(GtRecordingSession, failingStartRunsWorkUnrecorded)
{
    Tree t;
    RecordingTestObserver parentObserver;
    RecordingTestObserver childObserver;
    childObserver.throwIn = QStringLiteral("started");
    GtRecordingSession parent{t.component, parentObserver, roots(&t.root)};
    GtRecordingSession child{t.component, childObserver, roots(&t.root)};

    bool workRan = false;
    parent.run([&]() {
        return child.run([&]() {
            workRan = true;
            GtRecordingSession::current()->notifyPropertyRead(t.root,
                                                              t.root.value);
            return true;
        });
    });

    EXPECT_TRUE(workRan);
    // no events and no finish for the failed child, nothing leaks to parent
    EXPECT_EQ(childObserver.kinds(), (QStringList{"started"}));
    EXPECT_EQ(parentObserver.kinds(), (QStringList{"started", "finished"}));
}

TEST(GtRecordingSession, failingEventCallbackStopsForwarding)
{
    Tree t;
    RecordingTestObserver observer;
    observer.throwIn = QStringLiteral("read");
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    EXPECT_TRUE(session.run([&]() {
        auto* s = GtRecordingSession::current();
        s->notifyPropertyRead(t.root, t.root.value);
        s->notifyPropertyRead(t.root, t.root.value);
        s->notifyPropertyWritten(t.root, t.root.value);
        return true;
    }));

    EXPECT_EQ(observer.kinds(), (QStringList{"started", "read", "finished"}));
}

TEST(GtRecordingSession, failingFinishDoesNotChangeResult)
{
    Tree t;
    RecordingTestObserver observer;
    observer.throwIn = QStringLiteral("finished");
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    EXPECT_TRUE(session.run([]() { return true; }));
    EXPECT_EQ(GtRecordingSession::current(), nullptr);
}

TEST(GtRecordingSession, sessionIsSingleUse)
{
    Tree t;
    RecordingTestObserver observer;
    GtRecordingSession session{t.component, observer, roots(&t.root)};

    session.run([]() { return true; });

    GtRecordingSession* activeInWork = &session;
    EXPECT_FALSE(session.run([&]() {
        activeInWork = GtRecordingSession::current();
        return false;
    }));

    EXPECT_EQ(activeInWork, nullptr);
    EXPECT_EQ(observer.kinds(), (QStringList{"started", "finished"}));
}
