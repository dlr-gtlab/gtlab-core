/* GTlab - Gas Turbine laboratory
 * Source File: gt_taskhistorymodel.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 08.02.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QIcon>
#include <QFileInfoList>
#include <QJsonDocument>
#include <QJsonObject>

#include "gt_logging.h"
#include "gt_processcomponentmodel.h"

#include "gt_taskhistorymodel.h"

GtTaskHistoryModel::GtTaskHistoryModel(QObject* parent) :
    QAbstractItemModel(parent),
    m_dateFormat(QStringLiteral("dd.MM.yyyy-hh:mm:ss"))
{
}

int
GtTaskHistoryModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_entries.count();
}

int
GtTaskHistoryModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : 4;
}

QVariant
GtTaskHistoryModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_entries.count())
    {
        return QVariant();
    }

    const Entry& entry = m_entries[index.row()];

    switch (role)
    {
        case Qt::DisplayRole:
        {
            switch (index.column())
            {
                case 0:
                    return entry.m_taskId;

                case 1:
                    return taskStateToString(entry.m_state);

                case 2:
                    return entry.m_startTime.toString(m_dateFormat);

                case 3:
                    return entry.m_endTime.toString(m_dateFormat);
            }
        }

        case Qt::DecorationRole:
        {
            if (index.column() == 0)
            {
                return GtProcessComponentModel::stateToIcon(entry.m_state);
            }
        }
    }

    return QVariant();
}

QVariant
GtTaskHistoryModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            switch (section)
            {
                case 0:
                    return tr("Task Id");

                case 1:
                    return tr("Status");

                case 2:
                    return tr("Start");

                case 3:
                    return tr("End");
            }
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}

void
GtTaskHistoryModel::clear()
{
    beginResetModel();

    m_entries.clear();

    endResetModel();
}

void
GtTaskHistoryModel::setPath(const QString& path)
{
    m_path = path + QDir::separator() + QStringLiteral("history");

    populate();
}

bool
GtTaskHistoryModel::hasChildren(const QModelIndex& parent) const
{
    if (!parent.isValid())
    {
        return true;
    }

    return false;
}

QModelIndex
GtTaskHistoryModel::index(int row, int col, const QModelIndex& /*parent*/) const
{
    return createIndex(row, col);
}

QModelIndex
GtTaskHistoryModel::parent(const QModelIndex& /*index*/) const
{
    return {};
}

void
GtTaskHistoryModel::populate()
{
    gtDebug() << "GtTaskHistoryModel::populate()";

    if (m_path.isEmpty())
    {
        gtDebug() << "  |-> path is empty!";
        clear();
        return;
    }

    QDir dir(m_path);

    if (!dir.exists())
    {
        gtDebug() << "  |-> directory doeas not exist!";
        clear();
        return;
    }

    beginResetModel();

    m_entries.clear();

    QStringList list = dir.entryList(QDir::AllDirs, QDir::Name);

    foreach (const QString& listEntry, list)
    {
        gtDebug() << "found history folder (" << listEntry << "), analysing...";

        QDir dirEntry(dir.absoluteFilePath(listEntry));

        if (dirEntry.exists())
        {
            gtDebug() << "  |-> folder exists...";

            Entry tmpEntry;

            if (metaToEntry(
                        dirEntry.absoluteFilePath(QStringLiteral("meta.json")),
                        tmpEntry))
            {
                m_entries << tmpEntry;
            }
        }
    }

    gtDebug() << "found " << m_entries.size() << " entries!";

    endResetModel();

    emit modelRefreshed();
}

bool
GtTaskHistoryModel::metaToEntry(const QString& path, Entry& entry)
{
    QFile metaFile(path);

    if (!metaFile.exists())
    {
        gtDebug() << "  |-> meta file not found! (" << path << ")";
        return false;
    }

    if (!metaFile.open(QIODevice::ReadOnly))
    {
        gtError() << tr("Could not open meta file!");
        return false;
    }

    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(metaFile.readAll(), &error));

    if (loadDoc.isEmpty())
    {
        gtError() << tr("invalid collection item data! - ") <<
                  error.errorString();
        return false;
    }

    QJsonObject json = loadDoc.object();

    if (!metaIsValid(json))
    {
        gtError() << tr("Invalid meta data!");
        return false;
    }

    const QString ident = json[QStringLiteral("ident")].toString();

    entry.m_taskId = ident;

    const QString dateStart = json[QStringLiteral("date_start")].toString();
    const QString dateEnd = json[QStringLiteral("date_end")].toString();

    entry.m_startTime = QDateTime::fromString(dateStart, m_dateFormat);
    entry.m_endTime = QDateTime::fromString(dateEnd, m_dateFormat);

    gtDebug() << "  |-> dateStr = " << dateStart;
    gtDebug() << "  |-> date = " << entry.m_startTime.toString();

    const QString status = json[QStringLiteral("status")].toString();

    entry.m_state = stringToTaskState(status);

    return true;
}

bool
GtTaskHistoryModel::metaIsValid(const QJsonObject& json)
{
    if (!json.contains(QStringLiteral("ident")))
    {
        gtError() << tr("Identification string not found!");
        return false;
    }

    return true;
}

QString
GtTaskHistoryModel::taskStateToString(GtProcessComponent::STATE state) const
{
    switch (state)
    {
        case GtProcessComponent::FINISHED:
            return tr("[FINISHED]");

        case GtProcessComponent::FAILED:
            return tr("[FAILED]");

        case GtProcessComponent::WARN_FINISHED:
            return tr("[WARNING]");

        case GtProcessComponent::TERMINATED:
            return tr("[TERMINATED]");

    }

    return QString();
}

GtProcessComponent::STATE
GtTaskHistoryModel::stringToTaskState(const QString& str) const
{
    if (str == QLatin1String("FINISHED"))
    {
        return GtProcessComponent::FINISHED;
    }
    else if (str == QLatin1String("FAILED"))
    {
        return GtProcessComponent::FAILED;
    }
    else if (str == QLatin1String("WARN_FINISHED"))
    {
        return GtProcessComponent::WARN_FINISHED;
    }
    else if (str == QLatin1String("TERMINATED"))
    {
        return GtProcessComponent::TERMINATED;
    }

    return GtProcessComponent::NONE;
}
