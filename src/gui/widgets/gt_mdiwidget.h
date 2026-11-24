/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 30.4.2024
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#ifndef GTMDIWIDGET_H
#define GTMDIWIDGET_H

#include <QFrame>

/**
 * @brief The GtMdiWidget class.
 * Used to customize handling of undocked Gui Items.
 */
class GtMdiWidget : public QFrame
{
    Q_OBJECT

public:
    
    explicit GtMdiWidget(QWidget* parent = nullptr,
                         Qt::WindowFlags f = Qt::WindowFlags());
    ~GtMdiWidget() override;

protected:

    void hideEvent(QHideEvent* event) override;

signals:

    /// emiitted once the widget request to be redocked
    void redockWidget();

    /// emitted once the widget is about to be deleted
    /// (before `QWidget`s destructor)
    void widgetAboutToBeDeleted();
};

#endif // GTMDIWIDGET_H
