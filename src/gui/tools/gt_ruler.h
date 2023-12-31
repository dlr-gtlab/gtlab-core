/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_ruler.h
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#ifndef GTD_RULER_H
#define GTD_RULER_H

#include <Qt>
#include <QFrame>
#include <QPainterPath>
#include "gt_gui_exports.h"

class GtGrid;

class GT_GUI_EXPORT GtRuler : public QWidget
{
    Q_OBJECT

public:
    explicit GtRuler(Qt::Orientation o = Qt::Horizontal);

    /** Returns Buffer.
        @return Buffer */
    QImage& buffer();

    /** Returns ruler orientation.
        @return Ruler orientation */
    Qt::Orientation orientation();

    /** Returns custom ruler font.
        @return Ruler font */
    QFont getFont() const;

    /** Returns size of given string.
        @param str String
        @return Size of string */
    QSize getFontSizeHint(const QString& str) const;

    /** Sets cursor postion to given point.
        @param pos New cursor position */
    void setCursorPosition(const QPoint& pos);

    /** Returns whether ruler needs a repaint or not.
        @return Repaint indicator */
    bool needsRepaint();

    /** Sets repaint indicator.
        @param val New repaint indicator */
    void setNeedsRepaint(bool val);

protected:
    /** Overloaded Paint Function
     * @param e paint event */
    void paintEvent(QPaintEvent* e) override;

    /** Resize Ruler scale
     * @param e resize event
     */
    void resizeEvent(QResizeEvent* e) override;

private:
    /// Orientation indicator
    Qt::Orientation m_orientation;

    /// Buffer
    QImage m_buffer;

    /// Cursor arrow
    QPainterPath m_cursorArrow;

    /// Cursor position
    QPoint m_cursorPos;

    /// Repaint indicator
    bool m_needsRepaint;
};

#endif // GTD_RULER_H
