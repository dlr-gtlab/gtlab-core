/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessdataconnection.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 31.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ACCESSDATACONNECTION_H
#define GT_ACCESSDATACONNECTION_H

#include "gt_network_exports.h"

#include "gt_abstractaccessdataconnection.h"

class QNetworkAccessManager;

/**
 * @brief The GtAccessDataConnection class
 */
class GT_NETWORK_EXPORT GtAccessDataConnection :
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
    bool testConnection(const GtAccessData& accessData) Q_DECL_OVERRIDE;

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
