/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTMATCHDELEGATE_H
#define GTMATCHDELEGATE_H

#include <QStyledItemDelegate>

class QModelIndex;

namespace gt
{

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
    void setMatches(const QList<QModelIndex>& matches);

private:
    QList<QModelIndex> m_matches;
};

} // namespace gt

#endif // GTMATCHDELEGATE_H
