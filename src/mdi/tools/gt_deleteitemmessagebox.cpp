/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.12.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include <QIcon>
#include "gt_application.h"

#include "gt_deleteitemmessagebox.h"

GtDeleteItemMessageBox::GtDeleteItemMessageBox(QString title,
                                               QString elementName,
                                               QWidget* parent) :
    QMessageBox(parent)
{
    setIcon(QMessageBox::Question);
    setWindowTitle(title);
    setWindowIcon(gtApp->icon("closeIcon_16.png"));
    setText(tr("Delete") + " " + elementName + "?");
    setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    setDefaultButton(QMessageBox::Yes);
}
