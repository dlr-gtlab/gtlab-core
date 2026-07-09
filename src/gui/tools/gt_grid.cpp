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

    template <int NLinesPrealloc>
    void paintGridLinesImpl(const QRectF& sceneRect,
                            double vLineDistance,
                            double hLineDistance,
                            const QColor& lineColor,
                            QPainter& painter)
    {
        // draw also minor grid lines
        assert(vLineDistance > 0);
        assert(hLineDistance > 0);

        // TODO
        double firstVLineXPos = int64_t(sceneRect.left()) -
                                (int64_t(sceneRect.left()) %
                                 static_cast<int64_t>(std::ceil(vLineDistance)));
        double firstHLineYPos = int64_t(sceneRect.top()) -
                                (int64_t(sceneRect.top()) %
                                 static_cast<int64_t>(std::ceil(hLineDistance)));

        QVarLengthArray<QLineF, NLinesPrealloc> hLines;
        QVarLengthArray<QLineF, NLinesPrealloc> vLines;

        for (double y = firstHLineYPos; y < sceneRect.bottom(); y += hLineDistance)
        {
            hLines.push_back(QLineF(sceneRect.left(), y, sceneRect.right(), y));
        }

        for (double x = firstVLineXPos; x < sceneRect.right(); x += vLineDistance)
        {
            vLines.push_back(QLineF(x, sceneRect.top(), x, sceneRect.bottom()));
        }

        if (hLines.size() > NLinesPrealloc && gtApp && gtApp->devMode())
        {
            gtLogOnceId(Warning, "GtGrid").verbose(gt::log::Medium)
                << "Grid for view" << view << "exceeded preallocated grid lines (horizontal)";
        }
        if (vLines.size() > NLinesPrealloc && gtApp && gtApp->devMode())
        {
            gtLogOnceId(Warning, "GtGrid").verbose(gt::log::Medium)
                << "Grid for view" << view << "exceeded preallocated grid lines (vertical)";
        }

        QPen pen = painter.pen();
        pen.setCosmetic(true);
        pen.setColor(lineColor);
        painter.setPen(pen);

        painter.drawLines(hLines.data(), hLines.size());
        painter.drawLines(vLines.data(), vLines.size());
    }

    void paintGridLines(QPainter& painter, const QRectF& rect)
    {
        constexpr double stepSize = 80u;


        // ceil to prevent bad rounding and divison by zero in modulo operation
        int64_t scaledHSpacing = ceil(getScaledGrid(Qt::Horizontal));
        int64_t scaledVSpacing = ceil(getScaledGrid(Qt::Vertical));

        // use number of hlines as an indicator to calculate which grid to show
        const double leftMostLine = int64_t(rect.left()) - (int64_t(rect.left()) % scaledHSpacing);
        const unsigned nHLines   = ceil(-(leftMostLine - rect.right()) / scaledHSpacing);
        assert(nHLines > 0);

        // compute n pixels between two vertical lines
        const QPointF viewPixelSize = painter.worldTransform().map(QPointF(rect.right(), rect.bottom()));
        const double  majorLineDistance = viewPixelSize.x() / nHLines;

        gtDebug() << stepSize << majorLineDistance << majorLineDistance / stepSize << stepSize / majorLineDistance << getScaledGrid(Qt::Vertical);

        if (majorLineDistance > 80 && showMinorGrid)
        {
            // draw also minor grid lines
            const double scaledHMinorSpacing = static_cast<double>(scaledHSpacing) / minorHLineCount;
            const double scaledVMinorSpacing = static_cast<double>(scaledVSpacing) / minorVLineCount;

            paintGridLinesImpl<1000>(rect,
                                     scaledHMinorSpacing, scaledVMinorSpacing,
                                     minorLineColor, painter);
        }
        else if (majorLineDistance < 10)
        {
            scaledHSpacing *= 10;
            scaledVSpacing *= 10;

            if (majorLineDistance < 1)
            {
                scaledHSpacing *= 10;
                scaledVSpacing *= 10;

                if (majorLineDistance < 0.2) return;
            }
        }

        paintGridLinesImpl<200>(rect,
                                scaledHSpacing, scaledVSpacing,
                                majorLineColor, painter);
    }

    void paintAxis(QPainter& painter, const QRectF& rect)
    {
        for (Qt::Orientation axis : { Qt::Vertical, Qt::Horizontal })
        {
            if (!visibleAxis.testFlag(axis)) continue;

            QPen pen = painter.pen();
            pen.setColor(gt::gui::color::gridAxis());
            painter.setPen(pen);

            switch (axis)
            {
            case Qt::Vertical:
                painter.drawLine(rect.left(), 0, rect.right(), 0);
                break;
            case Qt::Horizontal:
                painter.drawLine(0, rect.top(), 0, rect.bottom());
                break;
            }
        }
    }

    void paintRuler(GtRuler& ruler)
    {
        if (ruler.buffer().isNull())
        {
            gtWarning().verbose() << "GtGrid::paintRuler"
                                  << tr("WARNING: buffer == NULL");
            return;
        }

        if (!rect.isValid())
        {
            gtWarning().verbose() << "GtGrid::paintRuler"
                                  << tr("WARNING: pimpl->rect not valid");
            gtWarning().verbose() << " |-> " << rect;

            rect.setBottomRight(QPointF(0.0, 0.0));
            rect.setTopLeft(QPointF(0.0, 0.0));
        }

        QPainter painter(&ruler.buffer());

        painter.fillRect(ruler.buffer().rect(),
                         ruler.palette().color(QPalette::Window));

        Qt::GlobalColor c = Qt::black;

        if (gt::gui::isApplicationDarkTheme())
        {
            c = Qt::white;
        }
        painter.setPen(c);

        assert(view);
        if (ruler.orientation() == Qt::Horizontal)
        {
            int tmpWidth = ceil(getScaledGrid(Qt::Horizontal));
            assert(tmpWidth > 0);

            double left = int(rect.left()) - (int(rect.left()) % tmpWidth);

            int h = ruler.buffer().height();

            for (double x = left; x < rect.right(); x += tmpWidth)
            {
                QPoint tmp = view->mapFromScene(QPointF(x, 0));
                painter.drawLine(tmp.x(), h - 5, tmp.x(), h);

                /* horizontal ticks */
                QByteArray tick;
                tick.setNum(x);
                QString tstr(tick);

                const QSize size = ruler.getFontSizeHint(tstr);
                QRect rect(tmp.x() - size.width() / 2, h - 5 - size.height(),
                           size.width(), size.height());
                painter.setFont(ruler.getFont());

                painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, tstr);
            }
        }
        else
        {
            int tmpHeight = ceil(getScaledGrid(Qt::Vertical));
            assert(tmpHeight > 0);

            double top = int(rect.top()) - (int(rect.top()) % tmpHeight);

            int w = ruler.buffer().width();

            for (double y = top; y < rect.bottom(); y += tmpHeight)
            {
                QPoint tmp = view->mapFromScene(QPointF(0, y));
                painter.drawLine(w - 5, tmp.y(), w, tmp.y());
            }

            /* vertical ticks */
            for (double y = top; y < rect.bottom(); y += tmpHeight)
            {
                QPoint tmp = view->mapFromScene(QPointF(0, y));
                QByteArray tick;
                tick.setNum(-y);
                QString tstr(tick);
                const QSize size = ruler.getFontSizeHint(tstr);
                QRect rect(w - 5 - size.width(), tmp.y() + size.width() / 2 + 5,
                           size.width(), size.height());
                painter.setFont(ruler.getFont());
                painter.save();
                painter.translate(rect.x(), rect.y());
                painter.rotate(-90);
                painter.drawText(QRect(0, 0, rect.height(), rect.width()),
                                 Qt::AlignTop | Qt::AlignHCenter, tstr);
                painter.restore();
            }
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
