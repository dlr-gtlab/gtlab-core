/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.12.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTDELETEITEMMESSAGEBOX_H
#define GTDELETEITEMMESSAGEBOX_H

#include "gt_gui_exports.h"
#include <QMessageBox>

/**
 * @brief The GtDeleteItemMessageBox class -
 * MessageBox for the process of deleting item in session, perspective ...-list
 */
class GT_GUI_EXPORT GtDeleteItemMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    /**
     * @brief GtDeleteItemMessageBox - Constructor
     * Gives it the specific appearance for the usage
     * @param title - the title line of the message box
     * @param elementName - the name of the element mentioned to be deleted
     * @param parent - use the specific widget as parent
     */
    GtDeleteItemMessageBox(const QString& title, const QString& elementName,
                           QWidget* parent = nullptr);
};

#endif // GTDELETEITEMMESSAGEBOX_H
