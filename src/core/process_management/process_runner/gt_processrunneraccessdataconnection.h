/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTPROCESSRUNNERACCESSDATACONNECTION_H
#define GTPROCESSRUNNERACCESSDATACONNECTION_H

#include "gt_abstractaccessdataconnection.h"

class GtProcessRunnerAccessDataConnection :
        public GtAbstractAccessDataConnection
{
    Q_OBJECT

public:

    Q_INVOKABLE GtProcessRunnerAccessDataConnection();

    bool requestPort() override;

    QString defaultHost() override;

    int defaultPort() override;

private:

    bool testConnection(const GtAccessData& accessData) override;

};

#endif // GTPROCESSRUNNERACCESSDATACONNECTION_H
