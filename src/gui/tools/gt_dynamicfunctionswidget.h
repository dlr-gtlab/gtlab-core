/* GTlab - Gas Turbine laboratory
 * Source File: gt_dynamicfunctionswidget.h
 * copyright 2009-2022 by DLR
 *
 *  Created on: 25.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTDYNAMICFUNCTIONSWIDGET_H
#define GTDYNAMICFUNCTIONSWIDGET_H

#include <QWidget>
#include "gt_gui_exports.h"

class QTreeWidget;
class GtSearchWidget;

/**
 * @brief The GtDynamicFunctionsWidget class
 * Widget for the overview of dynamic functions
 * from the dynamic function interface
 *
 * The functions are displayed as a treeview sorted by the modules they
 * are coming from.
 *
 * Additionally a search widget is added to serach functions by thier name.
 */
class GT_GUI_EXPORT GtDynamicFunctionsWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtDynamicFunctionsWidget
     * Constructs the widget and fills the treeview with the dynamic functions
     * from the interface
     * @param parent widget
     */
    Q_INVOKABLE explicit GtDynamicFunctionsWidget(QWidget* parent = nullptr);

private:
    /// Search widget
    GtSearchWidget* m_searchWidget;

    /// tree widget for the main content
    QTreeWidget* m_content;

    /// Expand states
    QStringList m_expandStates;
    /**
     * @brief divideID
     * The functionID from the InterfaceHandler is a combined name
     * based on the module id and the function id itself.
     * This function can be used to seperate the string in the two parts
     *
     * @param functionId from the getRegisteredFunctionIDs function
     * @return a tuple of module ID and functionId
     */
    static std::pair<QString, QString> divideKey(const QString &functionId);
private slots:
    /**
     * @brief filterData
     * @param val - String from the search widget to filter the treeview
     */
    void filterData(const QString& val);
};

#endif // GTDYNAMICFUNCTIONSWIDGET_H
