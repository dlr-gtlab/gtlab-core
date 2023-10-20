/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_cornerwidget.h
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CORNERWIDGET_H
#define GT_CORNERWIDGET_H

#include <QWidget>
#include <QPointer>

class QHBoxLayout;
class QLabel;
class GtMainWin;

/**
 * @brief The GtCornerWidget class
 */
class GtCornerWidget : public QWidget
{
public:
    /**
     * @brief Constructor.
     * @param mainwin Pointer to GTlab main window
     */
    explicit GtCornerWidget(GtMainWin* mainwin);

    /**
     * @brief showUpdateWidget
     * @param val
     */
    void showUpdateWidget(bool val);

private:
    /// Pointer to GTlab main window
    GtMainWin* m_mainwin;

    /// Update label
    QPointer<QLabel> m_updateLabel;

    /// Update animation
    QPointer<QMovie> m_updateAnim;

};

#endif // GT_CORNERWIDGET_H
