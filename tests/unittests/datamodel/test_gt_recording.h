/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TEST_GT_RECORDING_H
#define TEST_GT_RECORDING_H

#include "gt_object.h"
#include "gt_doubleproperty.h"
#include "gt_recordingobserver.h"
#include "gt_recordingsession.h"

#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QVector>

#include <stdexcept>

/// Builds a root list for GtRecordingSession
template <typename... Objects>
inline QList<const GtObject*>
roots(Objects*... objects)
{
    return QList<const GtObject*>{objects...};
}

/// Test object with one registered double property
class RecordingTestObject : public GtObject
{
public:
    explicit RecordingTestObject(GtObject* parent = nullptr) :
        GtObject(parent),
        value(QStringLiteral("value"), QStringLiteral("Value"))
    {
        registerProperty(value);
    }

    GtDoubleProperty value;
};

/// Test-only observer that logs every callback
class RecordingTestObserver : public GtRecordingObserver
{
public:
    struct Entry
    {
        QString kind;
        const GtObject* object{nullptr};
        const GtAbstractProperty* property{nullptr};
        const GtObject* child{nullptr};
        bool success{false};
        /// Session active on the thread while the callback ran
        GtRecordingSession* activeSession{nullptr};
    };

    /// Throw from the named callback kind ("started", "read", ...)
    QString throwIn;

    /// Read the property again inside propertyRead (recursion check)
    bool readInCallback{false};

    QVector<Entry> entries() const
    {
        QMutexLocker locker(&m_mutex);
        return m_entries;
    }

    QStringList kinds() const
    {
        QStringList result;
        for (const Entry& e : entries()) result << e.kind;
        return result;
    }

    void recordingStarted(const GtRecordingSession&) override
    {
        log({QStringLiteral("started")});
    }

    void propertyRead(const GtObject& owner,
                      const GtAbstractProperty& property) override
    {
        log({QStringLiteral("read"), &owner, &property});
        if (readInCallback) (void)property.valueToVariant();
    }

    void propertyWritten(const GtObject& owner,
                         const GtAbstractProperty& property) override
    {
        log({QStringLiteral("written"), &owner, &property});
    }

    void objectAttached(const GtObject& parent, const GtObject& child) override
    {
        log({QStringLiteral("attached"), &parent, nullptr, &child});
    }

    void objectDetached(const GtObject& parent, const GtObject& child) override
    {
        log({QStringLiteral("detached"), &parent, nullptr, &child});
    }

    void recordingFinished(const GtRecordingSession&, bool success) override
    {
        Entry e{QStringLiteral("finished")};
        e.success = success;
        log(e);
    }

private:
    mutable QMutex m_mutex;
    QVector<Entry> m_entries;

    void log(Entry entry)
    {
        entry.activeSession = GtRecordingSession::current();
        {
            QMutexLocker locker(&m_mutex);
            m_entries.push_back(entry);
        }
        if (throwIn == entry.kind)
        {
            throw std::runtime_error("observer failure");
        }
    }
};

#endif // TEST_GT_RECORDING_H
