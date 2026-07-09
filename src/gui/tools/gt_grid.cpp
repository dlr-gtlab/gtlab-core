/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gtd_grid.cpp
 *
 *  Created on: 17.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 *		  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include "gt_finally.h"
#include "gt_grid.h"
#include "gt_guiutilities.h"
#include "gt_ruler.h"
#include "gt_colors.h"
#include "gt_logging.h"
#include "gt_application.h"

#include <QGraphicsView>
#include <QPointer>
#include <QResizeEvent>

#include <cmath>

struct GtGrid::Impl
{
    Impl(QGraphicsView& v) : view(&v) {}

    /// GtdGraphicsView
    QPointer<QGraphicsView> view;

    /// Grid horizontal spacing
    unsigned hspacing = 100;

    /// Grid vertical spacing
    unsigned vspacing = 100;

    /// Grid horizontal lines inbetween major horizontal lines
    unsigned minorHLineCount = 10;

    /// Grid vertical lines inbetween major vertical lines
    unsigned minorVLineCount = 10;

    /// Major grid line color
    QColor majorLineColor = gt::gui::color::gridLine();

    /// Minor grid line color
    QColor minorLineColor = gt::gui::color::gridLineMinor();

    /// Axis color
    QColor axisColor = gt::gui::color::gridAxis();

    /// Scene rect
    QRectF rect{};

    /// Horizontal ruler
    std::unique_ptr<GtRuler> hRuler{};

    /// Vertical ruler
    std::unique_ptr<GtRuler> vRuler{};

    /// Axis indicator
    VisibleAxis visibleAxis{};

    /// Grid scaling factor
    int gridFactor = 0;

    /// Grid points indicator
    bool showMinorGrid = true;

    /// Hide grid
    bool isVisible = true;

    /// Grid scale indicator
    bool scaleGrid = true;

    /// Grid scale indicator
    bool ownsHRuler = true;

    /// Grid scale indicator
    bool ownsVRuler = true;

    double getScaledGrid(Qt::Orientation orientation)
    {
        auto pow2 = [](double x){ return x * x; };

        double length = orientation == Qt::Horizontal ? hspacing : vspacing;

        if (scaleGrid && gridFactor != 0)
        {
            if (gridFactor > 0)
            {
                return (length * pow2(gridFactor + 1));
            }

            return (length * pow2(gridFactor));
        }

        return length;
    }

    /// helper class that renders multiple lines with few draw calls
    template <int Capacity = 200>
    class BufferedLineRender
    {
    public:
        explicit BufferedLineRender(QPainter& painter) : m_painter(&painter), m_count(0) {}

        void draw(const QLineF& line)
        {
            m_lines[m_count++] = line;
            if (m_count == Capacity) flush();
        }

        void flush()
        {
            if (m_count > 0)
            {
                m_painter->drawLines(m_lines, m_count);
                m_count = 0;
            }
        }

        ~BufferedLineRender() { flush(); }

    private:
        QPainter* m_painter;
        QLineF m_lines[Capacity];
        int m_count = 0;
    };

    template <int NLinesPrealloc>
    void paintGridLinesImpl(const QRectF& sceneRect,
                            double vLineDistance,
                            double hLineDistance,
                            const QColor& lineColor,
                            QPainter& painter)
    {
        assert(vLineDistance > 0);
        assert(hLineDistance > 0);

        QPen pen = painter.pen();
        pen.setCosmetic(true);
        pen.setColor(lineColor);
        painter.setPen(pen);

        double firstVLineXPos = int64_t(sceneRect.left()) -
                                (int64_t(sceneRect.left()) %
                                 static_cast<int64_t>(std::ceil(vLineDistance)));
        double firstHLineYPos = int64_t(sceneRect.top()) -
                                (int64_t(sceneRect.top()) %
                                 static_cast<int64_t>(std::ceil(hLineDistance)));

        BufferedLineRender<NLinesPrealloc> hBuffer{painter};
        BufferedLineRender<NLinesPrealloc> vBuffer{painter};

        int numHLines = static_cast<int>(std::ceil((sceneRect.bottom() - firstHLineYPos) / hLineDistance));
        for (int i = 0; i < numHLines; ++i)
        {
            double y = firstHLineYPos + i * hLineDistance;
            hBuffer.draw(QLineF(sceneRect.left(), y, sceneRect.right(), y));
        }

        int numVLines = static_cast<int>(std::ceil((sceneRect.right() - firstVLineXPos) / vLineDistance));
        for (int i = 0; i < numVLines; ++i)
        {
            double x = firstVLineXPos + i * vLineDistance;
            vBuffer.draw(QLineF(x, sceneRect.top(), x, sceneRect.bottom()));
        }
    }

