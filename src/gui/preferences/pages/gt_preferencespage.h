/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESPAGE_H
#define GTPREFERENCESPAGE_H

#include <QWidget>

class QLabel;
class QVBoxLayout;

/**
 * @brief The GtPreferencesPage class
 */
class GtPreferencesPage : public QWidget
{
public:
    /**
     * @brief saveSettings
     */
    virtual void saveSettings() = 0;

protected:
    /**
     * @brief GtPreferencesPage
     * @param parent
     */
    GtPreferencesPage(QWidget* parent = NULL);

    /**
     * @brief loadSettings
     */
    virtual void loadSettings() = 0;

    /**
     * @brief setTitle
     * @param id
     */
    void setTitle(const QString& id);

    /**
     * @brief addStretch
     * @param val
     */
    void addStretch(int val = 1);

private:
    /// Label for preferences page title
    QLabel* m_title;

    /// Main layout for preferences page
    QVBoxLayout* m_layout;

};

#endif // GTPREFERENCESPAGE_H
