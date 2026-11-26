/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_elidedlineedit.h"
#include <QFontMetrics>
#include <QLineEdit>
#include <QStyleOptionFrame>
#include <QStylePainter>

void
GtElidedLineEdit::paintEvent(QPaintEvent* event)
{
    const QString fullPath = text();
    if (hasFocus() || fullPath.isEmpty())
    {
        QLineEdit::paintEvent(event);
        return;
    }

    QStyleOptionFrame frame;
    initStyleOption(&frame);

    constexpr int horizontalInset = 2;
    const QRect textRect =
        style()
            ->subElementRect(QStyle::SE_LineEditContents, &frame, this)
            .marginsRemoved(textMargins())
            .adjusted(horizontalInset, 0, -horizontalInset, 0);
    const QString preview =
        fontMetrics().elidedText(fullPath, Qt::ElideMiddle, textRect.width());

    if (preview == fullPath || textRect.isEmpty())
    {
        QLineEdit::paintEvent(event);
        return;
    }

    QStylePainter painter(this);
    painter.drawPrimitive(QStyle::PE_PanelLineEdit, frame);
    painter.setClipRect(textRect);
    const int flags =
        int(QStyle::visualAlignment(layoutDirection(), alignment())) |
        Qt::TextSingleLine;
    painter.drawItemText(textRect, flags, frame.palette, isEnabled(), preview,
                         QPalette::Text);
}

void
GtElidedLineEdit::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    update();
}

void
GtElidedLineEdit::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    update();
}
