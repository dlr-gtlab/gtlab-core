/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 14.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTHOSTINFO_H
#define GTHOSTINFO_H

#include <QObject>
#include <QHash>
#include <QHostInfo>

#include "gt_core_exports.h"

/**
 * @brief The GtHostInfoFetcher class
 * helper class for looking up host names and addresses
 */
class GT_CORE_EXPORT GtHostInfo : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Blocking call. Will lookup address and return a QHostAddress
     * object. May be null if look up fails.
     * @param address Address or hostname to look up
     * @param timeout Max ms to wait for lookup
     * @return Host address
     */
    static QHostAddress lookupHost(const QString& address,
                                   int timeout = 5 * 1000);

signals:

    /**
     * @brief Emitted once the look up finishes. Used for internally.
     */
    void finishedHostLookup();

private:

    GtHostInfo();

    /// table to map lookup ids and host info objects
    QHash<int, QHostInfo> m_hostInfos;

    /**
     * @brief Will be called once the host lookup is finished. Used internally
     * @param info Host info
     */
    void onHostLookup(QHostInfo const& info);
};

#endif // GTHOSTINFO_H
