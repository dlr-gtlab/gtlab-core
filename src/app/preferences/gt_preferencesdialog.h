/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESDIALOG_H
#define GTPREFERENCESDIALOG_H

#include "gt_dialog.h"

class QListWidgetItem;
class QListWidget;
class QStackedWidget;
class GtPreferencesPage;

/**
 * @brief The GtPreferencesDialog class
 */
class GtPreferencesDialog : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesDialog
     */
    explicit GtPreferencesDialog(int initItem = 0, QWidget* parent = nullptr);

    /**
     * @brief Allows to specify the page that is displayed first when opening
     * the preferences dialog.
     * @param title Title of the page to be displayed first.
     */
    void setStartingPage(const QString& title);

    /**
     * @brief Returns the title of the current page
     * @return
     */
    QString currentPageTitle() const;

public slots:
    /**
     * @brief changePage
     * @param current
     * @param previous
     */
    void changePage(QListWidgetItem* current, QListWidgetItem* previous);

    /**
     * @brief saveChanges
     */
    void saveChanges();

private:
    void addPage(GtPreferencesPage* page);

    /**
     * @brief Returns the index of the page with the given title.
     * @param title Title of the page whose number is searched for.
     * @return The index of the page with the given title. If no page was found
     * it returns -1.
     */
    int pageIndex(const QString& title);

    ///
    QListWidget* m_contentsWidget;

    ///
    QStackedWidget* m_pagesWidget;

    /// non owning vector of page references
    QVector<GtPreferencesPage*> m_pages;

    /**
     * @brief Loads the internally stored settings into
     * the widgets of each preference page
     */
    void loadSettings();
};

#endif // GTPREFERENCESDIALOG_H