    void paintGridLines(QPainter& painter, const QRectF& rect)
    {
        constexpr double pixelDensityTooDense = 10.0;
        constexpr double pixelDensityTooSparse = 100.0;
        constexpr int minLod = -3;
        constexpr int maxLod = 3;

        double baseHSpacing = getScaledGrid(Qt::Horizontal);
        double baseVSpacing = getScaledGrid(Qt::Vertical);

        double pixelsPerSceneUnit = std::abs(painter.worldTransform().m11());

        int lod = 0;
        double majorHSpacing = baseHSpacing;
        double majorVSpacing = baseVSpacing;
        double majorPixelDistance = majorHSpacing * pixelsPerSceneUnit;

        while (majorPixelDistance < pixelDensityTooDense && lod > minLod)
        {
            lod--;
            majorHSpacing *= 10;
            majorVSpacing *= 10;
            majorPixelDistance = majorHSpacing * pixelsPerSceneUnit;
        }

        while (majorPixelDistance > pixelDensityTooSparse && lod < maxLod)
        {
            lod++;
            majorHSpacing *= 0.1;
            majorVSpacing *= 0.1;
            majorPixelDistance = majorHSpacing * pixelsPerSceneUnit;
        }

        bool showMinor = showMinorGrid && (majorPixelDistance >= pixelDensityTooDense);

        if (showMinor)
        {
            double minorHSpacing = majorHSpacing / minorHLineCount;
            double minorVSpacing = majorVSpacing / minorVLineCount;

            paintGridLinesImpl<1000>(rect,
                                     minorHSpacing, minorVSpacing,
                                     minorLineColor, painter);
        }

        paintGridLinesImpl<200>(rect,
                                majorHSpacing, majorVSpacing,
                                majorLineColor, painter);
    }

    void paintAxis(QPainter& painter, const QRectF& rect)
    {
        for (Qt::Orientation axis : { Qt::Vertical, Qt::Horizontal })
        {
            if (!visibleAxis.testFlag(axis)) continue;

            QPen pen = painter.pen();
            pen.setColor(axisColor);
            painter.setPen(pen);

            switch (axis)
            {
            case Qt::Vertical:
                painter.drawLine(rect.left(), 0.0, rect.right(), 0.0);
                break;
            case Qt::Horizontal:
                painter.drawLine(0.0, rect.top(), 0.0, rect.bottom());
                break;
            }
        }
    }

