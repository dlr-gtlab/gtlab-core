/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.12.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include <QIcon>
#include "gt_icons.h"

#include "gt_deleteitemmessagebox.h"

GtDeleteItemMessageBox::GtDeleteItemMessageBox(const QString& title,
                                               const QString& elementName,
                                               QWidget* parent) :
    QMessageBox(parent)
{
    setIcon(QMessageBox::Question);
    setWindowTitle(title);
    setWindowIcon(gt::gui::icon::delete_());
    setText(tr("Delete") + " " + elementName + "?");
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Yes);
}
