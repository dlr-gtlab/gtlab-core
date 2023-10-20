/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_environmentdialog.h
 *
 *  Created on: 26.07.2019
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ENVIRONMENTDIALOG_H
#define GT_ENVIRONMENTDIALOG_H

#include "gt_dialog.h"

class GtEnvironmentModel;

/**
 * @brief The GtEnvironmentDialog class
 */
class GtEnvironmentDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param envVars Specific environment variables
     * @param winTitle Window title
     * @param infoText Information text
     * @param parent Parent Widget
     */
    GtEnvironmentDialog(const QStringList& envVars,
                        const QString& winTitle,
                        const QString& infoText,
                        QWidget* parent = nullptr);

private:
    /// Environment model
    GtEnvironmentModel* m_envModel;

private slots:
    /**
     * @brief Saves environment settings and accepts dialog.
     */
    void save();

};

#endif // GT_ENVIRONMENTDIALOG_H
