/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTMATCHDELEGATE_H
#define GTMATCHDELEGATE_H

#include "gt_outputdock.h"
#include <QStyledItemDelegate>

class QModelIndex;

/**
 * @brief The GtMatchDelegate class is a style delegate to define how to visualize
 * search matches in the output dock search.
 */
class GtMatchDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:


    explicit GtMatchDelegate(QObject* parent = nullptr);

    /**
     * @brief paint function to define how the found elements are highlighted
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    /**
     * @brief set the found matches for the delegate
     * @param matches
     */
    void setMatches(const QHash<QPersistentModelIndex, GtOutputDock::Matches>& matches);

    void setCurrentMatch(const QPersistentModelIndex& index,
                         int matchNumber);
private:
    //QList<QModelIndex> m_matches;
    QHash<QPersistentModelIndex, GtOutputDock::Matches> m_matches;

    void drawHighlightedText(QPainter* painter,
                             const QRect& rect,
                             const QStyleOptionViewItem& option,
                             const QString& text,
                             const GtOutputDock::Matches& matches) const;

    //QPersistentModelIndex m_currentMatchIndex;
    //int m_currentMatchNumber = -1;
};

#endif // GTMATCHDELEGATE_H
