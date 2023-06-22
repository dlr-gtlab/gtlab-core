/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORUI_H
#define GTCALCULATORUI_H

#include "gt_gui_exports.h"

#include "gt_objectui.h"

/**
 * @brief The GtCalculatorUI class
 */
class GT_GUI_EXPORT GtCalculatorUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtCalculatorUI
     */
    Q_INVOKABLE GtCalculatorUI();

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

};

#endif // GTCALCULATORUI_H
