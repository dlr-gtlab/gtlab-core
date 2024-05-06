/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLOGMODEL_H
#define GTLOGMODEL_H

#include "gt_core_exports.h"

#include "gt_typetraits.h"
#include "gt_logdest.h"

#include <QDateTime>
#include <QAbstractItemModel>
#include <QAbstractTableModel>
#include <QMutex>
#include <QTimer>

#define gtLogModel (&GtLogModel::instance())

struct GtLogDetails
{
    QString id;
    QDateTime time;
};

Q_DECLARE_METATYPE(GtLogDetails)

/**
 * @brief The GtLogSignalSlotDestination class
 * Functor destination sink which uses Qts Signal Slot mechanism. One should
 * not perform any time-consuming operations. Slot is connected using queued
 * connections.
 */
class GT_CORE_EXPORT GtLogSignalSlotDestination :
        public QObject, public gt::log::Destination
{
    Q_OBJECT

public:

    GtLogSignalSlotDestination() { setObjectName("GtLog::SignalSlotFunctor"); }

    /**
     * @brief Constructor that accepts member function pointers
     * @param object QObject
     * @param memberf member function to connect to
     */
    template <typename T, typename F,
              gt::trait::enable_if_derived_of_qobject<T> = true>
    GtLogSignalSlotDestination(T* object, F memberf) :
        GtLogSignalSlotDestination()
    {
        connect(this, &GtLogSignalSlotDestination::messageRecieved,
                object, memberf, Qt::QueuedConnection);
    }

    /**
     * @brief Constructor that accepts member function names
     * @param object QObject
     * @param memberf member function name to connect to
     */
    template <typename T,
              gt::trait::enable_if_derived_of_qobject<T> = true>
    GtLogSignalSlotDestination(T* object, const char* memberf) :
        GtLogSignalSlotDestination()
    {
        connect(this, SIGNAL(logMessageReady(QString, int)),
                object, memberf, Qt::QueuedConnection);
    }

    void write(std::string const& message,
               gt::log::Level level,
               gt::log::Details const& details) override;

signals:

    /// emitted once a new message was recieved
    void messageRecieved(QString message, int level, GtLogDetails details);
};


namespace gt
{

namespace log
{

template <typename T, typename F,
          gt::trait::enable_if_derived_of_qobject<T> = true>
inline std::unique_ptr<GtLogSignalSlotDestination>
makeSignalSlotDestination(T* receiver, F member)
{
    return std::make_unique<GtLogSignalSlotDestination>(receiver, member);
}

} // namespace log

} // namespace gt


/**
 * @brief The GtLogModel class
 */
class GT_CORE_EXPORT GtLogModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    using Details = GtLogDetails;

    /// custom roles to access the data directly
    enum LogRole
    {
        InvalidRole = Qt::UserRole, // invalid role
        LevelRole,                  // role to access level as int
        TimeRole,                   // role to access time as string
        IdRole,                     // role to access logging id
        MessageRole                 // role to access message
    };

    /**
     * @brief instance
     * @return self
     */
    static GtLogModel& instance();

    /**
     * @brief Formats the message
     * @param msg Message to format
     * @param details Message logging details
     * @return Formatted string
     */
    static QString format(QString const& msg, GtLogDetails const& details);

    static constexpr LogRole columnToRole(int col) noexcept
    {
        switch (col)
        {
        case 0:
            return LevelRole;
        case 1:
            return TimeRole;
        case 2:
            return IdRole;
        case 3:
            return MessageRole;
        default:
            return InvalidRole;
        }
    }

    static constexpr int columnFromRole(LogRole role) noexcept
    {
        switch (role)
        {
        case LevelRole:
            return 0;
        case TimeRole:
            return 1;
        case IdRole:
            return 2;
        case MessageRole:
            return 3;
        default:
            return -1;
        }
    }

    /**
     * @brief Returns whether the log model is enables (effectively ignoring
     * any incoming messages)
     * @return Whether incoming messages are ignored
     */
    bool isEnabled() const;

    /**
     * @brief Enables/Disables the log model effectively making it ignoring
     * incoming messages. Useful if one knows that many irrelevant messgaes are
     * about to be triggered. Handle with care: All messages that are ignored
     * cannot be viewed/accessed using the log model.
     * @param value Whether to enable or disable the log model
     */
    void setEnabled(bool enable = true);

    /**
     * @brief Checks if the model contains messages that have the specified
     * log level
     * @param level Log level to check
     * @return Whether the log model contains messages with the specified
     * log level
     */
    bool containsLogLevel(gt::log::Level level) const;

    /**
     * @brief Returns the row count of this model
     * @param parent Parent index
     * @return row count
     */
    int rowCount(QModelIndex const& parent = {}) const override;

    /**
     * @brief Returns the column count of this model
     * @param parent Parent index
     * @return column count
     */
    int columnCount(QModelIndex const& parent = {}) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(QModelIndex const& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief Exports the output log to the file specified
     * @param filename File to export to
     * @return success
     */
    bool exportLogToFile(QString const& filename);

    /**
     * @brief Converts the indexes data into mime data (text/plain).
     * List should be sorty by row and column.
     * @param indexes Indexes to convert
     * @return Mime data
     */
    QMimeData* mimeData(QModelIndexList const& indexes) const override;

    /**
     * @brief Sets maximum log length (lines)
     * @param Max. number of lines
     */
    void setMaxLogLength(int val);

    /**
     * @brief Removes the elemets from the logmodel
     * @param index Index corresponds to the parent from which the rows are
     * removed (see QAbstractItemModel::beginRemoveRows)
     * @param first Index of first element to be removed
     * @param last Index of last elment to be removed
     */
    void removeElement(QModelIndex index);

    /**
     * @brief Removes the elements from the logmodel
     * @param indexList Indicies corresponds to the parent from which the rows
     * are removed (see QAbstractItemModel::beginRemoveRows)
     * @param first Index of first element to be removed
     * @param last Index of last elment to be removed
     */
    void removeElementList(QModelIndexList indexList);

public slots:

    void onMessage(QString const& msg ,int level, GtLogDetails const& details);

    /**
     * @brief Clears the log
     */
    void clearLog();

signals:

    void logCleared();

protected:

    /**
     * @brief GtLogModel
     */
    explicit GtLogModel();

private:

    /// Entry struct
    struct Entry
    {
        QString msg;
        int level;
        Details details;
    };

    QVector<Entry> m_entries;

    QVector<Entry> m_tmpEntries;

    QMutex m_mutex;

    QTimer m_timer;

    bool m_tmpClearLog;

    int m_maxEntries;

    /**
     * @brief Formats the message described by entry
     * @param entry Entry to format
     * @return Formatted String
     */
    static QString format(Entry const& entry);

    /**
     * @brief Inserts the message into the model
     * @param msg Message
     * @param level Level
     * @param details Logging details
     */
    void insertMessageNonlocked(const QString& msg ,int level, Details const& details);

    /**
     * @brief Non mutex-locked implementation of element removal
     */
    void removeElementListNonlocked(const QModelIndexList& indexList);

    /**
     * @brief Helper method for clearing the output log
     */
    void execClear();

    /**
     * @brief Adds a logging destion for this class
     */
    void setupLoggingDestination();

private slots:

    void insertQueue();
};

#endif // GTLOGMODEL_H
