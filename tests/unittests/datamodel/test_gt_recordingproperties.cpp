/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_recording.h"
#include "test_propertycontainerobject.h"

#include "gt_recordinghooks.h"
#include "gt_structproperty.h"

namespace
{

class SilentPropertyObject : public GtObject
{
public:
    SilentPropertyObject() :
        silent(QStringLiteral("silent"), QStringLiteral("Silent"))
    {
        registerSilentProperty(silent);
    }

    GtDoubleProperty silent;
};

/// Runs the work in a session observing the given roots
template <typename Work>
void
record(RecordingTestObserver& observer, QList<const GtObject*> roots,
       Work&& work)
{
    GtObject component;
    GtRecordingSession session{component, observer, roots};
    session.run([&]() {
        work();
        return true;
    });
}

/// Kinds of the events between "started" and "finished"
QStringList
eventKinds(const RecordingTestObserver& observer)
{
    QStringList kinds = observer.kinds();
    kinds.removeAll(QStringLiteral("started"));
    kinds.removeAll(QStringLiteral("finished"));
    return kinds;
}

} // namespace

TEST(GtRecordingProperties, registeredPropertyKnowsItsOwner)
{
    RecordingTestObject obj;
    GtDoubleProperty unregistered(QStringLiteral("u"), QStringLiteral("U"));

    EXPECT_EQ(obj.value.ownerObject(), &obj);
    EXPECT_EQ(unregistered.ownerObject(), nullptr);
}

TEST(GtRecordingProperties, noSessionProducesNoEvents)
{
    RecordingTestObject obj;
    RecordingTestObserver observer;

    EXPECT_EQ(gt::detail::runningRecordings.load(), 0);
    obj.value.setVal(1.0);
    (void)obj.value.getVal();
    (void)static_cast<GtAbstractProperty&>(obj.value).valueToVariant();
    (void)obj.scaledValue();

    EXPECT_TRUE(observer.entries().isEmpty());
}

TEST(GtRecordingProperties, readsAndWritesReachObserverWithOwner)
{
    RecordingTestObject root;
    auto* child = new RecordingTestObject(&root);
    RecordingTestObject unrelated;
    RecordingTestObserver observer;

    record(observer, roots(&root), [&]() {
        (void)root.value.getVal();
        child->value.setVal(2.0);
        (void)unrelated.value.getVal();
        unrelated.value.setVal(3.0);
    });

    auto const entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"read", "written"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].property, &root.value);
    EXPECT_EQ(entries[2].object, child);
    EXPECT_EQ(entries[2].property, &child->value);
}

TEST(GtRecordingProperties, eachAccessorReportsExactlyOneRead)
{
    RecordingTestObject obj;
    const RecordingTestObject& cobj = obj;

    auto readsOf = [&](auto access) {
        RecordingTestObserver observer;
        record(observer, roots(&obj), [&]() { access(); });
        return eventKinds(observer);
    };

    QStringList const oneRead{"read"};
    EXPECT_EQ(readsOf([&]() { (void)obj.value.getVal(); }), oneRead);
    EXPECT_EQ(readsOf([&]() { (void)obj.value.getVal(QString("m")); }),
              oneRead);
    EXPECT_EQ(readsOf([&]() { (void)obj.value.get(); }), oneRead);
    EXPECT_EQ(readsOf([&]() { (void)cobj.value.get(); }), oneRead);
    EXPECT_EQ(readsOf([&]() { (void)obj.value(); }), oneRead);
    EXPECT_EQ(readsOf([&]() { double d = obj.value; (void)d; }), oneRead);
    EXPECT_EQ(readsOf([&]() { (void)static_cast<GtAbstractProperty&>(obj.value).valueToVariant(); }), oneRead);
    EXPECT_EQ(readsOf([&]() { (void)obj.value.value<double>(); }), oneRead);
}

TEST(GtRecordingProperties, mutableReferenceAccessCountsAsRead)
{
    RecordingTestObject obj;
    RecordingTestObserver observer;

    record(observer, roots(&obj), [&]() { obj.value.get() = 5.0; });

    EXPECT_EQ(eventKinds(observer), (QStringList{"read"}));
}

TEST(GtRecordingProperties, settingSameValueCountsAsWrite)
{
    RecordingTestObject obj;
    obj.value.setVal(1.0);
    RecordingTestObserver observer;

    record(observer, roots(&obj), [&]() { obj.value.setVal(1.0); });

    EXPECT_EQ(eventKinds(observer), (QStringList{"written"}));
}

