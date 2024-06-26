/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFile>
#include <QMimeData>
#include <QUrl>

#include <ctime>

#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_object.h"
#include "gt_logmodel.h"
#include "gt_logging.h"
#include "gt_logdest.h"
#include "gt_utilities.h"

const auto DESTINATION_ID = [](){ return GT_CLASSNAME(GtLogModel); };

void
GtLogSignalSlotDestination::write(std::string const& message,
                                  gt::log::Level level,
                                  gt::log::Details const& details)
{
    struct tm time = details.time;
    std::time_t t = std::mktime(&time);

    emit messageRecieved(
        QString::fromStdString(message),
        gt::log::levelToInt(level),
        GtLogDetails{
            QString::fromStdString(details.id),
            QDateTime::fromSecsSinceEpoch(t)
        }
    );
}

GtLogModel::GtLogModel() :
    m_tmpClearLog(false),
    m_maxEntries(2000)
{
    setupLoggingDestination();

    connect(&m_timer, SIGNAL(timeout()), SLOT(insertQueue()));

    // set logging length
    if (gtApp)
    {
        if (auto* settings = gtApp->settings())
        {
            setMaxLogLength(settings->maxLogLength());
        }
    }
}

GtLogModel&
GtLogModel::instance()
{
    static GtLogModel self;
    return self;
}

void
GtLogModel::setupLoggingDestination()
{
    gt::log::Logger& logger = gt::log::Logger::instance();

    auto dest = gt::log::makeSignalSlotDestination(this, &GtLogModel::onMessage);
    logger.addDestination(DESTINATION_ID(), std::move(dest));
}

QString
GtLogModel::format(QString const& msg, GtLogDetails const& details)
{
    auto time = details.time.toString("hh:mm:ss");

    if (!details.id.isEmpty())
    {
        return QStringLiteral("[%1] [%2] %3").arg(time, details.id, msg);
    }

    return QStringLiteral("[%1] %2").arg(time, msg);
}

bool
GtLogModel::isEnabled() const
{
    gt::log::Logger& logger = gt::log::Logger::instance();
    return logger.hasDestination(DESTINATION_ID());
}

void
GtLogModel::setEnabled(bool enable)
{
    if (isEnabled() == enable) return;

    gt::log::Logger& logger = gt::log::Logger::instance();

    if (!enable)
    {
        gtWarningId(DESTINATION_ID())
            << tr("Logmodel is now disabled, ignoring incomming messages!");

        logger.removeDestination(DESTINATION_ID());
        return;
    }

    setupLoggingDestination();

    gtInfoId(DESTINATION_ID())
        << tr("Logmodel is now enabled, recieving incomming messages!");
}

bool
GtLogModel::containsLogLevel(gt::log::Level level) const
{
    return std::any_of(m_entries.begin(), m_entries.end(),
                       [level](Entry const& e){
        return e.level == level;
    });
}

QString
GtLogModel::format(const Entry& entry)
{
    return format(entry.msg, entry.details);
}

int
GtLogModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid()) return 0; //no children

    return m_entries.count();
}

int
GtLogModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid()) return 0; //no children

    // col(number) = level, time, id, message
    return 4;
}

QVariant
GtLogModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    int col = index.column();

    if (!index.isValid() || row >= m_entries.count())
    {
        return {};
    }

    // delegate DisplayRole role to custom roles
    if (role == Qt::DisplayRole)
    {
        role = columnToRole(col);
    }

    Entry const& entry = m_entries.at(index.row());

    // get data
    switch (role)
    {
    case LevelRole:
        return entry.level;
    case TimeRole:
        return entry.details.time.toString("hh:mm:ss");
    case IdRole:
        return entry.details.id;
    case MessageRole:
        return entry.msg;
    default:
        break;
    }

    return {};
}

QVariant
GtLogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return {};
    }

    // delegate DisplayRole role to custom roles
    if (role == Qt::DisplayRole)
    {
        role = columnToRole(section);
    }

    switch (role)
    {
    case LevelRole:
        return tr("Level");
    case TimeRole:
        return tr("Time");
    case IdRole:
        return tr("ID");
    case MessageRole:
        return tr("Message");
    default:
        return {};
    }
}

bool
GtLogModel::exportLogToFile(const QString& fileName)
{
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly))
    {
        gtError() << tr("Could not export output log! File: '%1'")
                     .arg(fileName);
        return false;
    }

    QTextStream out(&file);

    for (Entry const& entry : qAsConst(m_entries))
    {
        auto level = gt::log::levelFromInt(entry.level);

        out << gt::log::levelToString(level).c_str() << ' '
            << format(entry) << "\r\n";
    }

    return true;
}

