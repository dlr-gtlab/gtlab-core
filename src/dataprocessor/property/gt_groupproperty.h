/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTGROUPPROPERTY_H
#define GTGROUPPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

/**
 * @brief The GtGroupProperty class
 */
class GT_DATAMODEL_EXPORT GtGroupProperty : public GtProperty<QString>
{
    Q_OBJECT

public:
    /**
     * @brief GtGroupProperty
     */
    GtGroupProperty(const QString& name,
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
     * @brief registerSubProperty
     * @param property
     */
    void registerSubProperty(GtAbstractProperty& property) Q_DECL_OVERRIDE;

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