    void paintRuler(GtRuler& ruler)
    {
        assert(view);

        if (!rect.isValid()) return;

        if (ruler.buffer().isNull())
        {
            gtWarning().verbose() << "GtGrid::paintRuler"
                                  << tr("WARNING: buffer == NULL");
            return;
        }

        QPainter painter(&ruler.buffer());

        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(ruler.buffer().rect(),
                         ruler.palette().color(QPalette::Window));
        painter.setPen(gt::gui::color::text());

        QRect bufferRect = ruler.buffer().rect();
        painter.setClipRect(bufferRect);

        auto drawHorizontalTicks = [&](int tickSpacing, int left, int right, int height)
        {
            int firstTickX = left - (left % tickSpacing);
            int numTicks = static_cast<int>(std::ceil(static_cast<double>(right - firstTickX) / tickSpacing));

            for (int i = 0; i < numTicks; ++i)
            {
                int x = firstTickX + i * tickSpacing;
                QPoint tmp = view->mapFromScene(QPointF(x, 0));
                int vpX = tmp.x();
                if (vpX < 0 || vpX > bufferRect.right()) continue;
                painter.drawLine(vpX, height - 5, vpX, height);

                QString tick = QString::number(x);

                const QSize size = ruler.getFontSizeHint(tick);
                QRect r(vpX - size.width() / 2, height - 5 - size.height(),
                        size.width(), size.height());
                painter.setFont(ruler.getFont());
                painter.drawText(r, Qt::AlignTop | Qt::AlignHCenter, tick);
            }
        };

        auto drawVerticalTicks = [&](int tickSpacing, int top, int bottom, int width)
        {
            int firstTickY = top - (top % tickSpacing);
            int numTicks = static_cast<int>(std::ceil(static_cast<double>(bottom - firstTickY) / tickSpacing));

            for (int i = 0; i < numTicks; ++i)
            {
                double y = firstTickY + i * tickSpacing;
                QPoint tmp = view->mapFromScene(QPointF(0, y));
                int vpY = tmp.y();
                if (vpY < 0 || vpY > bufferRect.bottom()) continue;
                painter.drawLine(width - 5, vpY, width, vpY);
            }

            for (int i = 0; i < numTicks; ++i)
            {
                int y = firstTickY + i * tickSpacing;
                QPoint tmp = view->mapFromScene(QPointF(0, y));

                int vpY = tmp.y();
                if (vpY < 0 || vpY > bufferRect.bottom()) continue;

                QString tick = QString::number(-y);

                const QSize size = ruler.getFontSizeHint(tick);
                QRect r(width - 5 - size.width(), vpY + size.width() / 2 + 5,
                        size.width(), size.height());
                painter.setFont(ruler.getFont());
                painter.save();
                painter.translate(r.x(), r.y());
                painter.rotate(-90);
                painter.drawText(QRect(0, 0, r.height(), r.width()),
                                 Qt::AlignTop | Qt::AlignHCenter, tick);
                painter.restore();
            }
        };

        if (ruler.orientation() == Qt::Horizontal)
        {
            int tmpWidth = static_cast<int>(std::ceil(getScaledGrid(Qt::Horizontal)));
            assert(tmpWidth > 0);
            drawHorizontalTicks(tmpWidth, static_cast<int>(rect.left()), static_cast<int>(rect.right()), ruler.buffer().height());
        }
        else
        {
            int tmpHeight = static_cast<int>(std::ceil(getScaledGrid(Qt::Vertical)));
            assert(tmpHeight > 0);
            drawVerticalTicks(tmpHeight, static_cast<int>(rect.top()), static_cast<int>(rect.bottom()), ruler.buffer().width());
        }

        ruler.update();
    }
};

GtGrid::GtGrid(QGraphicsView& view) :
    QObject(&view),
    pimpl(std::make_unique<Impl>(view))
{
    GT_REMOVAL_GUARD(2, 2, "remove this connection");
    connect(this, &GtGrid::visibilityChanged, this, &GtGrid::update);

    if (auto* vp = view.viewport())
    {
        vp->installEventFilter(this);
    }
}

GtGrid::~GtGrid()
{
    GT_REMOVAL_GUARD(2, 2, "In the future, both rulers should be owned by this class");
    if (!pimpl->ownsHRuler) pimpl->hRuler.release();
    if (!pimpl->ownsVRuler) pimpl->vRuler.release();
}

QGraphicsView&
GtGrid::view()
{
    assert(pimpl->view);
    return *pimpl->view;
}

QGraphicsView const&
GtGrid::view() const
{
    assert(pimpl->view);
    return *pimpl->view;
}

void
GtGrid::setHSpacing(unsigned value)
{
    pimpl->hspacing = value;
}

unsigned
GtGrid::hSpacing() const
{
    return pimpl->hspacing;
}

