/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractaccessdataconnection.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 31.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include "gt_abstractaccessdataconnection.h"

GtAbstractAccessDataConnection::GtAbstractAccessDataConnection() :
    m_accessData(GtAccessData(QString()))
{

}

void
GtAbstractAccessDataConnection::setAccessData(const GtAccessData& accessData)
{
    m_accessData = accessData;
}

bool
GtAbstractAccessDataConnection::requestPort()
{
    return false;
}

QString
GtAbstractAccessDataConnection::defaultHost()
{
    return QStringLiteral("https://");
}

int
GtAbstractAccessDataConnection::defaultPort()
{
    return -1;
}

void
GtAbstractAccessDataConnection::run()
{
    if (testConnection(m_accessData))
    {
        emit connectionSuccessful();
    }
    else
    {
        emit connectionFailed();
    }
}
