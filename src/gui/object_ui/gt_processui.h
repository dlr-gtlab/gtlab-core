/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSUI_H
#define GTPROCESSUI_H

#include "gt_objectui.h"

/**
 * @brief The GtProcessUI class
 */
class GtProcessUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessUI
     */
    Q_INVOKABLE GtProcessUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    QIcon icon(GtObject* obj) const override;

    /**
     * @brief openWith
     * @param obj
     * @return
     */
    QStringList openWith(GtObject* obj) override;

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

#endif // GTPROCESSUI_H
