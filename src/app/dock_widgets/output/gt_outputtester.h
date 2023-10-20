/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 09.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOUTPUTTESTER_H
#define GTOUTPUTTESTER_H

#include <QThread>

class GtOutputTester : public QThread
{
    Q_OBJECT

public:
    GtOutputTester();

protected:

    void run() override;
};

#endif // GTOUTPUTTESTER_H
