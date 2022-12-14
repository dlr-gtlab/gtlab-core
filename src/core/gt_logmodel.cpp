/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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

GtLogDetails
GtLogDetails::fromGtLog(gt::log::Details const& details)
{
    struct tm time = details.time;
    std::time_t t = std::mktime(&time);

    return {
        QString::fromStdString(details.id),
        QDateTime::fromSecsSinceEpoch(t)
    };
}

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

GtLogModel&
GtLogModel::instance()
{
    static GtLogModel self;
    return self;
}

QString
GtLogModel::format(QString const& msg, int level, GtLogDetails const& details)
{
    Q_UNUSED(level);

    auto time = details.time.toString("hh:mm:ss");

    if (!details.id.isEmpty())
    {
        return QStringLiteral("[%1] [%2] %3").arg(time, details.id, msg);
    }

    return QStringLiteral("[%1] %2").arg(time, msg);
}

GtLogModel::GtLogModel() :
    m_tmpClearLog(false),
    m_maxEntries(2000)
{
    gt::log::Logger& logger = gt::log::Logger::instance();

    auto dest = gt::log::makeSignalSlotDestination(this, &GtLogModel::onMessage);

    logger.addDestination(GT_CLASSNAME(GtLogModel), std::move(dest));

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

QMimeData*
GtLogModel::mimeData(QModelIndexList const& indexes) const
{
    QString str;

    for (const QModelIndex& index : indexes)
    {
        int row = index.row();
        if (row < 0 || row >= m_entries.count())
        {
            continue;
        }

        Entry const& entry = m_entries.at(row);
        const auto level = gt::log::levelFromInt(entry.level);

        str += QStringLiteral("%1 %2\r\n")
                   .arg(gt::log::levelToString(level).c_str(), format(entry));
    }

    QMimeData* mimeData = new QMimeData;
    mimeData->setText(str);

    return mimeData;
}

void
GtLogModel::setMaxLogLength(int val)
{
    if (val <= 10)
    {
        return;
    }

    m_maxEntries = val;
}

int
GtLogModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_entries.count();
}

QVariant
GtLogModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_entries.count())
    {
        return {};
    }

    Entry const& entry = m_entries[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    {
        return format(entry);
    }
    case GtLogModel::LogLevelRole:
        return entry.level;
    }

    return {};
}

bool
GtLogModel::exportLogToFile(const QString& filename)
{
    QFile file(filename);

    if (!file.open(QFile::WriteOnly))
    {
        gtError() << "could not open file! (" << filename << ")";
        return false;
    }

    QTextStream out(&file);

    for (Entry const& entry : qAsConst(m_entries))
    {
        auto level = gt::log::levelFromInt(entry.level);

        out << gt::log::levelToString(level).c_str() << "\t"
            << entry.msg << "\r\n";
    }

    return true;
}

void
GtLogModel::onMessage(const QString& msg, int level, Details const& details)
{
    m_mutex.lock();

    if (m_timer.isActive())
    {
        m_tmpEntries << Entry({ msg, level, details});
    }
    else
    {
        insertMessage(msg, level, details);
        m_timer.start(500);
    }

    m_mutex.unlock();
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
GtLogModel::insertMessage(QString const& msg, int level, Details const& details)
{
    if (m_entries.size() + 1 >= m_maxEntries)
    {
        int offset = m_entries.size() + 2 - m_maxEntries;
        beginRemoveRows(QModelIndex(), 0, offset);
        m_entries.remove(0, offset);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
    m_entries << Entry({ msg, level, details });
    endInsertRows();
}

void
GtLogModel::removeElement(QModelIndex index, int first, int last)
{
    beginRemoveRows(index, first, last);
    for (int i = 0; i <= (last - first); ++i)
    {
        m_entries.removeAt(first);
    }
    endRemoveRows();
}

void
GtLogModel::removeElementList(QModelIndexList indexList, int first, int last)
{
    int size = indexList.size();

    beginRemoveRows(indexList.first().parent(), first, last);

    for (int i = 0; i < size; ++i)
    {
        m_entries.removeAt(indexList.at(size - 1 - i).row());
    }

    endRemoveRows();
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
        beginRemoveRows(QModelIndex(), 0, delta - 1);
        for (int i = 0; i < delta; i++)
        {
            m_entries.removeFirst();
        }
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size() + newRows);

    for (Entry const& entry : qAsConst(m_tmpEntries))
    {
        m_entries << entry;
    }
    endInsertRows();

    m_tmpEntries.clear();
}
