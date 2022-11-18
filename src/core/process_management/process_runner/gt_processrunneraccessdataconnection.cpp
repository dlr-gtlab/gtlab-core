/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 24.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gt_processrunneraccessdataconnection.h"

#include "gt_processrunnerglobals.h"
#include "gt_logging.h"

GtProcessRunnerAccessDataConnection::GtProcessRunnerAccessDataConnection()
{
    setObjectName("ProcessRunnerAccessDataConnection");
}

bool
GtProcessRunnerAccessDataConnection::requestPort()
{
    return true;
}

QString
GtProcessRunnerAccessDataConnection::defaultHost()
{
    return gt::process_runner::S_DEFAULT_HOST;
}

int
GtProcessRunnerAccessDataConnection::defaultPort()
{
    return gt::process_runner::S_DEFAULT_PORT;
}

bool
GtProcessRunnerAccessDataConnection::testConnection(const GtAccessData&)
{
    gtWarningId(GT_EXEC_ID)
            << tr("Testing a Connection with a Process Runner is currently "
                  "unsupported!");
    return false;
}
