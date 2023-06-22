/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_preferencesaccess.h
 *
 *  Created on: 18.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTPREFERENCESACCESS_H
#define GTPREFERENCESACCESS_H

#include "gt_preferencespage.h"

class QTabWidget;
class GtAccessGroupModel;

/**
 * @brief The GtPreferencesAccess class
 */
class GtPreferencesAccess : public GtPreferencesPage
{
    Q_OBJECT

public:
    /**
     * @brief GtPreferencesAccess
     */
    GtPreferencesAccess();

    /**
     * @brief saveSettings
     */
    void saveSettings(GtSettings&) const override;

    /**
     * @brief loadSettings
     */
    void loadSettings(const GtSettings&) override;


private:
    /// list of access group models.
    QList<GtAccessGroupModel*> m_models;

    /// Tab widget.
    QTabWidget* m_tabWidget;

private slots:
    /**
     * @brief addAccessData
     */
    void addAccessData();

    /**
     * @brief onDoubleClicked
     * @param index
     */
    void onDoubleClicked(const QModelIndex& index);

    /**
     * @brief onClicked
     * @param index
     */
    void onClicked(const QModelIndex& index);

};

#endif // GTPREFERENCESACCESS_H
