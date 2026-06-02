/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processinfodialog.h
 */

#ifndef GT_PROCESSINFODIALOG_H
#define GT_PROCESSINFODIALOG_H

#include "gt_gui_exports.h"
#include <QDialog>

class QLabel;
class QPushButton;

/**
 * @brief The GtProcessInfoDialog class
 * Simple dialog to display Process element information
 */
class GT_GUI_EXPORT GtProcessInfoDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param processInfos - list of key value pairs to be shwon in the dialog
     * @param parent Parent widget
     */
    GtProcessInfoDialog(const QList<QPair<QString, QString> >& processInfos,
                        QWidget* parent = nullptr);
};

#endif // GT_PROCESSINFODIALOG_H
