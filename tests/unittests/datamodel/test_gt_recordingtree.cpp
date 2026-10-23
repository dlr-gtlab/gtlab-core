/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_recording.h"

#include <memory>

namespace
{

/// Observer that additionally stores the child's UUID at attach time
class UuidRecordingObserver : public RecordingTestObserver
{
public:
    QString attachedUuid;

    void objectAttached(const GtObject& parent, const GtObject& child) override
    {
        attachedUuid = child.uuid();
        RecordingTestObserver::objectAttached(parent, child);
    }
};

template <typename Work>
void
record(RecordingTestObserver& observer, QList<const GtObject*> roots,
       Work&& work)
{
    GtObject component;
    GtRecordingSession session{component, observer, roots};
    session.run([&]() {
        work(session);
        return true;
    });
}

QStringList
eventKinds(const RecordingTestObserver& observer)
{
    QStringList kinds = observer.kinds();
    kinds.removeAll(QStringLiteral("started"));
    kinds.removeAll(QStringLiteral("finished"));
    return kinds;
}

} // namespace

TEST(GtRecordingTree, constructorTimeAttachIsReportedAndObserved)
{
    RecordingTestObject root;
    UuidRecordingObserver observer;
    RecordingTestObject* created = nullptr;

    record(observer, roots(&root), [&](GtRecordingSession& s) {
        created = new RecordingTestObject(&root);
        EXPECT_TRUE(s.isObserved(*created));
        (void)created->value.getVal();
    });

    const auto entries = observer.entries();
    // the derived constructor's registration does not emit, the read does
    ASSERT_EQ(eventKinds(observer), (QStringList{"attached", "read"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].child, created);
    // the UUID is assigned after attaching
    EXPECT_TRUE(observer.attachedUuid.isEmpty());
    EXPECT_EQ(entries[2].object, created);
}

TEST(GtRecordingTree, appendingUnrelatedObjectMakesItObserved)
{
    RecordingTestObject root;
    auto* outside = new RecordingTestObject;
    auto* outsideChild = new RecordingTestObject(outside);
    RecordingTestObserver observer;

    record(observer, roots(&root), [&](GtRecordingSession& s) {
        EXPECT_FALSE(s.isObserved(*outsideChild));
        root.appendChild(outside);
        EXPECT_TRUE(s.isObserved(*outside));
        EXPECT_TRUE(s.isObserved(*outsideChild));
        outsideChild->value.setVal(1.0);
    });

    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"attached", "written"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].child, outside);
    EXPECT_EQ(entries[2].object, outsideChild);
}

TEST(GtRecordingTree, movingOutOfObservedTreeStopsObservation)
{
    RecordingTestObject root;
    auto* child = new RecordingTestObject(&root);
    RecordingTestObject unrelated;
    RecordingTestObserver observer;

    record(observer, roots(&root), [&](GtRecordingSession& s) {
        unrelated.appendChild(child);
        EXPECT_FALSE(s.isObserved(*child));
        (void)child->value.getVal();
    });

    const auto entries = observer.entries();
    // detached from the observed root, attached to an unobserved parent
    ASSERT_EQ(eventKinds(observer), (QStringList{"detached"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].child, child);
}

TEST(GtRecordingTree, disconnectFromParentIsReported)
{
    RecordingTestObject root;
    std::unique_ptr<RecordingTestObject> child{new RecordingTestObject(&root)};
    RecordingTestObserver observer;

    record(observer, roots(&root), [&](GtRecordingSession& s) {
        child->disconnectFromParent();
        EXPECT_FALSE(s.isObserved(*child));
        child->value.setVal(1.0);
    });

    ASSERT_EQ(eventKinds(observer), (QStringList{"detached"}));
}

TEST(GtRecordingTree, rootStaysObservedWhenAttachedElsewhere)
{
    RecordingTestObject unrelated;
    auto* root = new RecordingTestObject;
    RecordingTestObserver observer;

    record(observer, roots(root), [&](GtRecordingSession& s) {
        unrelated.appendChild(root);
        EXPECT_TRUE(s.isObserved(*root));
    });

    // the attach to an unobserved parent is not reported
    EXPECT_TRUE(eventKinds(observer).isEmpty());
}

TEST(GtRecordingTree, deletionIsReportedAndForgotten)
{
    RecordingTestObject root;
    auto* child = new RecordingTestObject(&root);
    auto* grandChild = new RecordingTestObject(child);
    RecordingTestObserver observer;
    const GtObject* deleted = child;
    const GtObject* deletedGrandChild = grandChild;

    record(observer, roots(&root), [&](GtRecordingSession& s) {
        delete child;

        // a new object, possibly at the same address, is not observed
        RecordingTestObject fresh;
        EXPECT_FALSE(s.isObserved(fresh));
        (void)fresh.value.getVal();
    });

    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"detached"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].child, deleted);
    EXPECT_NE(entries[1].child, deletedGrandChild);
}

