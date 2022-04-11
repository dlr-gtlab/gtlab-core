/* GTlab - Gas Turbine laboratory
 * Source File: gt_switchprojectmessagebox.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 14.03.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191S
 */
#ifndef GTSWITCHPROJECTMESSAGEBOX_H
#define GTSWITCHPROJECTMESSAGEBOX_H

#include "gt_gui_exports.h"
#include <QMessageBox>

/**
 * @brief The GtSwitchProjectMessageBox class -
 * Message-Box for the Switch-Project - Message
 */
class GT_GUI_EXPORT GtSwitchProjectMessageBox : public QMessageBox
{
    Q_OBJECT

public:
    /**
     * @brief GtSwitchProjectMessageBox - Constructor
     * Gives it the specific appearance for the usage
     * @param parent - use the specific widget as parent
     */
    GtSwitchProjectMessageBox(QWidget* parent = nullptr);
};

#endif // GTSWITCHPROJECTMESSAGEBOX_H
