/* GTlab - Gas Turbine laboratory
 * Source File: gt_switchprojectmessagebox.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.03.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include <QIcon>
#include "gt_application.h"
#include "gt_switchprojectmessagebox.h"

GtSwitchProjectMessageBox::GtSwitchProjectMessageBox(QWidget* parent) :
    QMessageBox(parent)
{
    setIcon(QMessageBox::Question);
    setWindowTitle("Confirm Switch Project");
    setWindowIcon(gtApp->icon("jumpToIcon.png"));
    setText(QString("Switch project?"));
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
}
