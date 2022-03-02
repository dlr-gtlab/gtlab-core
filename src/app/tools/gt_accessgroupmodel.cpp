/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessgroupmodel.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 26.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFont>
#include <QIcon>

#include "gt_accessgroup.h"
#include "gt_application.h"

#include "gt_accessgroupmodel.h"

GtAccessGroupModel::GtAccessGroupModel(GtAccessGroup* group, QObject* parent) :
    QAbstractTableModel(parent),
    m_group(group)
{

}

int
GtAccessGroupModel::rowCount(const QModelIndex& /*parent*/) const
{
    if (!m_group)
    {
        return 0;
    }

    return m_group->numberOfAccessData();
}

int
GtAccessGroupModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 5;
}

QVariant
GtAccessGroupModel::data(const QModelIndex& index, int role) const
{
    QVariant retVal{};

    if (!m_group)
    {
        return retVal;
    }

    const int row = index.row();
    const int col = index.column();

    QList<GtAccessData> accessDataList = m_group->accessData();

    if (row < 0 || row >= accessDataList.size())
    {
        return retVal;
    }

    switch (role)
    {
        case Qt::ToolTipRole:
        {
            if (col == 0)
            {
                QString tmpStr = accessDataList[row].host();

                if (accessDataList[row].port() >= 0)
                {
                    tmpStr = tmpStr +
                             QStringLiteral(":") +
                             QString::number(accessDataList[row].port());
                }

                retVal = tmpStr;
                break;
            }

            break;
        }
        case Qt::DisplayRole:
        {
            if (col == 0)
            {
                QString tmpStr = accessDataList[row].host();

                if (accessDataList[row].port() >= 0)
                {
                    tmpStr = tmpStr +
                             QStringLiteral(":") +
                             QString::number(accessDataList[row].port());
                }

                retVal = tmpStr;
                break;
            }
            else if (col == 1)
            {
                retVal = accessDataList[row].user();
                break;
            }
            else if (col == 2)
            {
                QString pwStr;

                for (int i = 0; i < accessDataList[row].password().size(); i++)
                {
                    pwStr += "*";
                }

                retVal = pwStr;
                break;
            }

            break;
        }

        case Qt::DecorationRole:
        {
            if (col == 3)
            {
                retVal = gtApp->icon(QStringLiteral("configIcon_16.png"));
            }
            else if (col == 4)
            {
                retVal = gtApp->icon(QStringLiteral("closeIcon_16.png"));
            }

            break;
        }

        case Qt::FontRole:
            retVal = QFont{"Arial", 8};
            break;

        default:
            break;
    }

    return retVal;
}

QVariant
GtAccessGroupModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return QStringLiteral("Host");
        }
        else if (section == 1)
        {
            return QStringLiteral("User");
        }
        else if (section == 2)
        {
            return QStringLiteral("Passwort");
        }
        else
        {
            return QString();
        }
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

bool
GtAccessGroupModel::addAccessData(const GtAccessData& accessData)
{
    if (!m_group)
    {
        return false;
    }

    if (!m_group->addAccessData(accessData.host(),
                                accessData.port(),
                                accessData.user(),
                                accessData.password()))
    {
        return false;
    }

    beginInsertRows(QModelIndex(), m_group->numberOfAccessData(),
                    m_group->numberOfAccessData());

    endInsertRows();

    return true;
}

bool
GtAccessGroupModel::removeAccessData(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return false;
    }

    if (index.model() != this)
    {
        return false;
    }

    if (!m_group)
    {
        return false;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());

    m_group->removeAccessData(index.row());

    endRemoveRows();

    return true;
}

bool
GtAccessGroupModel::setAccessData(const GtAccessData& accessData,
                                  const QModelIndex& index)
{
    if (!index.isValid())
    {
        return false;
    }

    if (index.model() != this)
    {
        return false;
    }

    if (!m_group)
    {
        return false;
    }

    if (!m_group->setAccessData(accessData, index.row()))
    {
        return false;
    }

    QModelIndex left = createIndex(index.row(), 0);
    QModelIndex right = createIndex(index.row(), 2);

    emit dataChanged(left, right);

    return true;
}
