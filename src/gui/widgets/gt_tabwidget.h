/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 *  Created on: 25.4.2024
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */


#ifndef GTTABWIDGET_H
#define GTTABWIDGET_H

#include <QTabWidget>
#include <gt_gui_exports.h>

/**
 * @brief The GtTabWidget class.
 * Specialized QTabWidget. Will emit tabCloseRequested when tab was middle
 * clicked (only emitted if cursor is on the same tab on release).
 */
class GT_GUI_EXPORT GtTabWidget : public QTabWidget
{
    Q_OBJECT

public:

    GtTabWidget(QWidget* parent = nullptr);

    /**
     * @brief Returns the index of `widget` in the tabbar. Returns -1 if it is
     * not part of the tab widget.
     * @param widget Widget
     * @return Index of widget. Returns -1 if `widget` is not part of the
     * tab widget.
     */
    int tabIndex(QWidget const* widget) const;

    /**
     * @brief Returns whether the tab widget contains `widget`
     * @param widget Widget to check
     * @return Whether tab widget contains `widget`
     */
    bool contains(QWidget const* widget) const;

    /**
     * @brief Returns a list of all widgets in this tab widget in order.
     * @return List of widgets
     */
    QList<QWidget*> widgets();
    QList<QWidget const*> widgets() const;

protected:

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

private:

    /// stores the tab that was right clicked
    int m_tabClickedIdx = -1;
};

#endif // GTTABWIDGET_H
