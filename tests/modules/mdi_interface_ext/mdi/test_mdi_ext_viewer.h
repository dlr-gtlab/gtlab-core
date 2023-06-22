/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TESTMDIEXTVIEWER_H
#define TESTMDIEXTVIEWER_H

#include "gt_mdiitem.h"

/**
 * @brief The TestMdiExtViewer class
 */
class TestMdiExtViewer : public GtMdiItem
{
    Q_OBJECT

public:

    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMdiExtViewer();

    /**
     * @brief Test that multiple one instance of this item can be created
     * @return true
     */
    bool allowsMultipleInstances() const override;

private:

};

#endif // TESTMDIEXTVIEWER_H
