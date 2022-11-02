/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESSESSION_H
#define GTPREFERENCESSESSION_H

#include "gt_preferencespage.h"

class GtSessionListWidget;
class QPushButton;
class QCheckBox;

/**
 * @brief The GtPreferencesSession class
 */
class GtPreferencesSession : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesSession
     */
    GtPreferencesSession();

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
    GtSessionListWidget* m_list;

    ///
    QCheckBox* m_openLastCheck;

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
    //    void selectDirectory();

    /**
     * @brief newSession
     */
    void newSession();

    /**
     * @brief deleteSession
     */
    void deleteSession();

    /**
     * @brief switchSession
     */
    void switchSession();

    /**
     * @brief renameSession
     */
    void renameSession();

    /**
     * @brief onItemSelectionChange
     */
    void onItemSelectionChange();

    /**
     * @brief onItemTextChange
     * @param val
     */
    void onItemTextChange(const QString& val);

    /**
     * @brief onLstItemsCommitData
     * @param pLineEdit
     */
    void onLstItemsCommitData(QWidget* pLineEdit);

};

#endif // GTPREFERENCESSESSION_H