TEST(GtRecordingProperties, subPropertyAddedAfterRegistrationIsRecorded)
{
    RecordingTestObject obj;
    GtDoubleProperty sub(QStringLiteral("sub"), QStringLiteral("Sub"));
    obj.value.registerSubProperty(sub);
    RecordingTestObserver observer;

    EXPECT_EQ(sub.ownerObject(), &obj);

    record(observer, roots(&obj), [&]() {
        sub.setVal(1.0);
        (void)sub.getVal();
    });

    auto const entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"written", "read"}));
    EXPECT_EQ(entries[1].object, &obj);
    EXPECT_EQ(entries[1].property, &sub);
}

TEST(GtRecordingProperties, propertyContainerEntriesAreRecorded)
{
    TestObject obj;
    obj.addEnvironmentVar(QStringLiteral("PATH"), QStringLiteral("/bin"));
    RecordingTestObserver observer;

    record(observer, roots(&obj), [&]() {
        auto& existing = obj.environmentVars.at(0);
        (void)existing.getMemberVal<QString>(QStringLiteral("name"));

        auto& added = obj.environmentVars.newEntry(
            QStringLiteral("EnvironmentVarsStruct"));
        added.setMemberVal(QStringLiteral("value"), QStringLiteral("1"));
    });

    auto const entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"read", "written"}));
    EXPECT_EQ(entries[1].object, &obj);
    EXPECT_EQ(entries[1].property,
              obj.environmentVars.at(0).findProperty(QStringLiteral("name")));
    EXPECT_EQ(entries[2].object, &obj);
    EXPECT_EQ(entries[2].property,
              obj.environmentVars.at(1).findProperty(QStringLiteral("value")));
}

TEST(GtRecordingProperties, silentPropertiesAreNotRecorded)
{
    SilentPropertyObject obj;
    RecordingTestObserver observer;

    EXPECT_EQ(obj.silent.ownerObject(), nullptr);

    record(observer, roots(&obj), [&]() {
        obj.silent.setVal(1.0);
        (void)obj.silent.getVal();
    });

    EXPECT_TRUE(eventKinds(observer).isEmpty());
}

TEST(GtRecordingProperties, readingInsideCallbackDoesNotRecurse)
{
    RecordingTestObject obj;
    RecordingTestObserver observer;
    observer.readInCallback = true;

    record(observer, roots(&obj), [&]() { (void)obj.value.getVal(); });

    EXPECT_EQ(eventKinds(observer), (QStringList{"read"}));
}

TEST(GtRecordingProperties, counterIsResetAfterRecording)
{
    RecordingTestObject obj;
    RecordingTestObserver observer;

    record(observer, roots(&obj), [&]() {
        EXPECT_EQ(gt::detail::runningRecordings.load(), 1);
    });

    EXPECT_EQ(gt::detail::runningRecordings.load(), 0);
}

TEST(GtRecordingProperties, objectValueReadsReachObserverWithObject)
{
    RecordingTestObject root;
    auto* child = new RecordingTestObject(&root);
    RecordingTestObject unrelated;
    RecordingTestObserver observer;

    record(observer, roots(&root), [&]() {
        (void)root.scaledValue();
        (void)child->scaledValue();
        (void)unrelated.scaledValue();
        // the read depth is restored after the scope
        (void)root.value.getVal();
    });

    // the property read inside scaledValue() produces no own event
    auto const entries = observer.entries();
    ASSERT_EQ(eventKinds(observer),
              (QStringList{"valueRead", "valueRead", "read"}));
    EXPECT_EQ(entries[1].object, &root);
    EXPECT_EQ(entries[1].property, nullptr);
    EXPECT_EQ(entries[2].object, child);
    EXPECT_EQ(entries[3].object, &root);
    EXPECT_EQ(entries[3].property, &root.value);
}

TEST(GtRecordingProperties, objectValueReadsFollowEventMask)
{
    RecordingTestObject obj;
    RecordingTestObserver observer;
    GtObject component;

    GtRecordingSession session{component, observer, roots(&obj),
                               GtRecordingSession::AllEvents &
                                   ~GtRecordingSession::Events(
                                       GtRecordingSession::ObjectValueRead)};
    session.run([&]() {
        (void)obj.scaledValue();
        return true;
    });

    EXPECT_TRUE(eventKinds(observer).isEmpty());
}
