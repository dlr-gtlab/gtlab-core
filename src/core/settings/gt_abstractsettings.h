/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTSETTINGS_H
#define GTABSTRACTSETTINGS_H

#include <QVariant>

#include "gt_object.h"

class GtSettingsItem;

/**
 * @brief The GtAbstractSettings class
 */
class GtAbstractSettings : public GtObject
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
    virtual ~GtAbstractSettings();

protected:
    /**
     * @brief GtAbstractSettings
     */
    explicit GtAbstractSettings(GtObject* parent = Q_NULLPTR);

    /**
     * @brief registerSetting
     * @param ident - identification string
     * @param initVal - initial value as variant
     * @return the new registered settings item
     */
    GtSettingsItem* registerSetting(const QString& ident,
                                    const QVariant& initVal = QVariant());

private:
    ///
    QList<GtSettingsItem*> m_settings;

};

#endif // GTABSTRACTSETTINGS_H
