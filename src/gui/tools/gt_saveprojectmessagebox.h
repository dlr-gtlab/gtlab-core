/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.12.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTSAVEPROJECTMESSAGEBOX_H
#define GTSAVEPROJECTMESSAGEBOX_H

#include "gt_gui_exports.h"
#include <QMessageBox>

/**
 * @brief The GtSaveProjectMessageBox class -
 * Message-Box for the Save-Project - Message
 */
class GT_GUI_EXPORT GtSaveProjectMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    /**
     * @brief GtSaveProjectMessageBox - Constructor
     * Gives it the specific appearance for the usage
     * @param parent - use the specific widget as parent
     */
    explicit GtSaveProjectMessageBox(const QString& text = QString(),
                                     QWidget *parent = nullptr);
};

#endif // GTSAVEPROJECTMESSAGEBOX_H
