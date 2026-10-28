/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_matchdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QFontMetrics>
#include <QStyleOption>
#include <QTableView>
#include <QTextFormat>
#include <QTextLayout>
#include <QVector>

GtMatchDelegate::GtMatchDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void
GtMatchDelegate::paint(QPainter* painter,
                       const QStyleOptionViewItem& option,
                       const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    /// clear initial
    QString text = opt.text;
    opt.text.clear();

    const QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

    const auto matchIt = m_matches.constFind(index);
    const bool selected = opt.state & QStyle::State_Selected;

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    if (matchIt != m_matches.constEnd() && !selected)
    {
        QColor color = opt.palette.highlight().color();
        color.setAlpha(100);
        painter->fillRect(opt.rect, color);
    }

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt,
                                           opt.widget);

    QColor textColor;

    if (option.state & QStyle::State_Selected)
    {
        textColor = option.palette.color(QPalette::HighlightedText);
    }
    else
    {
        textColor = index.data(Qt::ForegroundRole).value<QColor>();

        if (!textColor.isValid())
        {
            textColor = option.palette.color(QPalette::Text);
        }
    }

    painter->save();
    painter->setClipRect(textRect);

    drawHighlightedText(painter, textRect, opt, text, textColor,
                        matchIt != m_matches.constEnd()
                            ? matchIt.value()
                            : GtOutputDock::Matches{});

    painter->restore();
}

void
GtMatchDelegate::setMatches(const QHash<QPersistentModelIndex,
                                        GtOutputDock::Matches>& matches)
{
    m_matches = matches;
}

void
GtMatchDelegate::drawHighlightedText(QPainter* painter, const QRect& rect,
                                     const QStyleOptionViewItem& option,
                                     const QString& text,
                                     const QColor& textColor,
                                     const GtOutputDock::Matches& matches) const
{
    if (rect.isEmpty() || text.isEmpty())
    {
        return;
    }

    QTextLayout layout(text, option.font);

    QVector<QTextLayout::FormatRange> ranges;
    ranges.reserve(matches.size());

    for (const auto& match : matches)
    {
        QTextLayout::FormatRange range;
        range.start = match.start;
        range.length = match.length;

        QTextCharFormat format;
        QColor highlightColor(128, 128, 128);
        highlightColor.setAlpha(120);
        format.setBackground(highlightColor);

        range.format = format;
        ranges.push_back(range);
    }

    layout.setFormats(ranges);

    layout.beginLayout();

    qreal totalHeight = 0.0;
    bool layoutComplete = false;

    while (totalHeight < rect.height())
    {
        QTextLine line = layout.createLine();

        if (!line.isValid())
        {
            layoutComplete = true;
            break;
        }

        line.setLineWidth(rect.width());
        line.setPosition(QPointF(0.0, totalHeight));

        totalHeight += line.height();
    }

    layout.endLayout();

    qreal verticalOffset = 0.0;

    if (layoutComplete && totalHeight < rect.height())
    {
        if (option.displayAlignment & Qt::AlignVCenter)
        {
            verticalOffset = (rect.height() - totalHeight) / 2.0;
        }
        else if (option.displayAlignment & Qt::AlignBottom)
        {
            verticalOffset = rect.height() - totalHeight;
        }
    }

    painter->save();
    painter->setClipRect(rect);
    painter->setPen(textColor);

    layout.draw(painter, QPointF(rect.left(), rect.top() + verticalOffset));

    painter->restore();
}
