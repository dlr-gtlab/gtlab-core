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
#include "gt_graphicsview.h"
#include "gt_ruler.h"
#include "gt_application.h"
#include "gt_colors.h"
#include "gt_logging.h"
#include <QtMath>

GtGrid::GtGrid(GtGraphicsView& view) :
    m_view(view),
    m_width(100),
    m_height(100),
    m_nohgp(5),
    m_novgp(5),
    m_scaleGrid(true),
    m_gridFactor(0),
    m_showGridPoints(false),
    m_showAxis(false),
    m_hideGrid(false),
    m_hgColor(gt::gui::color::gridLine()),
    m_vgColor(gt::gui::color::gridLine()),
    m_gpColor(gt::gui::color::gridPoint()),
    m_hRuler(nullptr),
    m_vRuler(nullptr)
{

}

void
GtGrid::setGridWidth(int val)
{
    if (val < 10)
    {
        gtWarning().verbose() << tr("Grid width < 10!");
        return;
    }

    m_width = val;
}

void
GtGrid::setGridHeight(int val)
{
    if (val < 10)
    {
        gtWarning().verbose() << tr("Grid height < 10!");
        return;
    }

    m_height = val;
}

void
GtGrid::setNumberOfGridPoints(int horizontal, int vertical)
{
    if (horizontal < 2 || vertical < 2)
    {
        gtWarning().verbose() << tr("Number of points < 2!");
        return;
    }

    m_nohgp = horizontal;
    m_novgp = vertical;
}

void
GtGrid::setScaleGrid(bool val)
{
    m_scaleGrid = val;
}

void
GtGrid::setShowGridPoints(bool val)
{
    m_showGridPoints = val;
}

void
GtGrid::setShowAxis(bool val)
{
    m_showAxis = val;
}

void
GtGrid::setHorizontalGridLineColor(const QColor &color)
{
    m_hgColor = color;
}

void
GtGrid::setVerticalGridLineColor(const QColor &color)
{
    m_vgColor = color;
}

void
GtGrid::setGridPointColor(const QColor &color)
{
    m_gpColor = color;
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

    if (!m_hideGrid)
    {
        paintGridLines(painter, rect);

        paintAxis(painter, rect);
    }

    if (m_hRuler && m_hRuler->needsRepaint())
    {
        m_rect = rect;
        paintRuler(m_hRuler);
        m_hRuler->setNeedsRepaint(false);
    }

    if (m_vRuler && m_vRuler->needsRepaint())
    {
        m_rect = rect;
        paintRuler(m_vRuler);
        m_vRuler->setNeedsRepaint(false);
    }

    painter->restore();
}

QPointF
GtGrid::computeTopLeftGridPoint(const QPointF& p)
{
    int tmpWidth = int(getScaledGrid(Qt::Horizontal));
    int tmpHeight = int(getScaledGrid(Qt::Vertical));

    qreal xV = floor( p.x() / tmpWidth ) * tmpWidth;
    qreal yV = floor( p.y() / tmpHeight ) * tmpHeight;

    return {xV, yV};
}

