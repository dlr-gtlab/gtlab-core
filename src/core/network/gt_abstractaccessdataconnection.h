/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractaccessdataconnection.h
 *
 *  Created on: 31.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ABSTRACTACCESSDATACONNECTION_H
#define GT_ABSTRACTACCESSDATACONNECTION_H

#include "gt_core_exports.h"

#include "gt_object.h"
#include "gt_accessdata.h"

/**
 * @brief The GtAbstractAccessDataConnection class
 */
class GT_CORE_EXPORT GtAbstractAccessDataConnection : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GtAbstractAccessDataConnection();

    /**
     * @brief Sets given access data.
     * @param Access data.
     */
    void setAccessData(const GtAccessData& accessData);

    /**
     * @brief Returns true if port data is needed by connection.
     * @return Request port indicator.
     */
    virtual bool requestPort();

    /**
     * @brief Returns default host name.
     * @return Default host name.
     */
    virtual QString defaultHost();

    /**
     * @brief Returns default port number.
     * @return Default port number.
     */
    virtual int defaultPort();

public slots:
    /**
     * @brief Called to run data connection.
     */
    void run();

protected:
    /**
     * @brief Pure virtual test connection method.
     * @param Given access data.
     * @return Returns true if connection test was successful. Otherwise false
     * is returned.
     */
    virtual bool testConnection(const GtAccessData& accessData) = 0;

private:
    /// Access data
    GtAccessData m_accessData;

signals:
    /**
     * @brief Emitted if connection was successful.
     */
    void connectionSuccessful();

    /**
     * @brief Emitted if connection failed.
     */
    void connectionFailed();
};

#endif // GT_ABSTRACTACCESSDATACONNECTION_H
