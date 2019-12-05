/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkforupdatesdialog.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */


#include <QDialog>

class QCheckBox;

#ifndef GTCONFIRMDELETEPROJECTDIALOG_H
#define GTCONFIRMDELETEPROJECTDIALOG_H

/**
 * @brief The GtConfirmDeleteProjectDialog class
 *
 *  Dialog for confirmation to delete Project from session
 *  and if choosen from harddisk aswell
 */
class GtConfirmDeleteProjectDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent widget
     */
    GtConfirmDeleteProjectDialog(QWidget* parent = Q_NULLPTR,
                                 QString projectName = "");

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
