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

    /// Number of horizontal grid points
    size_t nohgp = 5;

    /// Number of vertical grid points
    size_t novgp = 5;

    /// Grid scale indicator
    bool scaleGrid = true;

    /// Grid scaling factor
    int gridFactor = 0;

    /// Grid points indicator
    bool showGridPoints = false;

    /// Axis indicator
    bool showAxis = false;

    /// Hide grid
    bool isHidden = false;

    /// Horizontal grid line color
    QColor hgColor = gt::gui::color::gridLine();

    /// Vertical grid line color
    QColor vgColor = gt::gui::color::gridLine();

    /// Grid point color
    QColor gpColor = gt::gui::color::gridPoint();

    /// Scene rect
    QRectF rect{};

    /// Horizontal ruler
    GtRuler* hRuler{};

    /// Vertical ruler
    GtRuler* vRuler{};
};

GtGrid::GtGrid(QGraphicsView& view) :
    QObject(&view),
    pimpl(std::make_unique<Impl>(view))
{

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
GtGrid::setNumberOfGridPoints(int horizontal, int vertical)
{
    if (horizontal < 2 || vertical < 2)
    {
        gtWarning().verbose() << tr("Number of points < 2!");
        return;
    }

    pimpl->nohgp = horizontal;
    pimpl->novgp = vertical;
}

void
GtGrid::setScaleGrid(bool val)
{
    pimpl->scaleGrid = val;
}

void
GtGrid::setShowGridPoints(bool val)
{
    pimpl->showGridPoints = val;
}

void
GtGrid::setShowAxis(bool val)
{
    pimpl->showAxis = val;
}

void
GtGrid::setHorizontalGridLineColor(const QColor &color)
{
    pimpl->hgColor = color;
}

void
GtGrid::setVerticalGridLineColor(const QColor &color)
{
    pimpl->vgColor = color;
}

void
GtGrid::setGridPointColor(const QColor &color)
{
    pimpl->gpColor = color;
}

void
GtGrid::paintGrid(QPainter* painter, const QRectF &rect)
{
    if (!painter)
    {
        return;
    }

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing, false);

    if (!pimpl->isHidden)
    {
        paintGridLines(painter, rect);

        paintAxis(painter, rect);
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

    painter->restore();
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    int tmpWidth = int(getScaledGrid(Qt::Horizontal));
    int tmpHeight = int(getScaledGrid(Qt::Vertical));

    double xV = floor( p.x() / tmpWidth ) * tmpWidth;
    double yV = floor( p.y() / tmpHeight ) * tmpHeight;

    return {xV, yV};
}

QPointF
GtGrid::computeNearestGridPoint(const QPointF& p)
{
    int tmpWidth = int(getScaledGrid(Qt::Horizontal));
    int tmpHeight = int(getScaledGrid(Qt::Vertical));

    double x = floor( p.x() / tmpWidth ) * tmpWidth;
    double y = floor( p.y() / tmpHeight ) * tmpHeight;

    if ((p.x() - x) > (tmpWidth / 2))
    {
        x += tmpWidth;
    }

    if ((p.y() - y) > (tmpHeight / 2))
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
        int tmpWidth = ceil(getScaledGrid(Qt::Horizontal));
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
        int tmpHeight = ceil(getScaledGrid(Qt::Vertical));
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
    emit update();
}

void
GtGrid::hideGrid(bool hidden)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    pimpl->isHidden = hidden;
    emit update();
}

void
GtGrid::showGrid(bool visible)
{
    GT_REMOVAL_GUARD(2, 2, "remove this function")
    pimpl->isHidden = !visible;
    emit update();
}


template <int NLinesPrealloc>
void
paintGridLinesImpl(const QRectF& sceneRect,
                   double vLineDistance,
                   double hLineDistance,
                   const QColor& lineColor,
                   QPainter* painter)
{
    // draw also minor grid lines
    Q_ASSERT(vLineDistance > 0);
    Q_ASSERT(hLineDistance > 0);

    double firstVLineXPos = int(sceneRect.left()) - (int(sceneRect.left())
                         % (static_cast<int>(std::ceil(vLineDistance))));
    double firstHLineYPos = int(sceneRect.top()) - (int(sceneRect.top())
                         % (static_cast<int>(std::ceil(hLineDistance))));

    QVarLengthArray<QLineF, NLinesPrealloc> hLinesMinor;
    QVarLengthArray<QLineF, NLinesPrealloc> vLinesMinor;

    for (double y = firstHLineYPos; y < sceneRect.bottom(); y += hLineDistance)
    {
        hLinesMinor.append(QLineF(sceneRect.left(), y, sceneRect.right(), y));
    }

    for (double x = firstVLineXPos; x < sceneRect.right(); x += vLineDistance)
    {
        vLinesMinor.append(QLineF(x, sceneRect.top(), x, sceneRect.bottom()));
    }

    QPen pen = painter->pen();
    pen.setCosmetic(true);
    pen.setColor(lineColor);

    painter->setPen(pen);

    painter->drawLines(hLinesMinor.data(), hLinesMinor.size());
    painter->drawLines(vLinesMinor.data(), vLinesMinor.size());
}

void
GtGrid::paintGridLines(QPainter* painter, const QRectF& rect)
{
    if (!painter)
    {
        return;
    }

    // minimum distance between to minor linex in device independent pixels
    constexpr const auto minimalMinorLineDistance = 9u;

    // ceil to prevent bad rounding and divison by zero in modulo operation
    const int tmpWidth = ceil(getScaledGrid(Qt::Horizontal));
    const int tmpHeight = ceil(getScaledGrid(Qt::Vertical));


    const double left = int(rect.left()) - (int(rect.left()) % tmpWidth);
    const int nVLines = std::ceil(-(left - rect.right()) / tmpWidth);


    // compute n pixels between two vertical lines
    const auto viewPixelSize = painter->worldTransform().map(
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

    paintGridLinesImpl<100>(rect, tmpWidth, tmpWidth, pimpl->hgColor, painter);
}

void
GtGrid::paintAxis(QPainter* painter, const QRectF& rect)
{
    if (!pimpl->showAxis)
    {
        return;
    }

    QPen pen = painter->pen();
    pen.setColor(gt::gui::color::gridAxis());
    painter->setPen(pen);

    painter->drawLine(int(rect.left()), 0, int(rect.right()), 0);
}

double
GtGrid::getScaledGridWidth()
{
    return getScaledGrid(Qt::Horizontal);
}

double
GtGrid::getScaledGridHeight()
{
    return getScaledGrid(Qt::Vertical);
}

double
GtGrid::getScaledGrid(Qt::Orientation orientation)
{
    auto pow2 = [](double x){ return x * x; };

    double length = pimpl->hspacing ;

    if (orientation == Qt::Horizontal)
    {
        length = pimpl->hspacing;
    }
    else
    {
        length = pimpl->vspacing;
    }

    if (pimpl->scaleGrid)
    {
        if (pimpl->gridFactor >= 0)
        {
            return (length * pow2(pimpl->gridFactor + 1));
        }

        return (length * pow2(pimpl->gridFactor));
    }

    return length;
}

void
GtGrid::drawRotatedText(QPainter* painter, int x, int y,
                              const QString &text)
{
    if (painter)
    {
        painter->save();
        painter->translate(x, y);
        painter->rotate(-90);
        painter->drawText(0, 0, text);
        painter->restore();
    }
}
