/* GTlab - Gas Turbine laboratory
 * Source File: gt_dialog.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_dialog.h"

GtDialog::GtDialog(QWidget* parent) :
    QDialog(parent)
{
    /// Turn  off the "?"-Button in the header
    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);
}

GtDialog&
GtDialog::setWindowTitle(const QString& title)
{
    QDialog::setWindowTitle(title);
    return *this;
}

GtDialog&
GtDialog::setWindowIcon(const QIcon& icon)
{
    QDialog::setWindowIcon(icon);
    return *this;
}
