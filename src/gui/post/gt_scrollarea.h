/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.03.2017
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 2579
 */
#ifndef GTSCROLLAREA_H
#define GTSCROLLAREA_H

#include "qscrollarea.h"

class GtScrollArea : public QScrollArea
{
public:
    GtScrollArea();

    void keyPressEvent(QKeyEvent* e) override;

    void wheelEvent(QWheelEvent* e) override;
};

#endif // GTSCROLLAREA_H