TEST(GtRecordingTree, unrelatedTreeChangesAreIgnored)
{
    RecordingTestObject root;
    RecordingTestObject unrelated;
    RecordingTestObserver observer;

    record(observer, roots(&root), [&](GtRecordingSession&) {
        auto* c = new RecordingTestObject(&unrelated);
        c->disconnectFromParent();
        delete c;
    });

    EXPECT_TRUE(eventKinds(observer).isEmpty());
}

TEST(GtRecordingTree, suspendedParentSessionTracksTreeChanges)
{
    RecordingTestObject parentRoot;
    RecordingTestObject childRoot;
    RecordingTestObserver parentObserver;
    RecordingTestObserver childObserver;
    GtObject component;
    GtRecordingSession parent{component, parentObserver, roots(&parentRoot)};
    GtRecordingSession child{component, childObserver, roots(&childRoot)};
    RecordingTestObject* created = nullptr;

    parent.run([&]() {
        child.run([&]() {
            // created under the parent session's tree while the child runs
            created = new RecordingTestObject(&parentRoot);
            return true;
        });

        EXPECT_TRUE(parent.isObserved(*created));
        (void)created->value.getVal();
        return true;
    });

    // the child session does not observe parentRoot: no events there
    EXPECT_EQ(eventKinds(childObserver), QStringList{});
    // the suspended parent got no event, but observes the new object
    ASSERT_EQ(eventKinds(parentObserver), (QStringList{"read"}));
    EXPECT_EQ(parentObserver.entries()[1].object, created);
}

TEST(GtRecordingTree, deletionInNestedSessionReachesSuspendedParent)
{
    RecordingTestObject parentRoot;
    auto* child = new RecordingTestObject(&parentRoot);
    RecordingTestObject childRoot;
    RecordingTestObserver parentObserver;
    RecordingTestObserver childObserver;
    GtObject component;
    GtRecordingSession parent{component, parentObserver, roots(&parentRoot)};
    GtRecordingSession nested{component, childObserver, roots(&childRoot)};
    const GtObject* deleted = child;

    parent.run([&]() {
        (void)child->value.getVal();
        nested.run([&]() {
            delete child;
            return true;
        });
        EXPECT_FALSE(parent.isObserved(*deleted));
        return true;
    });

    // the nested session does not observe the object
    EXPECT_EQ(eventKinds(childObserver), QStringList{});
    ASSERT_EQ(eventKinds(parentObserver), (QStringList{"read", "detached"}));
    EXPECT_EQ(parentObserver.entries()[2].child, deleted);
}

TEST(GtRecordingTree, deletedObservedRootIsReported)
{
    auto* root = new RecordingTestObject;
    RecordingTestObserver observer;
    const GtObject* deleted = root;

    record(observer, roots(root), [&](GtRecordingSession& s) {
        (void)root->value.getVal();
        delete root;
        EXPECT_FALSE(s.isObserved(*deleted));
    });

    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"read", "destroyed"}));
    EXPECT_EQ(entries[2].object, deleted);
}

TEST(GtRecordingTree, descendantsOfDeletedRootAreNotReportedSeparately)
{
    auto* root = new RecordingTestObject;
    new RecordingTestObject(root);
    RecordingTestObserver observer;

    record(observer, roots(root), [&](GtRecordingSession&) { delete root; });

    ASSERT_EQ(eventKinds(observer), (QStringList{"destroyed"}));
}
