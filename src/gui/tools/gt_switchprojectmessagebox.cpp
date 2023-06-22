/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_switchprojectmessagebox.cpp
 *
 *  Created on: 14.03.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include <QIcon>
#include "gt_icons.h"
#include "gt_switchprojectmessagebox.h"

GtSwitchProjectMessageBox::GtSwitchProjectMessageBox(QWidget* parent) :
    QMessageBox(parent)
{
    setIcon(QMessageBox::Question);
    setWindowTitle("Confirm Switch Project");
    setWindowIcon(gt::gui::icon::jumpTo());
    setText(QString("Switch project?"));
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
}
