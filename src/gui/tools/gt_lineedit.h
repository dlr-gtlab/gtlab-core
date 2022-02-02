/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLINEEDIT_H
#define GTLINEEDIT_H

#include <QLineEdit>
#include "gt_gui_exports.h"

/**
 * @brief The GtLineEdit class
 */
class GT_GUI_EXPORT GtLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * @brief GtLineEdit
     * @param parent
     */
    explicit GtLineEdit(QWidget* parent = nullptr);

protected:

    /**
     * @brief focusInEvent overloaded to emit the focusIn event
     * @param event
     */
    virtual void focusInEvent(QFocusEvent* event) override;

    /**
     * @brief focusOutEvent overloaded to emit the focusOut events
     * @param event
     */
    virtual void focusOutEvent(QFocusEvent* event) override;

signals:

    /**
     * @brief focusIn emitted once the lineedit receives the focus for input
     */
    void focusIn();

    /**
     * @brief focusOut emitted once the lineedit looses focus and contains any
     * text
     */
    void focusOut();

    /**
     * @brief clearFocusOut emitted once the lineedit looses focus and does not
     * contain any text
     */
    void clearFocusOut();
};

#endif // GTLINEEDIT_H
