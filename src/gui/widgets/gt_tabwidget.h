/* GTlab - Gas Turbine laboratory
 * copyright 2009-2024 by DLR
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

protected:

    void mousePressEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

private:

    /// stores the tab that was right clicked
    int m_tabClickedIdx = -1;
};

#endif // GTTABWIDGET_H
