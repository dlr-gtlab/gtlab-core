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

#include <QGraphicsView>

struct GtGrid::Impl
{
    Impl(QGraphicsView& v) : view(&v) {}

    /// GtdGraphicsView
    QGraphicsView* view;

    /// Grid horizontal spacing
    size_t hspacing = 100;

    /// Grid vertical spacing
    size_t vspacing = 100;

    /// Grid scale indicator
    bool scaleGrid = true;

    /// Grid scaling factor
    int gridFactor = 0;

    /// Grid points indicator
    bool showGridPoints = false;

    /// Axis indicator
    VisibleAxis visibleAxis{};

    /// Hide grid
    bool isHidden = false;

    /// Horizontal grid line color
    QColor hgColor = gt::gui::color::gridLine();

    /// Vertical grid line color
    QColor vgColor = gt::gui::color::gridLine();

    /// Scene rect
    QRectF rect{};

    /// Horizontal ruler
    GtRuler* hRuler{};

    /// Vertical ruler
    GtRuler* vRuler{};

    double
    getScaledGrid(Qt::Orientation orientation)
    {
        auto pow2 = [](double x){ return x * x; };

        double length = orientation == Qt::Horizontal ? hspacing : vspacing;

        if (scaleGrid)
        {
            if (gridFactor >= 0)
            {
                return (length * pow2(gridFactor + 1));
            }

            return (length * pow2(gridFactor));
        }

        return length;
    }

    template <int NLinesPrealloc>
    void
    paintGridLinesImpl(const QRectF& sceneRect,
                       double vLineDistance,
                       double hLineDistance,
                       const QColor& lineColor,
                       QPainter& painter)
    {
        // draw also minor grid lines
        Q_ASSERT(vLineDistance > 0);
        Q_ASSERT(hLineDistance > 0);

        double firstVLineXPos = int(sceneRect.left()) - (int(sceneRect.left())
                                                         % (static_cast<int>(std::ceil(vLineDistance))));
        double firstHLineYPos = int(sceneRect.top()) - (int(sceneRect.top())
                                                        % (static_cast<int>(std::ceil(hLineDistance))));

        QVarLengthArray<QLineF, NLinesPrealloc> hLines;
        QVarLengthArray<QLineF, NLinesPrealloc> vLines;

        for (double y = firstHLineYPos; y < sceneRect.bottom(); y += hLineDistance)
        {
            hLines.append(QLineF(sceneRect.left(), y, sceneRect.right(), y));
        }

        for (double x = firstVLineXPos; x < sceneRect.right(); x += vLineDistance)
        {
            vLines.append(QLineF(x, sceneRect.top(), x, sceneRect.bottom()));
        }

        if (hLines.size() > NLinesPrealloc)
        {
            gtLogOnceId(Warning, "GtGrid") << "Grid for view" << (void*)view << "exceeded preallocated grid lines (horizontal)";
        }
        if (vLines.size() > NLinesPrealloc)
        {
            gtLogOnceId(Warning, "GtGrid") << "Grid for view" << (void*)view << "exceeded preallocated grid lines (vertical)";
        }

        QPen pen = painter.pen();
        pen.setCosmetic(true);
        pen.setColor(lineColor);

        painter.setPen(pen);

        painter.drawLines(hLines.data(), hLines.size());
        painter.drawLines(vLines.data(), vLines.size());
    }

    void
    paintGridLines(QPainter& painter, const QRectF& rect)
    {
        // minimum distance between to minor linex in device independent pixels
        constexpr const auto minimalMinorLineDistance = 9u;

        // ceil to prevent bad rounding and divison by zero in modulo operation
        const int tmpWidth = ceil(getScaledGrid(Qt::Horizontal));
        const int tmpHeight = ceil(getScaledGrid(Qt::Vertical));


        const double left = int(rect.left()) - (int(rect.left()) % tmpWidth);
        const int nVLines = std::ceil(-(left - rect.right()) / tmpWidth);


        // compute n pixels between two vertical lines
        const auto viewPixelSize = painter.worldTransform().map(
            QPointF(rect.right(), rect.bottom()));

        const auto majorLineDistance = viewPixelSize.x() / nVLines;

        if(majorLineDistance > minimalMinorLineDistance * 10.)
        {
            // draw also minor grid lines
            const double tmpWidthMinor = static_cast<double>(tmpWidth) / 10.;
            const double tmpHeightMinor = static_cast<double>(tmpHeight) / 10.;

            paintGridLinesImpl<1000>(rect, tmpWidthMinor, tmpHeightMinor,
                                     gt::gui::color::gridLineMinor(), painter);
        }

        paintGridLinesImpl<100>(rect, tmpWidth, tmpWidth, hgColor, painter);
    }

