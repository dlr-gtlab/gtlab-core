/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 30.4.2024
 * Author: Marius Bröcker (AT-TWK)
 * E-Mail: marius.broecker@dlr.de
 */

#ifndef GTDOCKABLEFRAME_H
#define GTDOCKABLEFRAME_H

#include <QFrame>

/**
 * @brief The GtDockableFrame class.
 * Helper class that is used to customize handling of undocked MDI Items.
 */
class GtDockableFrame : public QFrame
{
    Q_OBJECT

public:

    explicit GtDockableFrame(QWidget* parent = nullptr,
                             Qt::WindowFlags f = Qt::WindowFlags());

protected:

    void hideEvent(QHideEvent* event) override;

signals:

    void redockWidget();
};

#endif // GTDOCKABLEFRAME_H
