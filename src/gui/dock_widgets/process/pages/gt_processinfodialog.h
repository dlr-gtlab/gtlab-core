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
#include <QFrame>
#include "gt_processoverviewmodel.h"

class QLabel;
class QPushButton;

/**
 * @brief The GtProcessInfoDialog class
 * Simple dialog to display Process element information
 */
class GT_GUI_EXPORT GtProcessInfoPopup : public QFrame
{
    Q_OBJECT

public:
    explicit GtProcessInfoPopup(
        processInformation const& processInfos,
        QWidget* parent = nullptr);
};

#endif // GT_PROCESSINFODIALOG_H
