/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_downloaditem.cpp
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_downloaditem.h"


GtDownloadItem::GtDownloadItem(QObject* parent) : QObject(parent)
{

}

const QUrl&
GtDownloadItem::url() const
{
    return m_url;
}

const QString&
GtDownloadItem::path() const
{
    return m_path;
}

const QString&
GtDownloadItem::targetFileName() const
{
    return m_targetFileName;
}
