/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_checkforupdatesdialog.cpp
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCONFIRMDELETEPROJECTDIALOG_H
#define GTCONFIRMDELETEPROJECTDIALOG_H


#include "gt_dialog.h"

class QCheckBox;

/**
 * @brief The GtConfirmDeleteProjectDialog class
 *
 *  Dialog for confirmation to delete Project from session
 *  and if choosen from harddisk aswell
 */
class GtConfirmDeleteProjectDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget
     */
    explicit GtConfirmDeleteProjectDialog(QWidget* parent = nullptr,
                                          const QString& projectName = "");

    /**
     * @brief deleteFromHardDisk
     *  gives the status of the checkBox
     * @return
     */
    bool deleteFromHardDisk();

private:
    /// checkBox for deleting from hard disk
    QCheckBox* m_checkBox;

};

#endif // GTCONFIRMDELETEPROJECTDIALOG_H
