/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.03.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTSHORTCUTSETTINGSDATA_H
#define GTSHORTCUTSETTINGSDATA_H

#include <QVariant>
#include "gt_core_exports.h"

/**
 * @brief The shortCutSettingsData struct
 * Struct to handle the information needed for the short cut system in
 * the settings and GTlab at all
 */
struct GT_CORE_EXPORT GtShortCutSettingsData{

    /**
     * @brief shortCutSettingsData - default constructor
     */

    GtShortCutSettingsData();

    /**
     * @brief shortCutSettingsData - constructor
     * @param identifier - identification string
     * @param cat - category string
     * @param key - key sequence for the short cut
     * @param readOnly - is the short cut read only and is not editable
     */
    GtShortCutSettingsData(QString const& identifier, QString const& cat,
                           QString const& key, bool readOnly = false);

    /**
     * @brief dataToVariant
     * @return content of the short cut data as QVariant
     */
    QVariant dataToVariant() const;

    /// identification string
    QString id;

    /// category string
    QString category;

    /// short cut key sequence
    QString shortCut;

    /// is the short cut read only an not editable
    bool isReadOnly;

};

#endif // GTSHORTCUTSETTINGSDATA_H
