/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessgroup.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 24.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTACCESSGROUP_H
#define GTACCESSGROUP_H

#include "gt_core_exports.h"

#include <QObject>

#include "gt_accessdata.h"

/**
 * @brief The GtAccessGroup class
 */
class GT_CORE_EXPORT GtAccessGroup : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    /**
     * @brief Constructor.
     * @param Access group identification string.
     * @param Access connection meta data.
     * @param Parent object.
     */
    GtAccessGroup(const QString& id, const QMetaObject& connection,
                  QObject* parent = Q_NULLPTR);

    /**
     * @brief Adds access data to group.
     * @param Host name.
     * @param Port number.
     * @param Username.
     * @param Password.
     * @return Returns true if access data was successfully added. Otherwise
     * false is returned.
     */
    bool addAccessData(const QString& host,
                 int port,
                 const QString& user,
                 const QString& pw = QString());

    /**
     * @brief Returns list of access data objects.
     * @return Access data list.
     */
    const QList<GtAccessData>& accessData();

    /**
     * @brief Returns number of access data.
     * @return Number of access data.
     */
    int numberOfAccessData() const;

    /**
     * @brief Returns true if given host exists in group. Otherwise false is
     * returned.
     * @param Host name.
     * @return Whether host exists or not.
     */
    bool hostExists(const QString& host);

    /**
     * @brief Removes access data corresponding to given index.
     * @param Index of access data object.
     * @return Returns true if access data was successfully removed from group.
     * Otherwise false is returned.
     */
    bool removeAccessData(int index);

    /**
     * @brief Replaces access data for given index.
     * @param New access data.
     * @param Access data position.
     * @return Returns true if access data was successfully replaced. Otherwise
     * false is returned.
     */
    bool setAccessData(const GtAccessData& data, int index);

    /**
     * @brief Returns meta object of access data connection.
     * @return Meta object of access data connection.
     */
    const QMetaObject& connectionMetaData();

private:
    /// Connection meta data
    QMetaObject m_connectionMetaData;

    /// access data list
    QList<GtAccessData> m_data;

};

#endif // GTACCESSGROUP_H
