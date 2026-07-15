/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_grid.cpp
 *
 *  Created on: 17.10.2013
 *      Author: Stanislaus Reitenbach (AT-TW)
 */

#include "gt_grid.h"
#include "gt_finally.h"
#include "gt_colors.h"

#include <QPainter>
GT_DEPRECATED_REMOVED_IN(2, 2, "QGraphicsView include not needed");
#include <QGraphicsView>

#include <cmath>

QPen makePen(QColor color)
{
    QPen pen{color};
    pen.setCosmetic(true);
    return pen;
}

namespace strategy
{

inline double base2(double ideal, double baseSpacing)
{
    const double ratio = ideal / baseSpacing;
    const double n = std::round(std::log2(ratio));
    return baseSpacing * std::exp2(n);
}

inline double sqrt2(double ideal, double baseSpacing)
{
    static const double logSqrt2 = std::log(std::sqrt(2.0));

    double ratio = ideal / baseSpacing;
    double n = std::round(std::log(ratio) / logSqrt2);
    return baseSpacing * std::pow(std::sqrt(2.0), n);
}

inline double base10(double ideal, double baseSpacing)
{
    const double ratio = ideal / baseSpacing;
    const double n = std::round(std::log10(ratio));
    return baseSpacing * std::pow(10.0, n);
}

inline double oneTwoFive(double ideal, double /*baseSpacing*/)
{
    const double decadeExp = std::floor(std::log10(ideal));
    const double fraction   = ideal / std::pow(10.0, decadeExp); // in [1, 10)
    assert(fraction >= 1 && fraction <  10);

    const double multiplier =
            (fraction < 1.5) ? 1.0 :
            (fraction < 3.5) ? 2.0 :
            (fraction < 7.5) ? 5.0 : 10.0;

    return multiplier * std::pow(10, decadeExp);
}

} // strategy namespace

struct GtGrid::Impl
{
    /// Grid horizontal spacing
    unsigned hSpacing = 100;

    /// Grid vertical spacing
    unsigned vSpacing = 100;

    /// Grid horizontal lines inbetween major horizontal lines
    unsigned hSubdivisons = 10;

    /// Grid vertical lines inbetween major vertical lines
    unsigned vSubdivisions = 10;

    /// Pen for major grid lines
    QPen majorPen = makePen(gt::gui::color::gridLine());

    /// Pen for minor grid lines
    QPen minorPen = makePen(gt::gui::color::gridLineMinor());

    /// Pen for grid axis
    QPen axisPen = makePen(gt::gui::color::gridAxis());

    /// Current grid spacing
    GtGridSpacing currentSpacing{1.0, 1.0};

    /// Axis indicator
    VisibleAxis visibleAxis{};

    /// Grid scaling strategy
    ScalingStrategy scalingStrategy = DefaultScalingStrategy;

    /// Hide grid
    bool showGrid = true;

    /// Grid points indicator
    bool showMinorGrid = true;

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

    double quantizedSpacing(double ideal, double baseSpacing) const
    {
        switch (scalingStrategy)
        {
        case Base2:
            return strategy::base2(ideal, baseSpacing);
        case Sqrt2:
            return strategy::sqrt2(ideal, baseSpacing);
        case Base10:
            return strategy::base10(ideal, baseSpacing);
        case OneTwoFive:
            return strategy::oneTwoFive(ideal, baseSpacing);
        case Fixed:
        default:
            return baseSpacing;
        }
    }

    GtGridSpacing scaledGridSpacing(double zoom) const
    {
        GtGridSpacing result{ static_cast<double>(hSpacing), static_cast<double>(vSpacing) };

        if (scalingStrategy == Fixed) return result;

        const double idealH = hSpacing / zoom;
        const double idealV = vSpacing / zoom;

        result.hSpacing = quantizedSpacing(idealH, hSpacing);
        result.vSpacing = quantizedSpacing(idealV, vSpacing);

        return result;
    }

    template <unsigned N>
    static void paintGridLinesImpl(const QRectF& sceneRect,
                            double vLineDistance,
                            double hLineDistance,
                            BufferedLineRender<N>& buffer)
    {
        assert(vLineDistance > 0);
        assert(hLineDistance > 0);

        const double bottom  = sceneRect.bottom();
        const double right = sceneRect.right();
        const double left = sceneRect.left();
        const double top = sceneRect.top();

        const double leftStart = std::floor(left / vLineDistance) * vLineDistance;
        const double topStart = std::floor(top / hLineDistance) * hLineDistance;

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
        // minimum distance between to minor lines in device independent pixels
        constexpr double pixelDensityTooSparseMinor = 80.0;

        const double pixelsPerSceneUnit = std::abs(painter.worldTransform().m11());
        assert(pixelsPerSceneUnit > 0);

        currentSpacing = scaledGridSpacing(pixelsPerSceneUnit);
        assert(currentSpacing.hSpacing > 0);
        assert(currentSpacing.vSpacing > 0);

        BufferedLineRender<500> buffer{painter};

        // TODO
        const double majorLineDistance = currentSpacing.hSpacing * pixelsPerSceneUnit;
        if(majorLineDistance > pixelDensityTooSparseMinor)
        {
            // draw also minor grid lines
            const double tmpHMinorSpacing = currentSpacing.hSpacing / static_cast<double>(hSubdivisons);
            const double tmpVMinorSpacing = currentSpacing.vSpacing / static_cast<double>(vSubdivisions);

            painter.setPen(minorPen);
            paintGridLinesImpl(rect, tmpHMinorSpacing, tmpVMinorSpacing, buffer);
            buffer.flush();
        }

        painter.setPen(majorPen);
        paintGridLinesImpl(rect, currentSpacing.hSpacing, currentSpacing.vSpacing, buffer);
    }

