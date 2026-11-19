/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "test_gt_recording.h"

#include "gt_recordinghooks.h"

#include <QStringList>
#include <QVector>

namespace
{

/// Object with a key/value property list ("keys" and "values") that is not
/// held in registered properties, similar to a 0D data zone
class RecordingTestTable : public GtObject
{
public:
    RecordingTestTable() :
        scale(QStringLiteral("scale"), QStringLiteral("Scale"))
    {
        registerProperty(scale);
        scale.setVal(1.0);
    }

    /// Keyed read; also reads the keys and a registered property
    double value(const QString& key) const
    {
        const int index = m_keys.indexOf(key);
        if (index < 0) return 0.0;

        gt::PropertyListReadScope readScope(*this, QStringLiteral("values"),
                                            index, key);
        return scale.getVal() * m_values.at(index);
    }

    /// Whole-list read
    QVector<double> values() const
    {
        gt::PropertyListReadScope readScope(*this, QStringLiteral("values"),
                                            GtRecordingObserver::WholeList);
        return m_values;
    }

    /// Wrongly wrapped accessor: hides the property list read
    double coarseValue(const QString& key) const
    {
        gt::ObjectValueReadScope readScope(*this);
        return value(key);
    }

    void setValue(const QString& key, double value)
    {
        const int index = m_keys.indexOf(key);
        if (index < 0) return;

        m_values[index] = value;
        gt::recordPropertyListWritten(*this, QStringLiteral("values"), index,
                                      key);
    }

    /// Changes two lists together
    void appendValue(const QString& key, double value)
    {
        m_keys.append(key);
        m_values.append(value);
        gt::recordPropertyListWritten(*this, QStringLiteral("keys"),
                                      GtRecordingObserver::WholeList);
        gt::recordPropertyListWritten(*this, QStringLiteral("values"),
                                      GtRecordingObserver::WholeList);
    }

    /// Bookkeeping, e.g. loading externalized data: not a data access
    void fetch()
    {
        gt::SuppressRecordingScope noRecording;
        const QVector<double> loaded = values();
        m_values.clear();
        for (int i = 0; i < loaded.size(); ++i)
        {
            m_values.append(loaded.at(i));
        }
        gt::recordPropertyListWritten(*this, QStringLiteral("values"),
                                      GtRecordingObserver::WholeList);
    }

    GtDoubleProperty scale;

private:
    QStringList m_keys{QStringLiteral("Tt4"), QStringLiteral("Pt4")};
    QVector<double> m_values{1650.0, 30.0};
};

/// Observer that only overrides the object value callbacks
class ObjectValueObserver : public GtRecordingObserver
{
public:
    QStringList kinds;

    void objectValueRead(const GtObject&) override
    {
        kinds << QStringLiteral("valueRead");
    }

    void objectValueWritten(const GtObject&) override
    {
        kinds << QStringLiteral("valueWritten");
    }
};

/// Runs the work in a session observing the given roots
template <typename Observer, typename Work>
void
record(Observer& observer, QList<const GtObject*> roots, Work&& work,
       GtRecordingSession::Events mask = GtRecordingSession::AllEvents)
{
    GtObject component;
    GtRecordingSession session{component, observer, roots, mask};
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

TEST(GtRecordingPropertyLists, keyedReadReportsOwnerListIndexAndKey)
{
    RecordingTestTable table;
    RecordingTestTable unrelated;
    RecordingTestObserver observer;

    record(observer, roots(&table), [&]() {
        EXPECT_DOUBLE_EQ(table.value(QStringLiteral("Pt4")), 30.0);
        (void)unrelated.value(QStringLiteral("Pt4"));
    });

    // the read of the registered property inside value() is hidden
    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"listRead"}));
    EXPECT_EQ(entries[1].object, &table);
    EXPECT_EQ(entries[1].listName, QStringLiteral("values"));
    EXPECT_EQ(entries[1].index, 1);
    EXPECT_EQ(entries[1].key, QStringLiteral("Pt4"));
}