void
GtGrid::setVSpacing(unsigned value)
{
    pimpl->vspacing = value;
}

unsigned
GtGrid::vSpacing() const
{
    return pimpl->vspacing;
}

void
GtGrid::setScaleGrid(bool val)
{
    pimpl->scaleGrid = val;
}

void
GtGrid::setShowAxis(bool show)
{
    setVisibleAxis(show, Qt::Horizontal);
}

void
GtGrid::setVisibleAxis(bool show, VisibleAxis axis)
{
    if (axis.testFlag(Qt::Vertical))
    {
        pimpl->visibleAxis.setFlag(Qt::Vertical, show);
    }
    if (axis.testFlag(Qt::Horizontal))
    {
        pimpl->visibleAxis.setFlag(Qt::Horizontal, show);
    }
}

GtGrid::VisibleAxis
GtGrid::visibleAxis() const
{
    return pimpl->visibleAxis;
}

void
GtGrid::setShowMinorGrid(bool show)
{
    pimpl->showMinorGrid = show;
}

bool
GtGrid::showMinorGrid() const
{
    return pimpl->showMinorGrid;
}

void
GtGrid::setMinorHLineCount(unsigned count)
{
    pimpl->minorHLineCount = std::max(count, 1u);
}

unsigned
GtGrid::minorHLineCount() const
{
    return pimpl->minorHLineCount;
}

void
GtGrid::setMinorVLineCount(unsigned count)
{
    pimpl->minorVLineCount = std::max(count, 1u);
}

unsigned
GtGrid::minorVLineCount() const
{
    return pimpl->minorVLineCount;
}

void
GtGrid::setLineColor(const QColor& color)
{
    pimpl->majorLineColor = color;
}

QColor
GtGrid::lineColor() const
{
    return pimpl->majorLineColor;
}

void
GtGrid::setMinorLineColor(const QColor& color)
{
    pimpl->minorLineColor = color;
}

QColor
GtGrid::minorLineColor() const
{
    return pimpl->minorLineColor;
}

void
GtGrid::setAxisColor(const QColor& color)
{
    pimpl->axisColor = color;
}

QColor
GtGrid::axisColor() const
{
    return pimpl->axisColor;
}

void
GtGrid::paint(QPainter& painter, const QRectF& rect, PaintOptions options) const
{
    if (!rect.isValid()) return;

    painter.save();
    auto finally = gt::finally([&painter]{ painter.restore(); });
    Q_UNUSED(finally);

    painter.setRenderHint(QPainter::Antialiasing, false);

    if (pimpl->isVisible)
    {
        if (options.testFlag(PaintGrid)) pimpl->paintGridLines(painter, rect);
        if (options.testFlag(PaintAxis)) pimpl->paintAxis(painter, rect);
    }
    if (options.testFlag(PaintHRuler) && pimpl->hRuler && pimpl->hRuler->needsRepaint())
    {
        pimpl->rect = rect;
        pimpl->paintRuler(*pimpl->hRuler);
        pimpl->hRuler->setNeedsRepaint(false);
    }
    if (options.testFlag(PaintVRuler) && pimpl->vRuler && pimpl->vRuler->needsRepaint())
    {
        pimpl->rect = rect;
        pimpl->paintRuler(*pimpl->vRuler);
        pimpl->vRuler->setNeedsRepaint(false);
    }
}

void
GtGrid::paintRuler(GtRuler* ruler) const
{
    if (!ruler) return;

    pimpl->paintRuler(*ruler);
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    int64_t tmpWidth  = static_cast<int64_t>(pimpl->getScaledGrid(Qt::Horizontal));
    int64_t tmpHeight = static_cast<int64_t>(pimpl->getScaledGrid(Qt::Vertical));
    assert(tmpWidth != 0);
    assert(tmpHeight != 0);

    double xV = floor( p.x() / tmpWidth ) * tmpWidth;
    double yV = floor( p.y() / tmpHeight ) * tmpHeight;

    return {xV, yV};
}

