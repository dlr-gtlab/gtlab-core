/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 14.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_hostinfo.h"
#include "gt_eventloop.h"
#include "gt_logging.h"

GtHostInfo::GtHostInfo() = default;

QHostAddress
GtHostInfo::lookupHost(const QString& address, int timeout)
{
    // check if address is already an valid host address
    QHostAddress helper{address};
    if (!helper.isNull())
    {
        return helper;
    }

    // check for localhost
    if (address.compare(QStringLiteral("localhost"),Qt::CaseInsensitive) == 0 ||
        address.compare(QHostInfo::localHostName(), Qt::CaseInsensitive) == 0)
    {
        return QHostAddress::LocalHost;
    }

    // resolve host name using QHostInfo
    GtHostInfo hostInfo;
    GtEventLoop loop{timeout};
    loop.connectSuccess(&hostInfo, &GtHostInfo::finishedHostLookup);

    int id = QHostInfo::lookupHost(address, &hostInfo,
                                   &GtHostInfo::onHostLookup);

    if (loop.exec() != GtEventLoop::Success)
    {
        gtWarning() << tr("Host lookup error:") << tr("Operation timed out");
        return {};
    }

    // retrieve host info (may be invalid)
    auto info = hostInfo.m_hostInfos.take(id);
    auto addresses = info.addresses();

    if (addresses.empty())
    {
        return {};
    }

    gtDebug() << "Client addresses found:" << addresses;

    // simply take first address
    return addresses.first();
}

void
GtHostInfo::onHostLookup(const QHostInfo& info)
{
    if (info.error() != QHostInfo::NoError &&
        info.error() != QHostInfo::UnknownError)
    {
        gtWarning() << tr("Host lookup error:") << info.errorString();
    }

    m_hostInfos.insert(info.lookupId(), info);
    emit finishedHostLookup();
}
