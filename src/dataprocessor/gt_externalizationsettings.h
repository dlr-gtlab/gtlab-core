/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizationsettings.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 04.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDSETTINGS_H
#define GTEXTERNALIZEDSETTINGS_H

#include "gt_object.h"

#include <QVariant>

#define gtExternalizationSettings (GtExternalizationSettings::instance())

class GtObject;
/**
 * @brief The GtExternalizationSettings class helper class to set settings
 * specific to the externalization
 */
class GT_DATAMODEL_EXPORT  GtExternalizationSettings : public GtObject
{
    Q_OBJECT

public:

    /**
     * @brief instance
     * @return singleton instance
     */
    static GtExternalizationSettings* instance();

    /**
     * @brief isExternalizationEnabled
     * @return whether to use the externalization feature
     */
    bool isExternalizationEnabled() const;

    /**
     * @brief autoExternalizeOnSave
     * @return whether to automatically externalize all fetched datasets when
     * saving a project
     */
    bool autoExternalizeOnSave() const;

public slots:

    /**
     * @brief onEnbaleExternalizationChanged setter for externalization feature
     * @param var
     */
    void onEnbaleExternalizationChanged(QVariant var);

    /**
     * @brief onAutoExternalizationChanged
     * @param var
     */
    void onAutoExternalizationChanged(QVariant var);

private:

    /// whether to automatically externalize or fetch all data when saving
    bool m_autoExternalize;
    /// whether to use the externalization feature
    bool m_enableExternalization;

    /**
     * @brief GtExternalizedSettings
     */
    GtExternalizationSettings();
};

#endif // GTEXTERNALIZEDSETTINGS_H