    void paintAxis(QPainter& painter, const QRectF& rect)
    {
        painter.setPen(axisPen);

        for (Qt::Orientation axis : { Qt::Vertical, Qt::Horizontal })
        {
            if (!visibleAxis.testFlag(axis)) continue;

            switch (axis)
            {
            case Qt::Vertical:
                painter.drawLine(0.0, rect.top() - 1.0, 0.0, rect.bottom() + 1.0);
                break;
            case Qt::Horizontal:
                painter.drawLine(rect.left() - 1.0, 0.0, rect.right() + 1.0, 0.0);
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
    pimpl->hSpacing = value;
}

unsigned
GtGrid::hSpacing() const
{
    return pimpl->hSpacing;
}

void
GtGrid::setVSpacing(unsigned value)
{
    pimpl->vSpacing = value;
}

unsigned
GtGrid::vSpacing() const
{
    return pimpl->vSpacing;
}

GtGridSpacing
GtGrid::currentGridSpacing() const
{
    return pimpl->currentSpacing;
}

GtGridSpacing
GtGrid::currentMinorGridSpacing() const
{
    const auto spacing = currentGridSpacing();
    return GtGridSpacing{
        spacing.hSpacing / pimpl->hSubdivisons,
        spacing.vSpacing / pimpl->vSubdivisions
    };
}

GtGridSpacing
GtGrid::scaledGridSpacing(double zoom) const
{
    zoom = std::abs(zoom);
    assert(zoom > 0);

    return pimpl->scaledGridSpacing(zoom);
}

GtGridSpacing
GtGrid::scaledMinorGridSpacing(double zoom) const
{
    const auto spacing = scaledGridSpacing(zoom);
    return GtGridSpacing{
        spacing.hSpacing / pimpl->hSubdivisons,
        spacing.vSpacing / pimpl->vSubdivisions
    };
}

void
GtGrid::setScalingStrategy(ScalingStrategy strategy)
{
    pimpl->scalingStrategy = strategy;
}

GtGrid::ScalingStrategy
GtGrid::scalingStrategy() const
{
    return pimpl->scalingStrategy;
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
GtGrid::setHSubdivions(unsigned count)
{
    pimpl->hSubdivisons = std::max(count, 1u);
}

unsigned
GtGrid::hSubdivions() const
{
    return pimpl->hSubdivisons;
}

void
GtGrid::setVSubdivions(unsigned count)
{
    pimpl->vSubdivisions = std::max(count, 1u);
}

unsigned
GtGrid::vSubdivions() const
{
    return pimpl->vSubdivisions;
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
GtGrid::setMajorPen(QPen pen)
{
    pimpl->majorPen = pen;
}

QPen
GtGrid::majorPen() const
{
    return pimpl->majorPen;
}

void
GtGrid::setLineColor(const QColor& color)
{
    pimpl->majorPen.setColor(color);
}

QColor
GtGrid::lineColor() const
{
    return pimpl->majorPen.color();
}

void
GtGrid::setMinorPen(QPen pen)
{
    pimpl->minorPen = pen;
}

QPen
GtGrid::minorPen() const
{
    return pimpl->minorPen;
}

void
GtGrid::setMinorLineColor(const QColor& color)
{
    pimpl->minorPen.setColor(color);
}

QColor
GtGrid::minorLineColor() const
{
    return pimpl->minorPen.color();
}

void
GtGrid::setAxisPen(QPen pen)
{
    pimpl->axisPen = pen;
}

QPen
GtGrid::axisPen() const
{
    return pimpl->axisPen;
}

void
GtGrid::setAxisColor(const QColor& color)
{
    pimpl->axisPen.setColor(color);
}

QColor
GtGrid::axisColor() const
{
    return pimpl->axisPen.color();
}

void
GtGrid::paint(QPainter& painter, const QRectF& rect, PaintOptions options)
{
    if (!rect.isValid()) return;

    painter.save();
    auto finally = gt::finally([&painter]{ painter.restore(); });
    Q_UNUSED(finally);

    painter.setRenderHint(QPainter::Antialiasing, false);

    if (pimpl->showGrid)
    {
        if (options.testFlag(PaintGrid)) pimpl->paintGridLines(painter, rect);
        if (options.testFlag(PaintAxis)) pimpl->paintAxis(painter, rect);
    }
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    double tmpWidth  = pimpl->currentSpacing.hSpacing;
    double tmpHeight = pimpl->currentSpacing.vSpacing;
    assert(tmpWidth  > 0);
    assert(tmpHeight > 0);

    double xV = floor( p.x() / tmpWidth ) * tmpWidth;
    double yV = floor( p.y() / tmpHeight ) * tmpHeight;

    return {xV, yV};
}

QPointF
GtGrid::computeNearestGridPoint(const QPointF& p)
{
    double tmpWidth  = pimpl->currentSpacing.hSpacing;
    double tmpHeight = pimpl->currentSpacing.vSpacing;
    assert(tmpWidth  > 0);
    assert(tmpHeight > 0);

    double x = std::floor(p.x() / tmpWidth) * tmpWidth;
    double y = std::floor(p.y() / tmpHeight) * tmpHeight;

    if ((p.x() - x) > (tmpWidth * 0.5)) x += tmpWidth;
    if ((p.y() - y) > (tmpHeight * 0.5)) y += tmpHeight;

    return {x, y};
}

bool
GtGrid::isVisible() const
{
    return pimpl->showGrid;
}

void
GtGrid::setVisible(bool visible)
{
    pimpl->showGrid = visible;
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
