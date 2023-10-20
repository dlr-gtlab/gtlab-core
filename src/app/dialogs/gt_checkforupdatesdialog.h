/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_checkforupdatesdialog.h
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CHECKFORUPDATESDIALOG_H
#define GT_CHECKFORUPDATESDIALOG_H

#include "gt_dialog.h"

class QLabel;
class QPushButton;
class QTableWidget;

/**
 * @brief The GtCheckForUpdatesDialog class
 */
class GtCheckForUpdatesDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget
     */
    explicit GtCheckForUpdatesDialog(QWidget* parent = nullptr);

private:
    /// Progress label
    QLabel* m_progressLabel;

    /// Update text label
    QLabel* m_txtLabel;

    /// Check for updates button
    QPushButton* m_checkButton;

    /// Run update button
    QPushButton* m_updateButton;

    /// Table widget to show available package updates
    QTableWidget* m_tableWid;

private slots:
    /**
     * @brief checkForUpdate
     */
    void checkForUpdate();

    /**
     * @brief updateAvailable
     */
    void updateAvailable();

    /**
     * @brief noUpdateAvailable
     * @param str
     */
    void noUpdateAvailable(int errorCode, const QString& str);

};

#endif // GT_CHECKFORUPDATESDIALOG_H
