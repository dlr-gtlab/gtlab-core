/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    void saveSettings() override;

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

    /**
     * @brief loadSettings
     */
    void loadSettings() override;

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
