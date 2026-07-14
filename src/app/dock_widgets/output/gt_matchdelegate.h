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

class GtMatchDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit GtMatchDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    void setMatches(const QList<QModelIndex>& matches);

private:
    QList<QModelIndex> m_matches;
};

} // namespace gt

#endif // GTMATCHDELEGATE_H
