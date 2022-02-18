/* GTlab - Gas Turbine laboratory
 * Source File: gtd_grid.cpp
 * copyright 2009-2013 by DLR
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
#include <QtMath>
#include <QDebug>

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
    m_hgColor(QColor(200, 200, 255, 125)),
    m_vgColor(QColor(200, 200, 255, 125)),
    m_gpColor(QColor(100, 100, 155)),
    m_hRuler(nullptr),
    m_vRuler(nullptr)
{
    if (gtApp->inDarkMode())
    {
        m_hgColor = QColor(200, 200, 255, 125);
        m_vgColor = QColor(200, 200, 255, 125);
        m_gpColor = QColor(100, 100, 155);
    }

}

void
GtGrid::setGridWidth(int val)
{
    if (val < 10)
    {
        qWarning() << "WARNING: grid width < 10!";
        return;
    }

    m_width = val;
}

void
GtGrid::setGridHeight(int val)
{
    if (val < 10)
    {
        qWarning() << "WARNING: grid height < 10!";
        return;
    }

    m_height = val;
}

void
GtGrid::setNumberOfGridPoints(int horizontal, int vertical)
{
    if (horizontal < 2 || vertical < 2)
    {
        qWarning() << "WARNING: number of points < 2!";
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
    if (painter == nullptr)
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

    if (m_hRuler != nullptr && m_hRuler->needsRepaint())
    {
        m_rect = rect;
        paintRuler(m_hRuler);
        m_hRuler->setNeedsRepaint(false);
    }

    if (m_vRuler != nullptr && m_vRuler->needsRepaint())
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

    if (m_hRuler != nullptr)
    {
        m_hRuler->setNeedsRepaint(true);
    }

    if ( m_vRuler != nullptr)
    {
        m_vRuler->setNeedsRepaint(true);
    }

}

void
GtGrid::paintRuler(GtRuler* ruler)
{
    if (ruler == nullptr)
    {
        qWarning() << "WARNING: ruler == NULL";
//        QLOG_WARN() << "WARNING: ruler == NULL";
        return;
    }

    if (ruler->buffer().isNull())
    {
        qWarning() << "WARNING: buffer == NULL";
//        QLOG_WARN() << "WARNING: buffer == NULL";
        return;
    }

    if (!m_rect.isValid())
    {
        qWarning() << "WARNING: m_rect not valid";
        qWarning() << " |-> " << m_rect;

        m_rect.setBottomRight(QPointF(0.0, 0.0));
        m_rect.setTopLeft(QPointF(0.0, 0.0));

//        QLOG_WARN() << "WARNING: m_rect not valid";
//        QLOG_WARN() << " |-> " << m_rect;
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

void
GtGrid::paintGridLines(QPainter* painter, const QRectF& rect)
{
    if (painter == nullptr)
    {
        return;
    }

    QVarLengthArray<QLineF, 100> hLines;
    QVarLengthArray<QLineF, 100> vLines;

    // qCeil to prevent bad rounding and divison by zero in modulo operation
    int tmpWidth = qCeil(getScaledGrid(Qt::Horizontal));
    int tmpHeight = qCeil(getScaledGrid(Qt::Vertical));

    Q_ASSERT(tmpWidth > 0);
    Q_ASSERT(tmpHeight > 0);

    qreal left = int(rect.left()) - (int(rect.left()) % tmpWidth);
    qreal top = int(rect.top()) - (int(rect.top()) % tmpHeight);

    for (qreal y = top; y < rect.bottom(); y += tmpHeight)
    {
        hLines.append(QLineF(rect.left(), y, rect.right(), y));
    }

    for (qreal x = left; x < rect.right(); x += tmpWidth)
    {
        vLines.append(QLineF(x, rect.top(), x, rect.bottom()));
    }

    QPen pen = painter->pen();
    pen.setCosmetic(true);
    pen.setColor(m_hgColor);
    painter->setPen(pen);
    painter->drawLines(hLines.data(), hLines.size());
    pen.setColor(m_vgColor);
    painter->setPen(pen);
    painter->drawLines(vLines.data(), vLines.size());

}

void
GtGrid::paintAxis(QPainter* painter, const QRectF& rect)
{
    if (!m_showAxis)
    {
        return;
    }

    QPen pen = painter->pen();
    pen.setColor(QColor(150, 150, 150));
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

void GtGrid::drawRotatedText(QPainter* painter, int x, int y,
                              const QString &text)
{
    if (painter != nullptr)
    {
        painter->save();
        painter->translate(x, y);
        painter->rotate(-90);
        painter->drawText(0, 0, text);
        painter->restore();
    }
}
