/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_ruler.cpp
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#include "gt_ruler.h"
#include "gt_colors.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>

#include <cmath>

struct GtRuler::Impl
{
    explicit Impl(Qt::Orientation o) : orientation{o} {}

    /// Cache for rendered ruler (native format for QPainter)
    QPixmap cache{20, 20};

    /// Cursor arrow path
    QPainterPath cursorArrow;

    /// Cursor position
    QPoint cursorPos{};

    /// cached view transform
    QTransform cachedTransform;

    /// cached scene rect
    QRectF cachedRect;

    /// Orientation indicator
    Qt::Orientation orientation;

    /// Offset for drawing the ruler. Needed in case the graphicsview has a
    /// frame
    int drawOffset = 0;

    /// Indicator whether the values of the axis should be flipped
    bool axisFlipped = false;
};

GtRuler::GtRuler(Qt::Orientation o, QWidget* parent) :
    QWidget(parent),
    pimpl(std::make_unique<Impl>(o))
{
    pimpl->cursorArrow.moveTo(0,0);
    pimpl->cursorArrow.lineTo(-3,-3);
    pimpl->cursorArrow.lineTo(3,-3);
    setMinimumSize(20, 20);

    if (o == Qt::Horizontal)
    {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    }
    else
    {
        flipAxis(true);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    }

    QFont f = font();
    f.setPointSize(7);
    setFont(f);
}

GtRuler::~GtRuler() = default;


QPixmap&
GtRuler::buffer()
{
    return pimpl->cache;
}

Qt::Orientation
GtRuler::orientation() const
{
    return pimpl->orientation;
}

void
GtRuler::setOrientation(Qt::Orientation o)
{
    pimpl->orientation = o;
    invalidate();
}

void
GtRuler::flipAxis(bool flipped)
{
    pimpl->axisFlipped = flipped;
}

bool
GtRuler::isAxisFlipped() const
{
    return pimpl->axisFlipped;
}

void
GtRuler::setDrawOffset(int offset)
{
    pimpl->drawOffset = offset;
}

int
GtRuler::drawOffset() const
{
    return pimpl->drawOffset;
}

QSize
GtRuler::textSizeHint(const QString& str) const
{
    QFontMetrics fm{font()};

    QSize size{
        static_cast<int>(std::ceil(fm.horizontalAdvance(str) * 1.2)),
        fm.height()
    };

    return size;
}

void
GtRuler::setCursorPosition(const QPoint& pos)
{
    pimpl->cursorPos = pos;
    repaint();
}

bool
GtRuler::needsRepaint(QRectF backgroundRect, QTransform viewportTransform) const
{
    return pimpl->cachedRect != backgroundRect ||
           pimpl->cachedTransform != viewportTransform;
}

void
GtRuler::invalidate()
{
    pimpl->cachedTransform = {};
    pimpl->cachedRect = {};
}

void
GtRuler::paint(GtGridSpacing spacing, QRectF backgroundRect, QTransform viewportTransform)
{
    if (!needsRepaint(backgroundRect, viewportTransform))
    {
        return;
    }

    if (!backgroundRect.isValid())
    {
        return;
    }

    assert(!pimpl->cache.isNull());

    pimpl->cachedRect = backgroundRect;
    pimpl->cachedTransform = viewportTransform;

    QPainter painter{&pimpl->cache};
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setRenderHint(QPainter::TextAntialiasing, false);
    painter.fillRect(buffer().rect(), palette().color(QPalette::Window));

    painter.setPen(gt::gui::color::text());
    painter.setFont(font());

    constexpr int tickLength = 5;
    constexpr int tickPadding = 1;

    double left{}, right{};
    double lineDistance{};

    int height{};

    double mult = isAxisFlipped() ? -1 : 1;

    QTransform adjustedTransform;

    switch (orientation())
    {
    case Qt::Horizontal:
        lineDistance = spacing.hSpacing;
        left   = backgroundRect.left();
        right  = backgroundRect.right();
        height = buffer().height();
        adjustedTransform = viewportTransform;
        break;

    case Qt::Vertical:
        painter.rotate(90); // rotating to operate on pixmap as if it is horizontal
        painter.translate(buffer().height(), 0); // move to end of pixmap
        painter.scale(-1, -1); // flip 180 degrees (so that the text is rotated correctly)

        lineDistance = spacing.vSpacing;
        left   = backgroundRect.top();
        right  = backgroundRect.bottom();
        height = buffer().width();

        // NOTE: black magic
        const double offset = buffer().height();
        // transform (x,y) -> (y,x)
        const QTransform swap{0, 1, 1, 0, 0, 0};
        // transform (x,y) -> (offset-x, y)
        const QTransform toOffsetX{-1, 0, 0, 1, offset, 0};

        adjustedTransform  = swap;              // (pos, 0) -> (0, pos)
        adjustedTransform *= viewportTransform; // map in scene space
        adjustedTransform *= swap;              // swap result back
        adjustedTransform *= toOffsetX;         // turn resulting y into offset-relative x
        break;
    }

    assert(lineDistance > 0);
    double leftStart = std::floor(left / lineDistance) * lineDistance;

    // calc largest tick once to determine text rect
    const double tmpMaxValue = std::max(std::abs(leftStart), std::abs(right));
    const QString tmpTick = QString::number(-std::ceil(tmpMaxValue) + lineDistance);
    const QSizeF tmpSize = textSizeHint(tmpTick);
    const QRectF textRect{
        -tmpSize.width() * 0.5 + tickPadding, height - tickLength - tickPadding - tmpSize.height(),
        tmpSize.width(), tmpSize.height()
    };

    // skip tick labels if they overlap
    const QPointF tmpStart = viewportTransform.map(QPointF{leftStart, 0});
    const QPointF tmpEnd   = viewportTransform.map(QPointF{leftStart + lineDistance, 0});
    const double tickPosDistance = tmpEnd.x() - tmpStart.x();
    assert(tickPosDistance > 0);
    const unsigned stepSize = std::max(1, static_cast<int>(std::ceil(textRect.width() / tickPosDistance)));
    lineDistance *= stepSize;

    // offset start-tick to make sure 0 tick is always visible
    double rem = std::remainderl(leftStart, lineDistance);
    leftStart -= rem;

    for (double pos = leftStart; pos < right; pos += lineDistance)
    {
        const QPointF point = adjustedTransform.map(QPointF{pos, 0});
        painter.drawLine(point.x(), height - tickLength, point.x(), height);
        painter.drawText(textRect.translated(point.x(), 0),
                         Qt::AlignTop | Qt::AlignHCenter,
                         QString::number(pos * mult));
    }

    update();
}

void
GtRuler::paintEvent(QPaintEvent* e)
{
    QColor c = gt::gui::color::text();

    QPainter painter{this};
    painter.drawPixmap(pimpl->drawOffset, pimpl->drawOffset, pimpl->cache);

    if (pimpl->orientation == Qt::Horizontal)
    {
        painter.translate(pimpl->cursorPos.x() + pimpl->drawOffset, height() - pimpl->drawOffset);
        painter.fillPath(pimpl->cursorArrow, c);
    }
    else
    {
        painter.rotate(-90);
        painter.translate(-pimpl->cursorPos.y() - pimpl->drawOffset, width() - pimpl->drawOffset);
        painter.fillPath(pimpl->cursorArrow, c);
    }

    QWidget::paintEvent(e);
}

void
GtRuler::resizeEvent(QResizeEvent* e)
{
    pimpl->cache = pimpl->cache.scaled(e->size(), Qt::IgnoreAspectRatio);
    invalidate();
}
