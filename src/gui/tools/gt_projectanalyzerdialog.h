/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_projectanalyzerdialog.h
 *
 *  Created on: 26.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECTANALYZERDIALOG_H
#define GTPROJECTANALYZERDIALOG_H

#include "gt_dialog.h"
#include "gt_projectanalyzer.h"

class QTabWidget;

/**
 * @brief The GtProjectAnalyzerDialog class visualizes information provided by
 * the GtProjectAnalyzer class.
 */
class GtProjectAnalyzerDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param analyzer Initialization of dialog
     */
    explicit GtProjectAnalyzerDialog(GtProjectAnalyzer* analyzer);

private:
    /// Tab widget pointer
    QTabWidget* m_tabWid;

    /**
     * @brief Returns pointer to tab widget. If no tab widget exists, a new
     * object will be generated.
     * @return Pointer to tab widget.
     */
    QTabWidget* tabWidget();

};

#endif // GTPROJECTANALYZERDIALOG_H
