/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTGROUPPROPERTY_H
#define GTGROUPPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"
#include "gt_version.h"

/**
 * @brief The GtGroupProperty class
 */
class GT_DATAMODEL_EXPORT GtGroupProperty : public GtProperty<QString>
{
    Q_OBJECT

public:

    /**
     * @brief Creates a group property
     *
     * @param id    The id of the property
     * @param name  The visible name of the property in the UI
     * @param brief A brief / help text
     */
    GtGroupProperty(const QString& id,
                    const QString& name,
                    const QString& brief);

    /**
     * @brief Creates a group property
     *
     * The construcor used `name` for the property id.
     * Note, that this constructor is deprecated in favour of manual ids.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use constructor with explicit ID")
    GtGroupProperty(const QString& name,
                    const QString& brief);

    /**
     * @brief valueToVariant
     * @return
     */
    QVariant valueToVariant(const QString& unit,
                            bool* success = nullptr) const override;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(const QVariant& val,
                             const QString& unit) override;

    /**
     * @brief registerSubProperty
     * @param property
     */
    void registerSubProperty(GtAbstractProperty& property) override;

private:
    /**
     * @brief generateSummary
     */
    void generateSummary();

private slots:
    /**
     * @brief onSubPropertyChange
     */
    void onSubPropertyChange();

};

#endif // GTGROUPPROPERTY_H
