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
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = 0) Q_DECL_OVERRIDE;

    /**
     * @brief properties
     * @return
     */
    const QList<GtAbstractProperty*>& properties() Q_DECL_OVERRIDE;

    /**
     * @brief registerSubProperty
     * @param property
     */
    void registerSubProperty(GtAbstractProperty& property) Q_DECL_OVERRIDE;

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
    bool modeExists(const QString& mode);

    /**
     * @brief propertyCount
     * @param mode
     * @return
     */
    int propertyCount(const QString& mode);

    /**
     * @brief modes
     * @return
     */
    QStringList modes();

    /**
     * @brief typeProperty - return the ProperttyObject of
     * the mode with the name "mode"
     * @param mode
     * @return
     */
    GtModeTypeProperty* typeProperty(const QString& mode);

private:
    /// temporary propertylist for selected mode type
    QList<GtAbstractProperty*> m_tmpProperties;

    /// current mode type
//    QString m_current;

    /**
     * @brief updateProperties
     */
    void updateProperties();

    /**
     * @brief validateValue
     * @param value
     * @param unit
     * @return
     */
    bool validateValue(const QString& value) Q_DECL_OVERRIDE;

};

#endif // GTMODEPROPERTY_H
