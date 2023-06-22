/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelusageviewer.h
 *
 *  Created on: 21.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELUSAGEVIEWER_H
#define GTLABELUSAGEVIEWER_H

#include "gt_mdiitem.h"

class QLabel;
class QListView;
class GtLabelUsageModel;

/**
 * @brief The GtLabelUsageViewer class
 */
class GtLabelUsageViewer : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelUsageViewer
     */
    Q_INVOKABLE GtLabelUsageViewer();

    /**
     * @brief setData
     * @param obj
     */
    void setData(GtObject* obj) override;

private:
    /// Information label.
    QLabel* m_label;

    /// Main usage list view.
    QListView* m_mainView;

    /// Model.
    GtLabelUsageModel* m_model;

};

#endif // GTLABELUSAGEVIEWER_H
