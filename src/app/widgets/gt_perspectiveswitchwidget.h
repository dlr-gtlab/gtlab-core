/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 30.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPERSPECTIVESWITCHWIDGET_H
#define GTPERSPECTIVESWITCHWIDGET_H

#include "gt_dialog.h"

class QListWidget;

/**
 * @brief The GtPerspectiveSwitchWidget class
 */
class GtPerspectiveSwitchWidget : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtPerspectiveSwitchWidget
     * @param parent
     */
    explicit GtPerspectiveSwitchWidget(QWidget* parent = nullptr);

protected:
    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent * event) override;

    /**
     * @brief keyReleaseEvent
     * @param event
     */
    void keyReleaseEvent(QKeyEvent * event) override;

private:
    /// List of all perspective ids
    QListWidget* m_list;

    /**
     * @brief init
     */
    void init();

    /**
     * @brief nextPerspective
     */
    void nextPerspective();

};

#endif // GTPERSPECTIVESWITCHWIDGET_H
