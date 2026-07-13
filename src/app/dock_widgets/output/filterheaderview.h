/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef FILTERHEADERVIEW_H
#define FILTERHEADERVIEW_H

#include <QHeaderView>
#include <QSet>
#include <QMap>
#include <QRect>

class LogFilterProxyModel;
class FilterPopupWidget;

/**
 * @brief The FilterHeaderView class
 * 
 * Custom header view that shows filter buttons in each column header.
 * Clicking a filter button opens a popup menu for multi-select filtering.
 */
namespace gt
{

class FilterHeaderView : public QHeaderView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param orientation Header orientation (horizontal only)
     * @param parent Parent widget
     */
    explicit FilterHeaderView(Qt::Orientation orientation = Qt::Horizontal,
                              QWidget* parent = nullptr);

    /**
     * @brief Set the filter model
     * @param model LogFilterProxyModel to control
     */
    void setFilterModel(LogFilterProxyModel* model);

    /// Get current filter model
    LogFilterProxyModel* filterModel() const { return m_filterModel; }

signals:
    void filterButtonClicked(int column);

private:
    void paintSection(QPainter* painter, const QRect& rect,
                      int logicalIndex) const override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;
    void closePopup();

    QRect filterButtonRect(int logicalIndex) const;
    bool isFilterButtonClicked(const QPoint& pos, int logicalIndex) const;

    LogFilterProxyModel* m_filterModel{nullptr};
    FilterPopupWidget* m_popup{nullptr};
    int m_activeColumn{-1};
    QMap<int, QSet<int>> m_levelFilters;
    QMap<int, QSet<QString>> m_categoryFilters;
    bool m_filtering{false};
};

} // namespace gt

#endif // FILTERHEADERVIEW_H
