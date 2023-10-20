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

#include "gt_saveprojectmessagebox.h"

GtSaveProjectMessageBox::GtSaveProjectMessageBox(const QString& text,
                                                 QWidget* parent) :
    QMessageBox(parent)
{
    setIcon(QMessageBox::Question);
    setWindowTitle(tr("Save Project"));
    setWindowIcon(gt::gui::icon::save());

    if (text.isEmpty())
    {
        setText(tr("Found unsaved project.\nDo you want to ") +
                tr("save all your changes before closing GTlab?"));
    }
    else
    {
        setText(text);
    }

    setStandardButtons(QMessageBox::Yes | QMessageBox::No |
                       QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Cancel);
}