QPointF
GtGrid::computeNearestGridPoint(const QPointF& p)
{
    int tmpWidth = int(getScaledGrid(Qt::Horizontal));
    int tmpHeight = int(getScaledGrid(Qt::Vertical));

    qreal x = floor( p.x() / tmpWidth ) * tmpWidth;
    qreal y = floor( p.y() / tmpHeight ) * tmpHeight;

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
    m_gridFactor = val;

    if (m_hRuler)
    {
        m_hRuler->setNeedsRepaint(true);
    }

    if ( m_vRuler)
    {
        m_vRuler->setNeedsRepaint(true);
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

    if (!m_rect.isValid())
    {
        gtWarning().verbose() << "GtGrid::paintRuler"
                              << tr("WARNING: m_rect not valid");
        gtWarning().verbose() << " |-> " << m_rect;

        m_rect.setBottomRight(QPointF(0.0, 0.0));
        m_rect.setTopLeft(QPointF(0.0, 0.0));

//        return;
    }

    QPainter painter(&ruler->buffer());

    painter.fillRect(ruler->buffer().rect(),
                     ruler->palette().color(QPalette::Window));


    Qt::GlobalColor c = Qt::black;

    if (gtApp->inDarkMode())
    {
        c = Qt::white;
    }
    painter.setPen(c);

    if (ruler->orientation() == Qt::Horizontal)
    {
        int tmpWidth = qCeil(getScaledGrid(Qt::Horizontal));

        Q_ASSERT(tmpWidth > 0);

        qreal left = int(m_rect.left()) - (int(m_rect.left()) % tmpWidth);

        int h = ruler->buffer().height();

        for (qreal x = left; x < m_rect.right(); x += tmpWidth)
        {
            QPoint tmp = m_view.mapFromScene(QPointF(x, 0));
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
        int tmpHeight = qCeil(getScaledGrid(Qt::Vertical));

        Q_ASSERT(tmpHeight > 0);

        qreal top = int(m_rect.top()) - (int(m_rect.top()) % tmpHeight);

        int w = ruler->buffer().width();

        for (qreal y = top; y < m_rect.bottom(); y += tmpHeight)
        {
            QPoint tmp = m_view.mapFromScene(QPointF(0, y));
            painter.drawLine(w - 5, tmp.y(), w, tmp.y());

        }

        /* vertical ticks */
        for (qreal y = top; y < m_rect.bottom(); y += tmpHeight)
        {
            QPoint tmp = m_view.mapFromScene(QPointF(0, y));
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
    m_hRuler = ruler;
}

void
GtGrid::setVerticalRuler(GtRuler* ruler)
{
    m_vRuler = ruler;
}

void
GtGrid::hideGrid(bool val)
{
    m_hideGrid = val;
    emit update();
}

void
GtGrid::showGrid(bool val)
{
    m_hideGrid = !val;
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

    qreal firstVLineXPos = int(sceneRect.left()) - (int(sceneRect.left())
                         % (static_cast<int>(std::ceil(vLineDistance))));
    qreal firstHLineYPos = int(sceneRect.top()) - (int(sceneRect.top())
                         % (static_cast<int>(std::ceil(hLineDistance))));

    QVarLengthArray<QLineF, NLinesPrealloc> hLinesMinor;
    QVarLengthArray<QLineF, NLinesPrealloc> vLinesMinor;

    for (qreal y = firstHLineYPos; y < sceneRect.bottom(); y += hLineDistance)
    {
        hLinesMinor.append(QLineF(sceneRect.left(), y, sceneRect.right(), y));
    }

    for (qreal x = firstVLineXPos; x < sceneRect.right(); x += vLineDistance)
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

    // qCeil to prevent bad rounding and divison by zero in modulo operation
    const int tmpWidth = qCeil(getScaledGrid(Qt::Horizontal));
    const int tmpHeight = qCeil(getScaledGrid(Qt::Vertical));


    const qreal left = int(rect.left()) - (int(rect.left()) % tmpWidth);
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

    paintGridLinesImpl<100>(rect, tmpWidth, tmpWidth, m_hgColor, painter);
}

void
GtGrid::paintAxis(QPainter* painter, const QRectF& rect)
{
    if (!m_showAxis)
    {
        return;
    }

    QPen pen = painter->pen();
    pen.setColor(gt::gui::color::gridAxis());
    painter->setPen(pen);

    painter->drawLine(int(rect.left()), 0, int(rect.right()), 0);
}

qreal
GtGrid::getScaledGridWidth()
{
    return getScaledGrid(Qt::Horizontal);
}

qreal
GtGrid::getScaledGridHeight()
{
    return getScaledGrid(Qt::Vertical);
}

qreal
GtGrid::getScaledGrid(Qt::Orientation val)
{
    int length;

    if (val == Qt::Horizontal)
    {
        length = m_width;
    }
    else
    {
        length = m_height;
    }

    if (m_scaleGrid)
    {
        if (m_gridFactor >= 0)
        {
            return (length * qPow(2, m_gridFactor + 1));
        }

        return (length * qPow(2, m_gridFactor));

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
