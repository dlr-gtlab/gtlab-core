/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESPERSPECTIVE_H
#define GTPREFERENCESPERSPECTIVE_H

#include "gt_preferencespage.h"

class GtPerspectivesListWidget;
class QPushButton;

/**
 * @brief The GtPreferencesPerspective class
 */
class GtPreferencesPerspective : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesPerspective
     */
    GtPreferencesPerspective();

    /**
     * @brief saveSettings
     */
    void saveSettings(GtSettings&) const override;

    /**
     * @brief loadSettings
     */
    void loadSettings(const GtSettings&) override;

private:
    ///
    GtPerspectivesListWidget* m_list;

    ///
    QPushButton* m_btnNew;

    ///
    QPushButton* m_btnRename;

    ///
    QPushButton* m_btnDuplicate;

    ///
    QPushButton* m_btnDelete;

    ///
    QPushButton* m_btnSwitch;


private slots:
    /**
     * @brief switchPerspective
     */
    void switchPerspective();

    /**
     * @brief renamePerspective
     */
    void renamePerspective();

    /**
     * @brief onItemSelectionChange
     */
    void onItemSelectionChange();

};

#endif // GTPREFERENCESPERSPECTIVE_H
