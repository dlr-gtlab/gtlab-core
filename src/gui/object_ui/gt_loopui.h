/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_loopui.h
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLOOPUI_H
#define GTLOOPUI_H

#include "gt_objectui.h"

/**
 * @brief The GtLoopUI class
 */
class GtLoopUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtLoopUI
     */
    Q_INVOKABLE GtLoopUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

    /**
     * @brief hasValidationRegExp
     * @return true, because element has validator
     */
    bool hasValidationRegExp() override;

    /**
     * @brief validatorRegExp
     * @return Regexp to accept letters, digits, -, _, and space
     */
    QRegExp validatorRegExp() override;

protected:
    /**
     * @brief specificData
     * @param obj
     * @param role
     * @param column
     * @return
     */
    QVariant specificData(GtObject* obj,
                          int role,
                          int column) const override;

};

#endif // GTLOOPUI_H
