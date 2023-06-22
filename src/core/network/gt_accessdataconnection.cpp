/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_accessdataconnection.cpp
 *
 *  Created on: 31.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>

#include "gt_logging.h"
#include "gt_accessmanager.h"

#include "gt_accessdataconnection.h"

GtAccessDataConnection::GtAccessDataConnection() : m_success(false),
    m_qnam(new QNetworkAccessManager(this))
{

}

bool
GtAccessDataConnection::testConnection(const GtAccessData& accessData)
{
    m_success = false;

    QNetworkRequest request(accessData.data());

    QNetworkReply* reply = m_qnam->head(request);

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(this,  SIGNAL(testFinished()), &loop, SLOT(quit()) );
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    if (reply)
    {
        connect(reply, SIGNAL(finished()), SLOT(onReplyFinished()));
    }
    else
    {
        gtWarning() << "Could not connect to host!";
        return false;
    }

    timer.start(10000);

    loop.exec();

    if (!timer.isActive())
    {
        gtWarning() << "Connection timeout!";
    }

    return m_success;
}

void
GtAccessDataConnection::onReplyFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        return;
    }

    if (reply->error() == QNetworkReply::NoError)
    {
        m_success = true;
    }
    else
    {
        m_success = false;
    }

    reply->deleteLater();

    emit testFinished();
}
