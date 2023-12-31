/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_accessdataconnection.h
 *
 *  Created on: 31.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ACCESSDATACONNECTION_H
#define GT_ACCESSDATACONNECTION_H

#include "gt_core_exports.h"

#include "gt_abstractaccessdataconnection.h"

class QNetworkAccessManager;

/**
 * @brief The GtAccessDataConnection class
 */
class GT_CORE_EXPORT GtAccessDataConnection :
        public GtAbstractAccessDataConnection
{
    Q_OBJECT

public:
    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtAccessDataConnection();

    /**
     * @brief Test connection function.
     * @return Returns true if connection was successful. Otherwise false is
     * returned.
     */
    bool testConnection(const GtAccessData& accessData) override;

private:
    /// Connection test indicator.
    bool m_success;

    /// Network access manager
    QNetworkAccessManager* m_qnam;

private slots:
    /**
     * @brief Called after reply has finished.
     */
    void onReplyFinished();

signals:
    /**
     * @brief Emitted after connection test was finished.
     */
    void testFinished();

};

#endif // GT_ACCESSDATACONNECTION_H
