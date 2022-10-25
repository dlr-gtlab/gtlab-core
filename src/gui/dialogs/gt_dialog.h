/* GTlab - Gas Turbine laboratory
 * Source File: gt_dialog.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 04.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTDIALOG_H
#define GTDIALOG_H

#include <QDialog>

#include "gt_gui_exports.h"

/**
 * @brief The GtDialog class
 * GTlab base class for dialogs
 */
class GT_GUI_EXPORT GtDialog: public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief GtDialog
     * Constructor - initialize some basic settings for gtlab standard dialogs
     * e.g. the help hint in the right upper corner is disabled
     * @param parent - paretn object
     */
    explicit GtDialog(QWidget* parent = nullptr);

    /**
     * @brief setWindowTitle
     * @param title
     * @return reference to the objec
     */
    GtDialog& setWindowTitle(QString const& title);

    /**
     * @brief setWindowIcon
     * @param icon
     * @return reference to the objec
     */
    GtDialog& setWindowIcon(QIcon const& icon);
};

#endif // GTDIALOG_H
