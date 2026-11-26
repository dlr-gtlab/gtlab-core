/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_ELIDEDLINEEDIT_H
#define GT_ELIDEDLINEEDIT_H

#include "gt_gui_exports.h"
#include <QLineEdit>

class GT_GUI_EXPORT GtElidedLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
};

#endif // GT_ELIDEDLINEEDIT_H
