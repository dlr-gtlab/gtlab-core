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
class GtSettings;

/**
 * @brief The GtPreferencesPage class
 */
class GtPreferencesPage : public QWidget
{
public:
    /**
     * @brief saveSettings
     */
    virtual void saveSettings(GtSettings&) const = 0;
    /**
     * @brief loadSettings
     */
    virtual void loadSettings(const GtSettings&) = 0;

protected:
    /**
     * @brief GtPreferencesPage
     * @param parent
     */
    explicit GtPreferencesPage(QWidget* parent = nullptr);


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
