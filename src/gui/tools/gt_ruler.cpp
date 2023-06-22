/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_ruler.cpp
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#include "gt_ruler.h"
#include "gt_graphicsview.h"
#include "gt_grid.h"
#include "gt_application.h"

GtRuler::GtRuler(Qt::Orientation o) :
    m_orientation(o),
    m_buffer(20, 20, QImage::Format_ARGB32),
    m_needsRepaint(true)
{
    m_cursorArrow.moveTo(0,0);
    m_cursorArrow.lineTo(-3,-3);
    m_cursorArrow.lineTo(3,-3);
    setMinimumHeight(20);
    setMinimumWidth(20);
    //setStyleSheet("background-color:white;");
}

QFont
GtRuler::getFont() const
{
    QFont font;
    font.setPointSize(7);
    return font;
}

void
GtRuler::paintEvent(QPaintEvent* e)
{
    Qt::GlobalColor c = Qt::black;

    if (gtApp->inDarkMode())
    {
        c = Qt::white;
    }

    QPainter wPainter(this);
    wPainter.drawImage(1, 1, m_buffer);

    if (m_orientation==Qt::Horizontal) {
        wPainter.translate(m_cursorPos.x() + 1, height() -1 );
        wPainter.fillPath(m_cursorArrow, c);
    }
    else
    {
        wPainter.rotate(-90);
        wPainter.translate(-m_cursorPos.y() - 1, width() - 1);
        wPainter.fillPath(m_cursorArrow, c);
    }

    QWidget::paintEvent(e);
}

void
GtRuler::resizeEvent(QResizeEvent *e)
{
    m_buffer = m_buffer.scaled(e->size(), Qt::IgnoreAspectRatio);
    m_needsRepaint = true;
}

QImage&
GtRuler::buffer()
{
    return m_buffer;
}

Qt::Orientation
GtRuler::orientation()
{
    return m_orientation;
}

QSize
GtRuler::getFontSizeHint(const QString &str) const
{
    QFontMetrics fm(getFont());
    QSize size(int(fm.horizontalAdvance(str) * 1.5), fm.height());

    if (m_orientation == Qt::Vertical)
    {
        size.transpose();
    }

    return size;
}

void
GtRuler::setCursorPosition(const QPoint& pos)
{
    m_cursorPos = pos;
    repaint();
}

bool
GtRuler::needsRepaint()
{
    return m_needsRepaint;
}

void
GtRuler::setNeedsRepaint(bool val)
{
    m_needsRepaint = val;
}
