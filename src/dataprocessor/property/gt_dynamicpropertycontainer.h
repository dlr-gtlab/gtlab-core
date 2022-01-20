/* GTlab - Gas Turbine laboratory
 * Source File: gt_structproperty.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDYNAMICPROPERTYCONTAINER_H
#define GTDYNAMICPROPERTYCONTAINER_H

#include "gt_datamodel_exports.h"

#include "gt_abstractproperty.h"

/**
 * @brief The GtDynamicPropertyContainer class
 */
class GT_DATAMODEL_EXPORT GtDynamicPropertyContainer : public GtAbstractProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtDynamicPropertyContainer
     * @param name
     */
    GtDynamicPropertyContainer(const QString& name);

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
    bool setValueFromVariant(const QVariant& val, const QString& unit,
                             bool* success = 0) Q_DECL_OVERRIDE;

    /**
     * @brief registerSubProperty
     * @param property
     */
    virtual void registerSubProperty(
            GtAbstractProperty& property) Q_DECL_OVERRIDE;

    /**
     * @brief deleteSubProperty
     * @param property
     */
    virtual void deleteSubProperty(GtAbstractProperty* property);

};

#endif // GTDYNAMICPROPERTYCONTAINER_H
