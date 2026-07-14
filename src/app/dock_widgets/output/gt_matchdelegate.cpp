/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_matchdelegate.h"

#include <QPainter>
#include <QApplication>
#include <QStyleOption>

gt::GtMatchDelegate::GtMatchDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void
gt::GtMatchDelegate::paint(QPainter* painter,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();

    if (m_matches.contains(index) && opt.state & QStyle::State_Selected)
    {
        opt.state = opt.state ^ QStyle::State_Selected;
    }

    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    if (m_matches.contains(index))
    {
        const QRect& rect = opt.rect;
        QColor highlightColor = opt.palette.highlight().color();
        highlightColor.setAlpha(100);
        
        painter->save();
        painter->fillRect(rect, highlightColor);
        painter->restore();
    }
}

void
gt::GtMatchDelegate::setMatches(const QList<QModelIndex>& matches)
{
    m_matches = matches;
}

void
gt::GtMatchDelegate::setCurrentMatch(int currentMatch)
{
    m_currentMatch = currentMatch;
}