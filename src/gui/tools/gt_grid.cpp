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

#include "gt_finally.h"
#include "gt_grid.h"
#include "gt_colors.h"
#include "gt_logging.h"

#include <QPainter>
GT_DEPRECATED_REMOVED_IN(2, 2, "QGraphicsView include not needed");
#include <QGraphicsView>

#include <cmath>

struct GtGrid::Impl
{
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

    QTransform m_cachedTransform{};

    GtGridSpacing config{};

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

    double getScaledGrid(Qt::Orientation orientation) const
    {
        auto pow2 = [](double x){ return x * x; };

        double length = orientation == Qt::Horizontal ? hspacing : vspacing;
        return length;

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
    template <unsigned Capacity>
    class BufferedLineRender
    {
    public:
        explicit BufferedLineRender(QPainter& painter) :
            m_painter(&painter)
        {}

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
        QPainter* m_painter{};
        unsigned m_count{0};
        QLineF m_lines[Capacity];
    };

    template <unsigned N>
    static void paintGridLinesImpl(const QRectF& sceneRect,
                            double vLineDistance,
                            double hLineDistance,
                            BufferedLineRender<N>& buffer)
    {
        assert(vLineDistance > 0);
        assert(hLineDistance > 0);

        double bottom  = sceneRect.bottom();
        double right = sceneRect.right();
        double left = sceneRect.left();
        double top = sceneRect.top();

        double leftStart = std::floor(left / vLineDistance) * vLineDistance;
        double topStart = std::floor(top / hLineDistance) * hLineDistance;

        for (double x = leftStart; x < right; x += vLineDistance)
        {
            buffer.draw(QLineF{x, top, x, bottom});
        }
        for (double y = topStart; y < bottom; y += hLineDistance)
        {
            buffer.draw(QLineF{left, y, right, y});
        }
    }

    void paintGridLines(QPainter& painter, const QRectF& rect)
    {
        constexpr double pixelDensityTooDenseBase = 20.0;
        constexpr double pixelDensityTooSparseMinorBase = 100.0;
        constexpr double pixelDensityTooSparseBase = 200.0;
        constexpr int minLod = -3;
        constexpr int maxLod =  3;

        const double pixelsPerSceneUnit = std::abs(painter.worldTransform().m11());

        if (painter.worldTransform() != m_cachedTransform)
        {
            m_cachedTransform = painter.worldTransform();

            const double pixelDensityTooDense  = pixelDensityTooDenseBase;
            const double pixelDensityTooSparse = pixelDensityTooSparseBase;

            int lod = 0;
            double majorHSpacing = getScaledGrid(Qt::Horizontal);
            double majorVSpacing = getScaledGrid(Qt::Vertical);
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
                majorHSpacing /= 10;
                majorVSpacing /= 10;
                majorPixelDistance = majorHSpacing * pixelsPerSceneUnit;
            }

            config.hSpacing = majorHSpacing;
            config.vSpacing = majorVSpacing;
        }

        const double pixelDensityTooSparseMinor = pixelDensityTooSparseMinorBase;
        const double majorPixelDistance = pixelsPerSceneUnit * config.hSpacing;

//        gtDebug() << majorPixelDistance << pixelDensityTooSparseMinor << pixelsPerSceneUnit << config.hSpacing;

        bool showMinor = showMinorGrid && (majorPixelDistance >= pixelDensityTooSparseMinor);

        BufferedLineRender<500> buffer{painter};
        QPen pen = painter.pen();
        pen.setCosmetic(true);

        if (showMinor)
        {
            pen.setColor(minorLineColor);
            painter.setPen(pen);

            double minorHSpacing = config.hSpacing / minorHLineCount;
            double minorVSpacing = config.vSpacing / minorVLineCount;

            paintGridLinesImpl(rect, minorHSpacing, minorVSpacing, buffer);
            buffer.flush();
        }

        pen.setColor(majorLineColor);
        painter.setPen(pen);

        paintGridLinesImpl(rect, config.hSpacing, config.vSpacing, buffer);
    }

    void paintAxis(QPainter& painter, const QRectF& rect)
    {
        for (Qt::Orientation axis : { Qt::Vertical, Qt::Horizontal })
        {
            if (!visibleAxis.testFlag(axis)) continue;

            QPen pen = painter.pen();
            pen.setCosmetic(true);
            pen.setColor(axisColor);
            painter.setPen(pen);

            switch (axis)
            {
            case Qt::Vertical:
                painter.drawLine(0.0, rect.top() + 1, 0.0, rect.bottom() - 1);
                break;
            case Qt::Horizontal:
                painter.drawLine(rect.left() - 1, 0.0, rect.right() + 1, 0.0);
                break;
            }
        }
    }

};

GtGrid::GtGrid(QGraphicsView& view) :
    GtGrid(&view)
{ }

GtGrid::GtGrid(QObject* parent) :
    QObject(parent),
    pimpl(std::make_unique<Impl>())
{
    GT_REMOVAL_GUARD(2, 2, "remove this connection");
    connect(this, &GtGrid::visibilityChanged, this, [this](){ emit update(); });
}

GtGrid::~GtGrid() = default;

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

GtGridSpacing
GtGrid::scaledMajorSpacing() const
{
    return pimpl->config;
}

GtGridSpacing
GtGrid::scaledMinorSpacing() const
{
    return GtGridSpacing{
        pimpl->config.hSpacing / pimpl->minorHLineCount,
        pimpl->config.vSpacing / pimpl->minorVLineCount
    };
}

void
GtGrid::setScaleGrid(bool val)
{
    pimpl->scaleGrid = val;
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
GtGrid::paint(QPainter& painter, const QRectF& rect, PaintOptions options)
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
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    double tmpWidth  = pimpl->getScaledGrid(Qt::Horizontal);
    double tmpHeight = pimpl->getScaledGrid(Qt::Vertical);
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
