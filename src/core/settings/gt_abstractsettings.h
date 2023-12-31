/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTSETTINGS_H
#define GTABSTRACTSETTINGS_H

#include "gt_core_exports.h"

#include <QVariant>

#include "gt_object.h"

class GtSettingsItem;

/**
 * @brief The GtAbstractSettings class
 */
class GT_CORE_EXPORT GtAbstractSettings : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief restoreSettings
     */
    void restoreSettings();

    /**
     * @brief ~GtAbstractSettings
     */
    ~GtAbstractSettings() override;

    /**
     * @brief Registers a setting at the framework
     *
     * @param ident - The identifier of the setting e.g. "mymodule/myvalue"
     * @param initVal - initial value as variant
     * @return the new registered settings item
     */
    GtSettingsItem* registerSetting(const QString& ident,
                                    const QVariant& initVal = QVariant());


    /**
     * @brief Registers a setting at the framework that requires an app restart
     *
     * @param ident - The identifier of the setting e.g. "mymodule/myvalue"
     * @param initVal - initial value as variant
     * @return the new registered settings item
     */
    GtSettingsItem* registerSettingRestart(const QString& ident,
                                           const QVariant& initVal = QVariant());

    /**
     * @brief Sets the value of a setting
     * @param ident The identifier of the setting e.g. "mymodule/myvalue"
     * @param value The value to set
     */
    void setSetting(const QString& ident, const QVariant& value);

    /**
     * @brief Retrieves the value of the setting
     * @param ident The identifier of the setting e.g. "mymodule/myvalue"
     * @return
     */
    QVariant getSetting(const QString& ident) const;

    /**
     * @brief Returns true if the setting "ident" exists
     * @param ident Ident of setting
     * @return Whether the setting exists
     */
    bool hasSetting(const QString& ident) const;

    /**
     * @brief Returns a list of all setting ids
     * @return setting ids
     */
    QStringList getAllSettingIds() const;

    /**
     * @brief GtAbstractSettings
     */
    explicit GtAbstractSettings(GtObject* parent = nullptr);

    /**
     * @brief Check, whether the app requires a restart
     *
     * If a change of a certain setting requires an app restart,
     * this function will return true.
     */
    bool requiresAppRestart() const;

private:

    /// Map of settings
    QMap<QString, GtSettingsItem*> m_settings;
};

#endif // GTABSTRACTSETTINGS_H
