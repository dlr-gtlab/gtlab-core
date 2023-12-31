/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_accessdatadialog.h
 *
 *  Created on: 30.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ACCESSDATADIALOG_H
#define GT_ACCESSDATADIALOG_H

#include "gt_dialog.h"

class QLineEdit;
class QCheckBox;
class QLabel;
class QSpinBox;
class QPushButton;
class GtAccessData;
class GtAbstractAccessDataConnection;

/**
 * @brief The GtAccessDataDialog class
 */
class GtAccessDataDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Reference to access data.
     * @param Parent widget.
     */
    GtAccessDataDialog(GtAccessData& data, const QMetaObject& connection,
                       QWidget* parent = nullptr);

private:
    ///
    GtAccessData& m_data;

    /// Host line edit
    QLineEdit* m_hostLine;

    /// Port line edit
    QSpinBox* m_portLine;

    /// Username line edit
    QLineEdit* m_userLine;

    /// Password line edit
    QLineEdit* m_pwLine;

    /// Checkbox for enable/disable user data input
    QCheckBox* m_userDataCheck;

    /// Output label
    QLabel* m_outputLabel;

    /// Save access data button
    QPushButton* m_saveBtn;

    /// Cancel button
    QPushButton* m_cancelBtn;

    /// Test conneciton button
    QPushButton* m_testConnectionBtn;

    /// Access data connection
    QMetaObject m_connectionMetaData;

    /**
     * @brief Enables/Disables buttons during connection process.
     * @param Whether buttons should be enabled or disabled.
     */
    void enableButtons(bool val);

    /**
     * @brief newConnectionObject
     * @return
     */
    GtAbstractAccessDataConnection* newConnectionObject();

private slots:
    /**
     * @brief Saves given user input to access data object and closes dialog.
     */
    void saveAccessData();

    /**
     * @brief Tests connection based on given user input.
     */
    void testConnection();

    /**
     * @brief Called after network reply of test connection was finished.
     */
    void onTestSuccessful();

    /**
     * @brief onTestFailed
     */
    void onTestFailed();

    /**
     * @brief Clears information of output label.
     */
    void clearOutput();

};

#endif // GT_ACCESSDATADIALOG_H