    void
    paintAxis(QPainter& painter, const QRectF& rect)
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

    /** Draws rotated text for vertical axis ticks.
        @param painter Current Painter
        @param x Coordinate
        @param y Coordinate
        @param text Tick text */
    void
    drawRotatedText(QPainter& painter, int x, int y, const QString &text)
    {
        painter.save();
        painter.translate(x, y);
        painter.rotate(-90);
        painter.drawText(0, 0, text);
        painter.restore();
    }
};

GtGrid::GtGrid(QGraphicsView& view) :
    QObject(&view),
    pimpl(std::make_unique<Impl>(view))
{
    GT_REMOVAL_GUARD(2, 2, "remove this connection");
    connect(this, &GtGrid::visibilityChanged,
            this, &GtGrid::update);
}

GtGrid::~GtGrid() = default;

void
GtGrid::setGridWidth(int value)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    return setHSpacing(value);
}

void
GtGrid::setHSpacing(size_t value)
{
    // TODO
    if (value < 10)
    {
        gtWarning().verbose() << tr("Grid width < 10!");
        return;
    }

    pimpl->hspacing = value;
}

size_t
GtGrid::hSpacing() const
{
    return pimpl->hspacing;
}

void
GtGrid::setGridHeight(int value)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    return setVSpacing(value);
}

void
GtGrid::setVSpacing(size_t value)
{
    // TODO
    if (value < 10)
    {
        gtWarning().verbose() << tr("Grid height < 10!");
        return;
    }

    pimpl->vspacing = value;
}

size_t
GtGrid::vSpacing() const
{
    return pimpl->vspacing;
}

void
GtGrid::setNumberOfGridPoints(int, int)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
}

void
GtGrid::setScaleGrid(bool val)
{
    pimpl->scaleGrid = val;
}

void
GtGrid::setShowGridPoints(bool)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
}

void
GtGrid::setShowAxis(bool show)
{
    setAxisVisible(show, Qt::Horizontal);
}

void
GtGrid::setAxisVisible(bool show, VisibleAxis axis)
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
GtGrid::setHorizontalGridLineColor(const QColor& color)
{
    setHLineColor(color);
}

void
GtGrid::setHLineColor(const QColor& color)
{
    pimpl->hgColor = color;
}

void
GtGrid::setVerticalGridLineColor(const QColor &color)
{
    setVLineColor(color);
}

void
GtGrid::setVLineColor(const QColor& color)
{
    pimpl->vgColor = color;
}

void
GtGrid::setGridPointColor(const QColor&)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
}

void
GtGrid::paintGrid(QPainter* painter, const QRectF& rect)
{
    if (!painter)
    {
        return;
    }

    return paint(*painter, rect);
}

