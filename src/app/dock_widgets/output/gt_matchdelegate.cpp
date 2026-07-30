/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_matchdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QStyleOption>
#include <QTextFormat>
#include <QTextLayout>

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

    // eigene Selection-Logik
    if (matchIt != m_matches.constEnd() && (opt.state & QStyle::State_Selected))
    {
        opt.state &= ~QStyle::State_Selected;
    }

    // alles außer Text zeichnen
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    // komplette Zeile markieren
    if (matchIt != m_matches.constEnd())
    {
        QColor color = opt.palette.highlight().color();
        color.setAlpha(100);

        painter->fillRect(opt.rect, color);
    }

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt,
                                           opt.widget);

    drawHighlightedText(painter, textRect, option, text,
        matchIt != m_matches.constEnd()
            ? matchIt.value()
            : GtOutputDock::Matches{});
}

void
GtMatchDelegate::setMatches(const QHash<QPersistentModelIndex,
                                        GtOutputDock::Matches>& matches)
{
    m_matches = matches;
}

void
GtMatchDelegate::drawHighlightedText(QPainter* painter,
                                    const QRect& rect,
                                    const QStyleOptionViewItem& option,
                                    const QString& text,
                                    const GtOutputDock::Matches& matches) const
{
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

    QTextLine line = layout.createLine();
    line.setLineWidth(rect.width());

    layout.endLayout();

    QPointF pos = rect.topLeft();
    pos.setY(pos.y() + (rect.height() - line.height()) / 2.0);

    layout.draw(painter, pos);
}
