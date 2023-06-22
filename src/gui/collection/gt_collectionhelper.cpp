/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_collectionhelper.cpp
 *
 *  Created on: 13.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QNetworkReply>
#include <QTimer>
#include <QEventLoop>
#include <QJsonDocument>

#include "gt_logging.h"

#include "gt_collectionhelper.h"

GtCollectionHelper::GtCollectionHelper(
    const QList<GtAccessData>& accessDataList,
    GtCollectionReply* reply,
    QObject* parent) :
    QObject(parent),
    m_hostQueue(accessDataList),
    m_qnam(new QNetworkAccessManager(this)),
    m_reply(reply)
{

}

QList<GtCollectionHelperData>
GtCollectionHelper::collectionItems() const
{
    return m_items;
}

GtCollectionReply*
GtCollectionHelper::reply()
{
    return m_reply;
}

void
GtCollectionHelper::exec()
{

    foreach (GtAccessData accessData, m_hostQueue)
    {
        readAccessData(accessData);
    }

    gtDebug().medium() << tr("all data loaded!");

    emit finished();
}

void
GtCollectionHelper::readAccessData(const GtAccessData& accessData)
{
    gtDebug() << "fetching " << accessData.host() << "...";

    QUrl url = accessData.data();

    if (accessData.host().endsWith(QStringLiteral("/")))
    {
        url.setUrl(accessData.host() + QStringLiteral("index.dat"));
    }
    else
    {
        url.setUrl(accessData.host() + QStringLiteral("/index.dat"));
    }

    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(this,  SIGNAL(accessDataFinished()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &timer, SLOT(stop()));

    QNetworkReply* reply = m_qnam->get(QNetworkRequest(url));

    if (reply)
    {
        connect(reply, SIGNAL(finished()), SLOT(onListFetched()));
    }
    else
    {
        gtWarning() << "Could not connect to host!";
        return;
    }

    timer.start(10000);

    loop.exec();

    if (!timer.isActive())
    {
        gtWarning() << "Connection timeout!";
    }
}

void
GtCollectionHelper::processListData(const QByteArray& byteArray,
                                    QUrl url)
{
    QString str = byteArray;
    QStringList urls = str.split(QRegExp(QStringLiteral("[\r\n]")),
                                 Qt::SkipEmptyParts);

    url.setUrl(url.toDisplayString().remove(QStringLiteral("index.dat")));

    for (int i = 0; i < urls.size(); i++)
    {
        QUrl newUrl = url;
        newUrl.setUrl(url.toDisplayString() + urls[i]);

        m_itemQueue.append(newUrl);
    }
}

void
GtCollectionHelper::processItemData(const QUrl& url,
                                    const QByteArray& byteArray)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(byteArray, &error));

    if (loadDoc.isEmpty())
    {
        gtError() << tr("invalid collection item data!");
        return;
    }

    gtDebug().medium() << "item data loaded!";

    m_mutex.lock();

    m_items << GtCollectionHelperData(url, loadDoc.object());

    m_mutex.unlock();
}

void
GtCollectionHelper::onListFetched()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        return;
    }

    if (reply->error())
    {
        gtError() << tr("Could not fetch list!") << QStringLiteral(" (") <<
                     reply->url().toDisplayString() << QStringLiteral(")");
    }
    else
    {
        processListData(reply->readAll(), reply->url());
    }

    reply->deleteLater();

    gtDebug() << "list fetched! loading items...";

    loadNextItem();
}

void
GtCollectionHelper::onItemDataLoaded()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (!reply)
    {
        return;
    }

    if (reply->error())
    {
        gtError() << tr("Could not load item data!");
    }
    else
    {
        QUrl url = reply->url();

        QString urlStr = url.toDisplayString();

        if (urlStr.endsWith(QStringLiteral("/index.json")))
        {
            urlStr.remove(QStringLiteral("/index.json"));
        }

        url.setUrl(urlStr);

        gtDebug().medium() << "onItemDataLoaded() " << url.toDisplayString();

        processItemData(url, reply->readAll());
    }

    reply->deleteLater();

    loadNextItem();
}

void
GtCollectionHelper::loadNextItem()
{
    if (m_itemQueue.isEmpty())
    {
        emit accessDataFinished();
        return;
    }

    QUrl url = m_itemQueue.takeFirst();

    gtDebug() << tr("loading collection item ")
              << url.toDisplayString() << "...";

    QString urlStr = url.toDisplayString();

    const QString filename = QStringLiteral("index.json");

    if (urlStr.endsWith(QStringLiteral("/")))
    {
        url.setUrl(QString(urlStr + filename));
    }
    else
    {
        url.setUrl(QString(urlStr + QStringLiteral("/") + filename));
    }

    QNetworkReply* reply = m_qnam->get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), SLOT(onItemDataLoaded()),
            Qt::QueuedConnection);
}
