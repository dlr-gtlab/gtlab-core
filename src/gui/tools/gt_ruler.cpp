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
#include "gt_finally.h"
#include "gt_graphicsview.h"
#include "gt_grid.h"
#include "gt_guiutilities.h"
#include "gt_colors.h"
#include "gt_logging.h"


//GtRulerBand::GtRulerBand(QGraphicsView *view, QObject *parent)
//    : QObject(parent), pimpl->view(view) {}

//void GtRulerBand::setConfig(const GridConfig& cfg)
//{
//    pimpl->config = cfg;
//    invalidateCache();
//}

//void GtRulerBand::setBandThickness(int px)
//{
//    pimpl->thickness = px;
//    invalidateCache();
//}

//void GtRulerBand::setFont(const QFont& font)
//{
//    pimpl->font = font; invalidateCache();
//}

//void GtRulerBand::invalidateCache()
//{
//    pimpl->cache = QPixmap();
//}

//void GtRulerBand::paint(QPainter& painter)
//{
//    const QSize vpSize = pimpl->view->viewport()->size();
//    const QTransform vt = pimpl->view->viewportTransform();

//    // Cache key: viewport size + full scene->viewport transform. This captures
//    // BOTH pan and zoom in one comparison (transform() alone would miss
//    // scroll-only pans, since scrolling doesn't change the transform itself).
//    if (pimpl->cache.isNull() || vpSize != pimpl->cacheSize || vt != pimpl->cacheTransform)
//    {
//        regenerateCache(vpSize, vt);
//    }
//    painter.drawPixmap(0, 0, pimpl->cache);
//}

//void GtRulerBand::regenerateCache(const QSize& vpSize, const QTransform& vt)
//{
//    const qreal scale = viewScale(vt);
//    const GridLevels levels = computeGridLevels(pimpl->config, scale);

//    pimpl->cache = QPixmap(vpSize);
//    pimpl->cache.fill(Qt::transparent);

//    QPainter p(&pimpl->cache);
//    p.setRenderHint(QPainter::Antialiasing);
//    p.setFont(pimpl->font);

//    const QRectF sceneRect = pimpl->view->mapToScene(pimpl->view->viewport()->rect()).boundingRect();
//    const qreal spacing = levels.majorSpacing; // ruler labels track the major grid

//    p.fillRect(QRect(0, 0, vpSize.width(), pimpl->thickness), pimpl->view->palette().window());
//    p.fillRect(QRect(0, 0, pimpl->thickness, vpSize.height()), pimpl->view->palette().window());
//    p.setPen(pimpl->view->palette().color(QPalette::WindowText));

//    if (spacing > 0) {
//        const qreal left = std::floor(sceneRect.left() / spacing) * spacing;
//        for (qreal x = left; x < sceneRect.right(); x += spacing)
//        {
//            const int px = pimpl->view->mapFromScene(QPointF(x, 0)).x();
//            p.drawLine(px, 0, px, pimpl->thickness);
//            p.drawText(px + 2, pimpl->thickness - 2, QString::number(x));
//        }

//        const qreal top = std::floor(sceneRect.top() / spacing) * spacing;
//        for (qreal y = top; y < sceneRect.bottom(); y += spacing)
//        {
//            const int py = pimpl->view->mapFromScene(QPointF(0, y)).y();
//            p.drawLine(0, py, pimpl->thickness, py);
//            p.drawText(pimpl->thickness + 2, py - 2, QString::number(y));
//        }
//    }

//    pimpl->cacheSize = vpSize;
//    pimpl->cacheTransform = vt;
//}

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

    QRectF cachedSceneRect;
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
GtRuler::setGridSpacing(GtGridSpacing spacing)
{
    pimpl->spacing = spacing;
}

void
GtRuler::setCursorPosition(const QPoint& pos)
{
    pimpl->cursorPos = pos;
    repaint();
}

bool
GtRuler::needsRepaint(QRectF sceneRect, QTransform sceneTransform) const
{
    return pimpl->cache.isNull() ||
           pimpl->cachedSceneRect != sceneRect ||
           pimpl->cachedTransform == sceneTransform;
}

void
GtRuler::invalidateCache()
{
    pimpl->cachedTransform = {};
    pimpl->cachedSceneRect = {};
}

