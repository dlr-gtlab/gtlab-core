/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.03.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_shortcutsettingsdata.h"

GtShortCutSettingsData::GtShortCutSettingsData()
{

}

GtShortCutSettingsData::GtShortCutSettingsData(QString const& identifier, QString const& cat,
        QString key, bool readOnly) :
    id(identifier), category(cat),
    shortCut(key), isReadOnly(readOnly)
{

}

QVariant
GtShortCutSettingsData::dataToVariant() const
{
    QString readOnly = "false";
    if (isReadOnly)
    {
        readOnly = "true";
    }

    QStringList l {shortCut, category, readOnly};
    return QVariant(l);
}
