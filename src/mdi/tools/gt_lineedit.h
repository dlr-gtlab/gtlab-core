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

/**
 * @brief The GtLineEdit class
 */
class GtLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * @brief GtLineEdit
     * @param parent
     */
    GtLineEdit(QWidget* parent = Q_NULLPTR);

protected:
    virtual void focusOutEvent(QFocusEvent* event) Q_DECL_OVERRIDE;

signals:
    void focusOut();

    void clearFocusOut();

};

#endif // GTLINEEDIT_H
