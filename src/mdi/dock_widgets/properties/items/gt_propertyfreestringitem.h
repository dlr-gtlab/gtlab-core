/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfreestringitem.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 20.11.2018
 *  Author: Björn Schneider (AT-TWK)
 *  Tel.: +49 2203 601 2642
 */


#ifndef GTPROPERTYFREESTRINGITEM_H
#define GTPROPERTYFREESTRINGITEM_H

#include "gt_propertyitem.h"

/**
 * @brief The GtPropertyFreeStringItem class
 */
class GtPropertyFreeStringItem: public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyFreeStringItem constructor
     */
    Q_INVOKABLE GtPropertyFreeStringItem();

    /**
     * @brief editorWidget
     * @param parent
     * @param delegate
     * @return QWidget pointer
     */
    virtual QWidget* editorWidget(
            QWidget* parent, const GtPropertyValueDelegate* delegate)
    const Q_DECL_OVERRIDE;
};

#endif // GTPROPERTYFREESTRINGITEM_H