QMimeData*
GtLogModel::mimeData(QModelIndexList const& indexes) const
{
    QStringList list;
    int lastRow = -1;

    /// we assume a sorted list
    for (const QModelIndex& index : indexes)
    {
        int row = index.row();
        if (row < 0 || row >= m_entries.count())
        {
            return {};
        }

        // append new row
        if (lastRow != row)
        {
            list.append(QString{});
            lastRow = row;
        }

        // access last string
        assert(!list.empty());
        QString& str = list.last();

        auto const& entry = m_entries.at(row);
        auto const  level = gt::log::levelFromInt(entry.level);

        // append data
        switch (columnToRole(index.column()))
        {
        case LevelRole:
            str.append(QString::fromStdString(gt::log::levelToString(level)));
            break;
        case TimeRole:
            str.append(entry.details.time.toString("[hh:mm:ss]"));
            break;
        case IdRole:
            if (!entry.details.id.isEmpty())
            {
                str.append(gt::quoted(entry.details.id, QChar{'['}, QChar{']'}));
            }
            break;
        case MessageRole:
            str.append(entry.msg);
            break;
        default:
            continue;
        }
        // padding
        str.append(QChar{' '});
    }

    auto* mimeData = new QMimeData;
    mimeData->setText(list.join(QStringLiteral("\r\n")));
    return mimeData;
}

void
GtLogModel::setMaxLogLength(int val)
{
    if (val < 1) return;

    m_maxEntries = val;

    // remove some entries
    if (m_entries.size() > m_maxEntries)
    {
        beginResetModel();

        m_entries = m_entries.mid(m_entries.size() - m_maxEntries);

        assert(m_entries.size() == m_maxEntries);

        endResetModel();
    }
}

void
GtLogModel::onMessage(const QString& msg, int level, Details const& details)
{
    QMutexLocker locker{&m_mutex};
    if (m_timer.isActive())
    {
        m_tmpEntries << Entry({ msg, level, details});
    }
    else
    {
        insertMessageNonlocked(msg, level, details);
        m_timer.start(500);
    }
}

void
GtLogModel::execClear()
{
    if (m_tmpClearLog)
    {
        beginResetModel();
        m_entries.clear();
        endResetModel();
        m_tmpClearLog = false;
        emit logCleared();
    }
}

void
GtLogModel::clearLog()
{
    m_tmpClearLog = true;

    if (m_timer.isActive())
    {
        return;
    }

    execClear();
}

void
GtLogModel::insertMessageNonlocked(QString const& msg, int level, Details const& details)
{
    if (m_entries.size() >= m_maxEntries)
    {
        // we must use the non-locked
        removeElementListNonlocked({index(0, 0)});
    }

    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries << Entry({ msg, level, details });
    endInsertRows();
}

void
GtLogModel::removeElement(QModelIndex index)
{
    removeElementList({index});
}


void
GtLogModel::removeElementListNonlocked(const QModelIndexList& indexList)
{

    for (auto iter = indexList.rbegin(); iter != indexList.rend(); ++iter)
    {
        const auto& idx = *iter;

        if (idx.row() < 0 || idx.row() >= m_entries.size())
        {
            continue;
        }

        beginRemoveRows(QModelIndex(), idx.row(), idx.row());

        m_entries.removeAt(idx.row());

        endRemoveRows();
    }
}


void
GtLogModel::removeElementList(QModelIndexList indexList)
{
    if (indexList.isEmpty()) return;

    QMutexLocker locker{&m_mutex};
    removeElementListNonlocked(indexList);
}

void
GtLogModel::insertQueue()
{
    m_timer.stop();

    execClear();

    if (m_tmpEntries.isEmpty())
    {
        return;
    }

    int newRows = m_tmpEntries.size();

    // check whether size of new entries exceeds the maximum allowed entries
    if (newRows > m_maxEntries)
    {
        for (int i = 0; i < (newRows - m_maxEntries); i++)
        {
            m_tmpEntries.removeFirst();
        }
    }

    int delta = (m_entries.size() + m_tmpEntries.size()) - m_maxEntries;

    if (delta > m_maxEntries)
    {
        for (int i = 0; i < (delta - m_maxEntries); i++)
        {
            m_tmpEntries.removeFirst();
        }
    }

    newRows = m_tmpEntries.size() - 1;
    delta = (m_entries.size() + m_tmpEntries.size()) - m_maxEntries;

    if (delta > 0)
    {
        beginRemoveRows(QModelIndex{}, 0, delta - 1);
        for (int i = 0; i < delta; i++)
        {
            m_entries.removeFirst();
        }
        endRemoveRows();
    }

    beginInsertRows(QModelIndex{}, m_entries.size(), m_entries.size() + newRows);
    for (Entry const& entry : qAsConst(m_tmpEntries))
    {
        m_entries << entry;
    }
    endInsertRows();

    m_tmpEntries.clear();
}
