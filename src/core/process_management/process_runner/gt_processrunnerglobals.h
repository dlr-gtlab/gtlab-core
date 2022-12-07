/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 24.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_PROCESSRUNNERGLOBALS_H
#define GT_PROCESSRUNNERGLOBALS_H

#include "gt_core_exports.h"
#include "gt_enumproperty.h"
#include "gt_processcomponent.h"
#include "gt_stringproperty.h"
#include <QVariant>

class GtProcessRunnerResponse;

namespace gt
{
// namespace for process runner exports
namespace process_runner
{
// the namespace must be added to MOS to register the enums.
// the namespace must also be exported
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
Q_NAMESPACE_EXPORT(GT_CORE_EXPORT);
#else
GT_CORE_EXPORT Q_NAMESPACE;
#endif

GT_CORE_EXPORT extern const quint16 S_DEFAULT_PORT;
GT_CORE_EXPORT extern const QString S_DEFAULT_HOST;
GT_CORE_EXPORT extern const QString S_ACCESS_ID;

GT_CORE_EXPORT QString portLockFilePath(quint16 port);

GT_CORE_EXPORT QString errorMessage(GtProcessRunnerResponse* response);

enum State
{
    UninitializedState = 0,  // session not initialized
    InitializedState,        // session initialized, waiting for commands
    RunningTaskState,        // task is beeing executed
    FinishedTaskState,       // task is finished
    CollectedState           // task has been collected
};
Q_ENUM_NS(State)

enum Error
{
    NoError = 0,            // no error (i.e. success)
    UnkownCommandError,     // command unkown
    InvalidArgumentError,   // command argument invalid
    InvalidStateError,      // command for wrong state
    InvalidSessionError,    // invalid session (e.g. id mismatch)
    FootprintError,         // footprint mismatches
    RunTaskError,           // task could not be triggered
    CollectTaskError,       // task could not be triggered
    // ...
    UnkownError = 42,
};
Q_ENUM_NS(Error)

enum NotificationType
{
    UnkownNotification = 0,         // Unkown notification
    TaskStateChangeNotification,    // A process component changed its state
    TaskPropertyChangeNotification, // Monitoring data of a proess component changed
    TaskFinishedNotification,       // Task finished successfully
    TaskFailedNotification,         // Task failed
};
Q_ENUM_NS(NotificationType)

/**
 * @brief The ConnectionAddress struct.
 * Helper struct which holds the connection data of the current/next
 * connection
 */
struct ConnectionAddress
{
    QString address{};
    quint16 port{};

    bool isValid() const
    {
        return !(address.isEmpty() && port == 0);
    }
};

/**
 * @brief The GtProcessComponentMetaData class.
 * Helper class for storing the state of a process component
 * (used in the process executor)
 */
class GT_CORE_EXPORT ProcessComponentStatePayload : public GtObject
{
    Q_OBJECT

public:

    Q_INVOKABLE explicit ProcessComponentStatePayload(GtProcessComponent* pc = {});

    GtProcessComponent::STATE state() const;

    QString /*const&*/ componentUuid() const;

private:

    GtStringProperty m_pComponent;
    GtEnumProperty<GtProcessComponent::STATE> m_pComponentState;
};

/**
 * @brief The GtProcessComponentMetaData class.
 * Helper class for storing the state of a process component
 * (used in the process executor)
 */
class GT_CORE_EXPORT MementoDiffPayload : public GtObject
{
    Q_OBJECT

public:

    Q_INVOKABLE MementoDiffPayload();

    explicit MementoDiffPayload(GtObjectMementoDiff const& diff);

    GtObjectMementoDiff mementoDiff() const;

private:

    GtStringProperty m_pMementoDiff;
};

} // namespace process_runner

} // namespace gt

#endif // GT_PROCESSRUNNERGLOBALS_H
