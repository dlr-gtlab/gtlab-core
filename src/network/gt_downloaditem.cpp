/* GTlab - Gas Turbine laboratory
 * Source File: gt_downloaditem.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_downloaditem.h"


GtDownloadItem::GtDownloadItem(QObject* parent) : QObject(parent)
{

}

QUrl
GtDownloadItem::url() const
{
    return m_url;
}

QString
GtDownloadItem::path() const
{
    return m_path;
}

QString
GtDownloadItem::targetFileName() const
{
    return m_targetFileName;
}