void
GtGrid::paint(QPainter& painter, const QRectF& rect)
{
    if (!rect.isValid()) return;

    painter.save();
    auto finally = gt::finally([&painter]{ painter.restore(); });
    Q_UNUSED(finally);

    painter.setRenderHint(QPainter::Antialiasing, false);

    if (!pimpl->isHidden)
    {
        pimpl->paintGridLines(painter, rect);
        pimpl->paintAxis(painter, rect);
    }

    if (pimpl->hRuler && pimpl->hRuler->needsRepaint())
    {
        pimpl->rect = rect;
        paintRuler(pimpl->hRuler);
        pimpl->hRuler->setNeedsRepaint(false);
    }

    if (pimpl->vRuler && pimpl->vRuler->needsRepaint())
    {
        pimpl->rect = rect;
        paintRuler(pimpl->vRuler);
        pimpl->vRuler->setNeedsRepaint(false);
    }
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    double tmpWidth  = static_cast<int64_t>(pimpl->getScaledGrid(Qt::Horizontal));
    double tmpHeight = static_cast<int64_t>(pimpl->getScaledGrid(Qt::Vertical));
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
    assert(tmpWidth != 0);
    assert(tmpHeight != 0);

    double x = floor( p.x() / tmpWidth ) * tmpWidth;
    double y = floor( p.y() / tmpHeight ) * tmpHeight;

    if ((p.x() - x) > (tmpWidth * 0.5))
    {
        x += tmpWidth;
    }

    if ((p.y() - y) > (tmpHeight * 0.5))
    {
        y += tmpHeight;
    }

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
GtGrid::paintRuler(GtRuler* ruler)
{
    if (!ruler)
    {
        gtWarning().verbose().nospace() 
                << __FUNCTION__ << ": " << tr("Ruler == NULL");
        return;
    }

    if (ruler->buffer().isNull())
    {
        gtWarning().verbose() << "GtGrid::paintRuler"
                              << tr("WARNING: buffer == NULL");
        return;
    }

    if (!pimpl->rect.isValid())
    {
        gtWarning().verbose() << "GtGrid::paintRuler"
                              << tr("WARNING: pimpl->rect not valid");
        gtWarning().verbose() << " |-> " << pimpl->rect;

        pimpl->rect.setBottomRight(QPointF(0.0, 0.0));
        pimpl->rect.setTopLeft(QPointF(0.0, 0.0));
    }

    QPainter painter(&ruler->buffer());

    painter.fillRect(ruler->buffer().rect(),
                     ruler->palette().color(QPalette::Window));


    Qt::GlobalColor c = Qt::black;

    if (gt::gui::isApplicationDarkTheme())
    {
        c = Qt::white;
    }
    painter.setPen(c);

    if (ruler->orientation() == Qt::Horizontal)
    {
        int tmpWidth = ceil(pimpl->getScaledGrid(Qt::Horizontal));
        assert(tmpWidth > 0);

        double left = int(pimpl->rect.left()) - (int(pimpl->rect.left()) % tmpWidth);

        int h = ruler->buffer().height();

        for (double x = left; x < pimpl->rect.right(); x += tmpWidth)
        {
            QPoint tmp = pimpl->view->mapFromScene(QPointF(x, 0));
            painter.drawLine(tmp.x(), h - 5, tmp.x(), h);

            /* horizontal ticks */
            QByteArray tick;
            tick.setNum(x);
            QString tstr(tick);

            const QSize size = ruler->getFontSizeHint(tstr);
            QRect rect(tmp.x() - size.width() / 2, h - 5 - size.height(),
                       size.width(), size.height());
            painter.setFont(ruler->getFont());

            painter.drawText(rect, Qt::AlignTop | Qt::AlignHCenter, tstr);
        }
    }
    else
    {
        int tmpHeight = ceil(pimpl->getScaledGrid(Qt::Vertical));
        assert(tmpHeight > 0);

        double top = int(pimpl->rect.top()) - (int(pimpl->rect.top()) % tmpHeight);

        int w = ruler->buffer().width();

        for (double y = top; y < pimpl->rect.bottom(); y += tmpHeight)
        {
            QPoint tmp = pimpl->view->mapFromScene(QPointF(0, y));
            painter.drawLine(w - 5, tmp.y(), w, tmp.y());
        }

        /* vertical ticks */
        for (double y = top; y < pimpl->rect.bottom(); y += tmpHeight)
        {
            QPoint tmp = pimpl->view->mapFromScene(QPointF(0, y));
            QByteArray tick;
            tick.setNum(-y);
            QString tstr(tick);
            const QSize size = ruler->getFontSizeHint(tstr);
            QRect rect(w - 5 - size.width(), tmp.y() + size.width() / 2 + 5,
                       size.width(), size.height());
            painter.setFont(ruler->getFont());
            painter.save();
            painter.translate(rect.x(), rect.y());
            painter.rotate(-90);
            painter.drawText(QRect(0, 0, rect.height(), rect.width()),
                             Qt::AlignTop | Qt::AlignHCenter, tstr);
            painter.restore();
        }
    }

    ruler->update();
}

void
GtGrid::setHorizontalRuler(GtRuler* ruler)
{
    pimpl->hRuler = ruler;
}

void
GtGrid::setVerticalRuler(GtRuler* ruler)
{
    pimpl->vRuler = ruler;
}

bool
GtGrid::isVisible() const
{
    return !pimpl->isHidden;
}

void
GtGrid::setVisible(bool visible)
{
    pimpl->isHidden = !visible;
    emit visibilityChanged();
}

void
GtGrid::hideGrid(bool hidden)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    pimpl->isHidden = hidden;
    emit visibilityChanged();
}

void
GtGrid::showGrid(bool visible)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    pimpl->isHidden = !visible;
    emit visibilityChanged();
}
