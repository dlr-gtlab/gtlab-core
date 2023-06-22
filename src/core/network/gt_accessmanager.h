/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_accessmanager.h
 *
 *  Created on: 17.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTACCESSMANAGER_H
#define GTACCESSMANAGER_H

#include "gt_core_exports.h"

#include <QObject>

#define gtAccessManager (GtAccessManager::instance())

class QNetworkAccessManager;
class GtAccessGroup;

/**
 * @brief The GtAccessManager class
 */
class GT_CORE_EXPORT GtAccessManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns singleton of access manager.
     * @return Instance of access manager.
     */
    static GtAccessManager* instance();

    /**
     * @brief Serializes given stringlist.
     * @param Stringlist.
     * @return Serialized stringlist.
     */
    static QString serializeStringList(const QStringList& list);

    /**
     * @brief Deserializes given stringlist.
     * @param Serialized stringlist.
     * @return Stringlist.
     */
    static QStringList deserializeStringList(const QString& str);

    /**
     * @brief Adds new group based on given identification string and
     * access data connection meta informations.
     * @param Access data connection meta informations.
     * @return Returns pointer to new created access group. Returns null
     * pointer if access group could not be added.
     */
    GtAccessGroup* addAccessGroup(const QString& id,
                                  const QMetaObject& connection);

    /**
     * @brief Returns the size of access data list.
     * @return Access data list size
     */
    int numberOfDataGroups() const;

    /**
     * @brief Returns true if access manager has no groups. Otherwise false
     * is returned.
     * @return Whether access manager is empty or not.
     */
    bool isEmpty() const;

    /**
     * @brief Returns true if access group of given identification string
     * exists. Otherwise false is returned.
     * @param Access group identification string.
     * @return Whether access group exists or not.
     */
    bool groupExists(const QString& id) const;

    /**
     * @brief Returns list of access group identification strings.
     * @return List of access group identification strings.
     */
    QStringList accessGroupIds() const;

    /**
     * @brief Returns list of access groups.
     * @return List of access groups.
     */
    const QList<GtAccessGroup*>& accessGroups() const;

    /**
     * @brief Saves access data to hard disk.
     * @return Returns true if access data was successfully saved. Otherwise
     * false is returned.
     */
    bool saveAccessData() const;

    /**
     * @brief Returns mac adress of current machine.
     * @return Mac adress.
     */
    QString macAddress() const;

    /**
     * @brief Returns access group corresponding to given identification string.
     * Returns null pointer if no access group was found.
     * @param Access group identification string.
     * @return Access group
     */
    GtAccessGroup* accessGroup(const QString& id);

    /**
     * @brief Returns pointer to network access manager.
     * @return Pointer to network access manager.
     */
    QNetworkAccessManager* qnam();

private:
    /// Network access manager
    QNetworkAccessManager* m_qnam;

    /// Access data list
    QList<GtAccessGroup*> m_data;

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtAccessManager(QObject* parent = nullptr);

    /**
     * @brief Loads access data from hard disk.
     * @param Access group.
     * @return returns true if access data was successfully loaded. Otherwise
     * false is returned.
     */
    bool loadAccessData(GtAccessGroup* accessGroup);

    /**
     * @brief Returns roaming path of application.
     * @return Application roaming path.
     */
    QString roamingPath() const;

};

#endif // GTACCESSMANAGER_H
