/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_ruler.cpp
 *
 *  Created on: 15.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#include "gt_ruler.h"
#include "gt_grid.h"
#include "gt_colors.h"
#include "gt_logging.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>

struct GtRuler::Impl
{
    Impl(Qt::Orientation o) : orientation{o} {}

    QFont font;

    GtGridSpacing spacing;

    /// Orientation indicator
    Qt::Orientation orientation;

    /// Cache for rendered ruler
    QPixmap cache{20, 20};

    /// Cursor arrow
    QPainterPath cursorArrow;

    /// Cursor position
    QPoint cursorPos{};

    /// Repaint indicator
    QTransform cachedTransform;

    QRectF cachedRect;

    bool axisFlipped = false;
};

GtRuler::GtRuler(Qt::Orientation o) :
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
GtRuler::flipAxis(bool flipped)
{
    pimpl->axisFlipped = flipped;
}

bool
GtRuler::isAxisFlipped() const
{
    return pimpl->axisFlipped;
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
        gtWarning() << "Scene Rect Invalid!";
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

    double bottom  = backgroundRect.bottom();
    double right = backgroundRect.right();
    double left = backgroundRect.left();
    double top = backgroundRect.top();
    constexpr int tickLength = 5;

    if (orientation() == Qt::Horizontal)
    {
        double hLineDistance = spacing.hSpacing;
        assert(hLineDistance > 0);

        double leftStart = std::floor(left / hLineDistance) * hLineDistance;

        const int height = buffer().height();

        // calc largest tick once
        const double tmpMaxValue = std::max(std::abs(leftStart), std::abs(right));
        const QString tmpTick = QString::number(-std::ceil(tmpMaxValue) + hLineDistance);
        const QSizeF tmpSize = textSizeHint(tmpTick);
        const QRectF textRect{
            -tmpSize.width() * 0.5, height - tickLength - 1 - tmpSize.height(),
            tmpSize.width(), tmpSize.height()
        };

        // skip tick labels if they overlap
        const QPointF tmpStart = viewportTransform.map(QPointF{leftStart, 0});
        const QPointF tmpEnd   = viewportTransform.map(QPointF{leftStart + hLineDistance, 0});
        const double tickPosDistance = tmpEnd.x() - tmpStart.x();
        const unsigned stepSize = std::max(1, static_cast<int>(std::ceil(textRect.width() / tickPosDistance)));
        hLineDistance *= stepSize;

        // offset start-tick to make sure 0 tick is always visbile
        double rem = std::remainderl(leftStart, hLineDistance);
        leftStart -= rem;

        for (double x = leftStart; x < right; x += hLineDistance)
        {
            const QPointF point = viewportTransform.map(QPointF{x, 0});
            painter.drawLine(point.x(), height - tickLength, point.x(), height);
            painter.drawText(textRect.translated(point.x(), 0),
                             Qt::AlignTop | Qt::AlignHCenter,
                             QString::number(isAxisFlipped() ? -x : x));
        }
    }
    else
    {
        painter.rotate(90);
        painter.translate(buffer().height(), 0);
        painter.scale(-1, -1);

        double vLineDistance = spacing.vSpacing;
        assert(vLineDistance > 0);

        double topStart = std::floor(top / vLineDistance) * vLineDistance;

        // view on buffer is flipped!
        const int width  = buffer().height();
        const int height = buffer().width();

        // calc largest tick once
        const double tmpMaxValue = std::max(std::abs(topStart), std::abs(bottom));
        const QString tmpTick = QString::number(-std::ceil(tmpMaxValue) + vLineDistance);
        const QSizeF tmpSize = textSizeHint(tmpTick);
        const QRectF textRect{
            -tmpSize.width() * 0.5, height - tickLength - 1 - tmpSize.height(),
            tmpSize.width(), tmpSize.height()
        };

        // skip tick labels if they overlap
        const QPointF tmpStart = viewportTransform.map(QPointF{topStart, 0});
        const QPointF tmpEnd   = viewportTransform.map(QPointF{topStart + vLineDistance, 0});
        const double tickPosDistance = tmpEnd.x() - tmpStart.x();
        const unsigned stepSize = std::max(1, static_cast<int>(std::ceil(textRect.width() / tickPosDistance)));
        vLineDistance *= stepSize;

        // offset start-tick to make sure 0 tick is always visbile
        double rem = std::remainderl(topStart, vLineDistance);
        topStart -= rem;

        for (double y = topStart; y < bottom; y += vLineDistance)
        {
            const QPointF point = viewportTransform.map(QPointF{0, y}).transposed();
            painter.drawLine(width - point.x(), height - tickLength, width - point.x(), height);
            painter.drawText(textRect.translated(width - point.x(), 0),
                             Qt::AlignTop | Qt::AlignHCenter,
                             QString::number(isAxisFlipped() ? -y : y));
        }
    }

    update();
}

void
GtRuler::paintEvent(QPaintEvent* e)
{
    QColor c = gt::gui::color::text();

    QPainter painter{this};
    painter.drawPixmap(1, 1, pimpl->cache);

    if (pimpl->orientation == Qt::Horizontal)
    {
        painter.translate(pimpl->cursorPos.x() + 1, height() -1 );
        painter.fillPath(pimpl->cursorArrow, c);
    }
    else
    {
        painter.rotate(-90);
        painter.translate(-pimpl->cursorPos.y() - 1, width() - 1);
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
