/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPREFERENCESPAGE_H
#define GTPREFERENCESPAGE_H

#include "gt_gui_exports.h"

#include <QWidget>
#include <QIcon>

class QLabel;
class QVBoxLayout;
class GtSettings;

/**
 * @brief The GtPreferencesPage class
 */
class GT_GUI_EXPORT GtPreferencesPage : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief saveSettings
     */
    virtual void saveSettings(GtSettings&) const = 0;
    /**
     * @brief loadSettings
     */
    virtual void loadSettings(const GtSettings&) = 0;

    QString title() const;
    QString titleShort() const;
    QIcon icon() const;

protected:
    /**
     * @brief GtPreferencesPage
     * @param parent
     */
    explicit GtPreferencesPage(const QString& title, QWidget* parent = nullptr);


    /**
     * @brief setTitle
     * @param id
     */
    void setTitle(const QString& id);
    void setTitleShort(const QString& shortTitle);
    void setIcon(const QIcon& icon);


private:
    /// Label for preferences page title
    QString m_shortTitle, m_title;
    QIcon m_icon;

};

#endif // GTPREFERENCESPAGE_H
