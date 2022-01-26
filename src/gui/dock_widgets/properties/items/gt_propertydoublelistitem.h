/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertydoublelistitem.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 24.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROPERTYDOUBLELISTITEM_H
#define GT_PROPERTYDOUBLELISTITEM_H

#include "gt_propertyitem.h"

class GtDoubleListProperty;

/**
 * @brief The GtDoubleListPropertyItem class
 */
class GtPropertyDoubleListItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtPropertyDoubleListItem();

    /**
     * @brief Returns double list property pointer.
     * @return Pointer to double list property.
     */
    GtDoubleListProperty* doubleListProperty() const;

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const Q_DECL_OVERRIDE;

    /**
     * @brief setData
     * @param column
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(int column,
                         const QVariant& value,
                         GtObject* obj,
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

};

#endif // GT_PROPERTYDOUBLELISTITEM_H
