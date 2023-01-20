/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTIESDOCK_H
#define GTPROPERTIESDOCK_H

#include <QPointer>

#include "gt_dockwidget.h"

class QLabel;

class GtProcessComponentSettingsButton;
class GtSearchWidget;
class GtPropertyTreeView;
class GtObject;

/**
 * @brief The GtPropertiesDock class
 */
class GtPropertiesDock : public GtDockWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertiesDock
     */
    Q_INVOKABLE GtPropertiesDock();

    /**
     * @brief Returns the recommended dock widget area in main windows.
     * @return specific dock widget area
     */
    Qt::DockWidgetArea getDockWidgetArea() override;

private:
    /// Search widget
    GtSearchWidget* m_search;

    /// Tree view
    GtPropertyTreeView* m_treeView;

    /// Label
    QLabel* m_label;

    /// Button to open calculator settings wizard
    /// (only for custom wizard calculators)
    GtProcessComponentSettingsButton* m_processComponentSettingBtn;

    /// Pointer to current object
    QPointer<GtObject> m_obj;

    /**
     * @brief resizeEvent - override of the event of QWidget:
     * Is used here to resize (truncate) the text displayed in the label
     */
    void resizeEvent(QResizeEvent*) override;

private slots:
    /**
     * @brief objectSelected
     * @param obj
     */
    void objectSelected(GtObject* obj);

    /**
     * @brief refreshTitle
     */
    void refreshTitle();

};

#endif // GTPROPERTIESDOCK_H