TEST(GtRecordingPropertyLists, wholeListReadUsesWholeListIndex)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table), [&]() { (void)table.values(); });

    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer), (QStringList{"listRead"}));
    EXPECT_EQ(entries[1].index, GtRecordingObserver::WholeList);
    EXPECT_TRUE(entries[1].key.isEmpty());
}

TEST(GtRecordingPropertyLists, unknownKeyReportsNothing)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table),
           [&]() { (void)table.value(QStringLiteral("unknown")); });

    EXPECT_TRUE(eventKinds(observer).isEmpty());
}

TEST(GtRecordingPropertyLists, readDepthIsRestoredAfterScope)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table), [&]() {
        (void)table.value(QStringLiteral("Tt4"));
        (void)table.scale.getVal();
    });

    EXPECT_EQ(eventKinds(observer), (QStringList{"listRead", "read"}));
}

TEST(GtRecordingPropertyLists, outerObjectValueReadHidesListRead)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table),
           [&]() { (void)table.coarseValue(QStringLiteral("Tt4")); });

    EXPECT_EQ(eventKinds(observer), (QStringList{"valueRead"}));
}

TEST(GtRecordingPropertyLists, writesReportIndexAndKeyOrWholeList)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table), [&]() {
        table.setValue(QStringLiteral("Tt4"), 1700.0);
        table.appendValue(QStringLiteral("Wf"), 2.5);
    });

    const auto entries = observer.entries();
    ASSERT_EQ(eventKinds(observer),
              (QStringList{"listWritten", "listWritten", "listWritten"}));
    EXPECT_EQ(entries[1].object, &table);
    EXPECT_EQ(entries[1].listName, QStringLiteral("values"));
    EXPECT_EQ(entries[1].index, 0);
    EXPECT_EQ(entries[1].key, QStringLiteral("Tt4"));
    EXPECT_EQ(entries[2].listName, QStringLiteral("keys"));
    EXPECT_EQ(entries[2].index, GtRecordingObserver::WholeList);
    EXPECT_EQ(entries[3].listName, QStringLiteral("values"));
    EXPECT_EQ(entries[3].index, GtRecordingObserver::WholeList);
}

TEST(GtRecordingPropertyLists, eventsOutsideRecordingAreIgnored)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    (void)table.value(QStringLiteral("Tt4"));
    table.setValue(QStringLiteral("Tt4"), 1700.0);

    EXPECT_TRUE(observer.entries().isEmpty());
}

TEST(GtRecordingPropertyLists, eventsFollowEventMask)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    // Events(AllEvents): Qt5 has no operator&(Enum, QFlags)
    const GtRecordingSession::Events mask =
        GtRecordingSession::Events(GtRecordingSession::AllEvents) &
        ~(GtRecordingSession::Events(GtRecordingSession::PropertyListRead) |
          GtRecordingSession::Events(GtRecordingSession::PropertyListWritten));

    record(
        observer, roots(&table),
        [&]() {
            (void)table.value(QStringLiteral("Tt4"));
            table.setValue(QStringLiteral("Tt4"), 1700.0);
        },
        mask);

    EXPECT_TRUE(eventKinds(observer).isEmpty());
}

TEST(GtRecordingPropertyLists, defaultCallbacksForwardToObjectValueEvents)
{
    RecordingTestTable table;
    ObjectValueObserver observer;

    record(observer, roots(&table), [&]() {
        (void)table.value(QStringLiteral("Tt4"));
        table.setValue(QStringLiteral("Tt4"), 1700.0);
    });

    EXPECT_EQ(observer.kinds, (QStringList{"valueRead", "valueWritten"}));
}

TEST(GtRecordingPropertyLists, suppressedBookkeepingReportsNothing)
{
    RecordingTestTable table;
    RecordingTestObserver observer;

    record(observer, roots(&table), [&]() {
        table.fetch();
        // recording resumes after the scope
        (void)table.values();
    });

    EXPECT_EQ(eventKinds(observer), (QStringList{"listRead"}));
}
