/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_startuppage.h
 *
 *  Created on: 19.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_STARTUPPAGE_H
#define GT_STARTUPPAGE_H

#include "gt_mdiitem.h"

class QFrame;
class QToolButton;
class QLabel;

/**
 * @brief The GtStartupPage class
 */
class GtStartupPage : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtStartupPage
     */
    Q_INVOKABLE GtStartupPage();

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    QIcon icon() const override;

    /**
     * @brief allowsMultipleInstances
     * @return true because the memento viewer can be opened multiple times
     */
    bool allowsMultipleInstances() const override;

public slots:
    /**
     * @brief onThemeChange - handle the change between the GUI theme options
     */
    void onThemeChanged() override;

private slots:
    /**
     * @brief showIndicatorToggled
     * @param val
     */
    void showIndicatorToggled(bool val);


private:
    /// main frame
    QFrame* m_frame;

    /// Placeholder for gtlab logo
    QLabel* m_logoLabel;

    /**
     * @brief initialize widget content
     */
    void initializeTheme();

};

#endif // GT_STARTUPPAGE_H
