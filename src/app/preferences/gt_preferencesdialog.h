/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESDIALOG_H
#define GTPREFERENCESDIALOG_H

#include <QDialog>

class QListWidgetItem;
class QListWidget;
class QStackedWidget;

/**
 * @brief The GtPreferencesDialog class
 */
class GtPreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesDialog
     */
    GtPreferencesDialog(int initItem = 0);

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
    ///
    QListWidget* m_contentsWidget;

    ///
    QStackedWidget* m_pagesWidget;

    /**
     * @brief createIcons
     */
    void createIcons();

};

#endif // GTPREFERENCESDIALOG_H
