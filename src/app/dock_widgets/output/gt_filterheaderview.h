/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef FILTERHEADERVIEW_H
#define FILTERHEADERVIEW_H

#include <QHeaderView>
#include <QSet>
#include <QMap>
#include <QRect>


namespace gt
{

class LogFilterProxyModel;
class FilterPopupWidget;

/**
 * @brief The FilterHeaderView class
 *
 * Custom header view that shows filter buttons in specific column headers.
 * Filter buttons with search icons appear only in Level (column 0) and
 * Id (column 2) columns. Clicking a filter button opens a popup menu
 * for multi-select filtering.
 *
 * The header view maintains minimum column widths (70px) for Level and Id
 * columns to accommodate the filter buttons and ensure good usability.
 *
 * @note Level column uses integer-based filtering (log levels)
 * @note Id column uses string-based filtering (log IDs/categories)
 */
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
    void setFilterModel(gt::LogFilterProxyModel* model);

    /// Get current filter model
    LogFilterProxyModel* filterModel() const { return m_filterModel; }

signals:
    void filterButtonClicked(int column);

private:
    /**
     * @brief paintSection
     * @param painter QPainter for drawing the section
     * @param rect Rectangle of the section
     * @param logicalIndex Logical index of the section
     * 
     * Draws the header section content and overlays a filter button icon
     * for Level (column 0) and Id (column 2) columns. The icon is drawn
     * after the base header content to ensure visibility using painter
     * composition mode management.
     * 
     * For other columns, only the standard header content is drawn.
     */
    void paintSection(QPainter* painter, const QRect& rect,
                      int logicalIndex) const override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;
    void closePopup();

    /**
     * @brief filterButtonRect
     * @param logicalIndex Logical index of the column
     * @return Rectangle position and size of the filter button
     * 
     * Calculates the position of the filter button in the header section.
     * The button is positioned at the right edge of the section with a fixed
     * width of 16 pixels and appropriate margins.
     */
    QRect filterButtonRect(int logicalIndex) const;

    /**
     * @brief isFilterButtonClicked
     * @param pos Mouse position in widget coordinates
     * @param logicalIndex Logical index of the column
     * @return true if the filter button was clicked
     * 
     * Checks if the given mouse position is within the filter button area
     * in the specified column. Returns true only for Level (column 0) and 
     * Id (column 2) columns where filter buttons are displayed.
     */
    bool isFilterButtonClicked(const QPoint& pos, int logicalIndex) const;

    LogFilterProxyModel* m_filterModel{nullptr};
    gt::FilterPopupWidget* m_popup{nullptr};
    int m_activeColumn{-1};
    QMap<int, QSet<int>> m_levelFilters;
    QMap<int, QSet<QString>> m_categoryFilters;
    bool m_filtering{false};
};

} // namespace gt

#endif // FILTERHEADERVIEW_H