void
GtRuler::paint(QRectF sceneRect, QTransform sceneTransform, QGraphicsView& view)
{
//    if (!needsRepaint(sceneRect, sceneTransform))
//    {
//        return update();
//    }

    if (!sceneRect.isValid())
    {
        gtWarning() << "Scene Rect Invalid!";
        return;
    }

    if (pimpl->cache.isNull())
    {
        gtFatal() << "CACHE IS NULL!";
        assert(false);
        pimpl->cache = QPixmap(size());
    }

    pimpl->cachedSceneRect = sceneRect;
    pimpl->cachedTransform = sceneTransform;

    QPainter painter{&pimpl->cache};

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.fillRect(buffer().rect(), palette().color(QPalette::Window));

    painter.setPen(gt::gui::color::text());
    painter.setFont(font());

    double bottom  = sceneRect.bottom();
    double right = sceneRect.right();
    double left = sceneRect.left();
    double top = sceneRect.top();
    constexpr int tickLength = 5;

    if (orientation() == Qt::Horizontal)
    {
        double hLineDistance = pimpl->spacing.hSpacing;
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
//        const QPointF tmpStart = view.mapFromScene(QPointF{leftStart, 0});
//        const QPointF tmpEnd   = view.mapFromScene(QPointF{leftStart + hLineDistance, 0});
        const QPointF tmpStart = sceneTransform.map(QPointF{leftStart, 0});
        const QPointF tmpEnd   = sceneTransform.map(QPointF{leftStart + hLineDistance, 0});
        const double tickPosDistance = tmpEnd.x() - tmpStart.x();
        const unsigned stepSize = std::max(1, static_cast<int>(std::ceil(textRect.width() / tickPosDistance)));
        hLineDistance *= stepSize;

        // offset start-tick to make sure 0 tick is always visbile
        double rem = std::remainderl(leftStart, hLineDistance);
        leftStart -= rem;

        for (double x = leftStart; x < right; x += hLineDistance)
        {
//            const QPointF point = view.mapFromScene(QPointF{x, 0});
            const QPointF point = sceneTransform.map(QPointF{x, 0});
            painter.drawLine(point.x(), height - tickLength, point.x(), height);
            painter.drawText(textRect.translated(point.x(), 0),
                             Qt::AlignTop | Qt::AlignHCenter,
                             QString::number(x));
        }
    }
    else
    {
        painter.translate(buffer().width(), 0);
        painter.rotate(90);

        double vLineDistance = pimpl->spacing.vSpacing;
        assert(vLineDistance > 0);

        double topStart = std::floor(top / vLineDistance) * vLineDistance;

        const int height = buffer().width();

        // calc largest tick once
        const double tmpMaxValue = std::max(std::abs(topStart), std::abs(bottom));
        const QString tmpTick = QString::number(-std::ceil(tmpMaxValue) + vLineDistance);
        const QSizeF tmpSize = textSizeHint(tmpTick);
        const QRectF textRect{
            -tmpSize.width() * 0.5, height - tickLength + 1 - tmpSize.height(),
            tmpSize.width(), tmpSize.height()
        };

        // skip tick labels if they overlap
//        const QPointF tmpStart = view.mapFromScene(QPointF{topStart, 0});
//        const QPointF tmpEnd   = view.mapFromScene(QPointF{topStart + vLineDistance, 0});
        const QPointF tmpStart = sceneTransform.map(QPointF{topStart, 0});
        const QPointF tmpEnd   = sceneTransform.map(QPointF{topStart + vLineDistance, 0});
        const double tickPosDistance = tmpEnd.x() - tmpStart.x();
        const unsigned stepSize = std::max(1, static_cast<int>(std::ceil(textRect.width() / tickPosDistance)));
        vLineDistance *= stepSize;

        // offset start-tick to make sure 0 tick is always visbile
        double rem = std::remainderl(topStart, vLineDistance);
        topStart -= rem;

        for (double y = topStart; y < bottom; y += vLineDistance)
        {
//            const QPointF point = view.mapFromScene(QPointF{0, y}).transposed();
            const QPointF point = sceneTransform.map(QPointF{0, y}).transposed();
            painter.drawLine(point.x(), tickLength, point.x(), 0);
            painter.drawText(textRect.translated(point.x(), 0),
                             Qt::AlignTop | Qt::AlignHCenter,
                             QString::number(y));
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
GtRuler::resizeEvent(QResizeEvent *e)
{
    pimpl->cache = pimpl->cache.scaled(e->size(), Qt::IgnoreAspectRatio);
    invalidateCache();
}
