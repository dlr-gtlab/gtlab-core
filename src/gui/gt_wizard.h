/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_wizard.h
 *
 *  Created on: 02.12.2021
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTWIZARD_H
#define GTWIZARD_H

#include <QWizard>
#include "gt_gui_exports.h"

/**
 * @brief The GtWizard class
 * This is the tmaplate to use for Wizards in GTlab
 */
class GT_GUI_EXPORT GtWizard : public QWizard
{
    Q_OBJECT
public:
    /**
     * @brief GtWizard
     * @param parent - parent widget
     */
    explicit GtWizard(QWidget* parent);
};

#endif // GTWIZARD_H
