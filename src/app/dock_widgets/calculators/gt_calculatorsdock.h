/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatorsdock.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORSDOCK_H
#define GTCALCULATORSDOCK_H

#include "gt_dockwidget.h"

/**
 * @brief The GtCalculatorsDock class
 */
class GtCalculatorsDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtCalculatorsDock();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

};

#endif // GTCALCULATORSDOCK_H