QPointF
GtGrid::computeNearestGridPoint(const QPointF& p)
{
    double tmpWidth  = pimpl->getScaledGrid(Qt::Horizontal);
    double tmpHeight = pimpl->getScaledGrid(Qt::Vertical);
    assert(tmpWidth  != 0);
    assert(tmpHeight != 0);

    double x = std::floor(p.x() / tmpWidth) * tmpWidth;
    double y = std::floor(p.y() / tmpHeight) * tmpHeight;

    if ((p.x() - x) > (tmpWidth * 0.5)) x += tmpWidth;
    if ((p.y() - y) > (tmpHeight * 0.5)) y += tmpHeight;

    return {x, y};
}

void
GtGrid::setGridScaleFactor(int val)
{
    pimpl->gridFactor = val;

    if (pimpl->hRuler)
    {
        pimpl->hRuler->setNeedsRepaint(true);
    }

    if ( pimpl->vRuler)
    {
        pimpl->vRuler->setNeedsRepaint(true);
    }
}

void
GtGrid::setHorizontalRuler(GtRuler* ruler)
{
    pimpl->hRuler.reset(ruler);
    pimpl->ownsHRuler = false;
}

void
GtGrid::setHRuler(std::unique_ptr<GtRuler> ruler)
{
    pimpl->hRuler = std::move(ruler);
    pimpl->ownsHRuler = true;
}

GtRuler*
GtGrid::hRuler() const
{
    return pimpl->hRuler.get();
}

std::unique_ptr<GtRuler>
GtGrid::releaseHRuler()
{
    return std::move(pimpl->hRuler);
}

void
GtGrid::setVerticalRuler(GtRuler* ruler)
{
    pimpl->vRuler.reset(ruler);
    pimpl->ownsVRuler = false;
}

void
GtGrid::setVRuler(std::unique_ptr<GtRuler> ruler)
{
    pimpl->vRuler = std::move(ruler);
    pimpl->ownsVRuler = true;
}

GtRuler*
GtGrid::vRuler() const
{
    return pimpl->vRuler.get();
}

std::unique_ptr<GtRuler>
GtGrid::releaseVRuler()
{
    return std::move(pimpl->vRuler);
}

bool
GtGrid::isVisible() const
{
    return pimpl->isVisible;
}

void
GtGrid::setVisible(bool visible)
{
    pimpl->isVisible = visible;
    emit visibilityChanged(visible);
}

void
GtGrid::hide()
{
    setVisible(false);
}

void
GtGrid::show()
{
    setVisible(true);
}

double
GtGrid::getScaledGridWidth()
{
    return pimpl->getScaledGrid(Qt::Horizontal);
}

double
GtGrid::getScaledGridHeight()
{
    return pimpl->getScaledGrid(Qt::Vertical);
}

void
GtGrid::resizeRulerBuffer(GtRuler& ruler, const QSize& vpSize)
{
    constexpr int rulerThickness = 30;

    QSize bufferSize = (ruler.orientation() == Qt::Horizontal)
                           ? QSize(vpSize.width(), rulerThickness)
                           : QSize(rulerThickness, vpSize.height());

    if (ruler.buffer().size() != bufferSize)
    {
        ruler.buffer() = ruler.buffer().scaled(bufferSize, Qt::IgnoreAspectRatio);
    }
}

void
GtGrid::markRulersForRepaint()
{
    if (pimpl->hRuler) pimpl->hRuler->setNeedsRepaint(true);
    if (pimpl->vRuler) pimpl->vRuler->setNeedsRepaint(true);
}

bool
GtGrid::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize)
    {
        auto* vp = pimpl->view ? pimpl->view->viewport() : nullptr;
        if (watched == vp && vp)
        {
            const QSize vpSize(vp->width(), vp->height());
            if (pimpl->hRuler) resizeRulerBuffer(*pimpl->hRuler, vpSize);
            if (pimpl->vRuler) resizeRulerBuffer(*pimpl->vRuler, vpSize);
        }
    }
    return QObject::eventFilter(watched, event);
}
