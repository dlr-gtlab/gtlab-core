/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMODEPROPERTY_H
#define GTMODEPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

class GtModeTypeProperty;

/**
 * @brief The GtModeProperty class
 */
class GT_DATAMODEL_EXPORT GtModeProperty : public GtProperty<QString>
{
    Q_OBJECT

public:
    using GtProperty<QString>::operator=;

    GtModeProperty(const QString& ident,
                   const QString& name,
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
     * @brief properties
     * @return
     */
    const QList<GtAbstractProperty*>& properties() const override;

    /**
     * @brief registerSubProperty
     * @param property
     */
    void registerSubProperty(GtAbstractProperty& property) override;

    /**
     * @brief registerSubProperty
     * @param property
     */
    void registerSubProperty(GtModeTypeProperty& property);

    /**
     * @brief modeExists
     * @param mode
     * @return
     */
    bool modeExists(const QString& mode) const;

    /**
     * @brief propertyCount
     * @param mode
     * @return
     */
    int propertyCount(const QString& mode) const;

    /**
     * @brief modes
     * @return
     */
    QStringList modes() const;

    /**
     * @brief typeProperty - return the ProperttyObject of
     * the mode with the name "mode"
     * @param mode
     * @return
     */
    GtModeTypeProperty* typeProperty(const QString& mode);

private:
    /// temporary propertylist for selected mode type
    mutable QList<GtAbstractProperty*> m_tmpProperties;

    /// current mode type
//    QString m_current;

    /**
     * @brief updateProperties
     */
    void updateProperties() const;

    /**
     * @brief validateValue
     * @param value
     * @param unit
     * @return
     */
    bool validateValue(const QString& value) override;

};

namespace gt
{

/**
 * @brief Creates a property factory for bools with a default value
 */
GT_DATAMODEL_EXPORT
gt::PropertyFactoryFunction makeModeProperty();

} // namespace gt

#endif // GTMODEPROPERTY_H
