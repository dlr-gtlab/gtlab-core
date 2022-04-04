/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
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
    setWindowIcon(GtGUI::Icon::delete16());
    setText(tr("Delete") + " " + elementName + "?");
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Yes);
}
