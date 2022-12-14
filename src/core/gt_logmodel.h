/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
class GT_CORE_EXPORT GtLogModel : public QAbstractListModel
{
    Q_OBJECT

public:

    using Details = GtLogDetails;

    enum Roles
    {
        LogLevelRole = Qt::UserRole
    };

    /**
     * @brief instance
     * @return
     */
    static GtLogModel& instance();

    static QString format(QString const& msg, int level, GtLogDetails const& details);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(QModelIndex const& parent ={}) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(QModelIndex const& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief exportLogToFile
     * @param filename
     * @return
     */
    bool exportLogToFile(QString const& filename);

    /**
     * @brief mimeData
     * @param indexes
     * @return
     */
    QMimeData* mimeData(QModelIndexList const& indexes) const override;

    /**
     * @brief Sets maximum log length (lines)
     * @param Max. number of lines
     */
    void setMaxLogLength(int val);

    /**
     * @brief removeElement - removes elemts from logmodel
     * @param index -  index corresponds to the parent from
     * which the new rows are removed (see QAbstractItemModel::beginRemoveRows)
     * @param first - index of first element to be removed
     * @param last - index of last elment to be removed
     */
    void removeElement(QModelIndex index, int first, int last);

    /**
     * @brief removeElement - removes elemts from logmodel
     * @param indexList -  index corresponds to the parent from
     * which the new rows are removed (see QAbstractItemModel::beginRemoveRows)
     * @param first - index of first element to be removed
     * @param last - index of last elment to be removed
     */
    void removeElementList(QModelIndexList indexList, int first, int last);

public slots:

    void onMessage(QString const& msg ,int level, GtLogDetails const& details);

    void clearLog();

protected:

    /**
     * @brief GtLogModel
     */
    explicit GtLogModel();

private:

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

    static QString format(Entry const& entry)
    {
        return format(entry.msg, entry.level, entry.details);
    }

    void insertMessage(const QString& msg ,int level, Details const& details);

    void execClear();

private slots:

    void insertQueue();
};

#endif // GTLOGMODEL_H
