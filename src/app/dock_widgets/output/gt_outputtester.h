/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
