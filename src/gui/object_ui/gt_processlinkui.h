/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSLINKUI_H
#define GTPROCESSLINKUI_H

#include "gt_processui.h"

/**
 * @brief The GtProcessLinkUI class
 */
class GtProcessLinkUI : public GtProcessUI
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessLinkUI
     */
    Q_INVOKABLE GtProcessLinkUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

};

#endif // GTPROCESSLINKUI_H
