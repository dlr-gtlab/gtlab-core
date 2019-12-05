/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectioninterface
 * copyright 2009-2017 by DLR
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONINTERFACE_H
#define GT_COLLECTIONINTERFACE_H

#include "gt_mdi_exports.h"

#include <QtPlugin>

/**
 * @brief The GtCollectionInterface class
 */
class GT_MDI_EXPORT GtCollectionInterface
{
public:
    /**
     * @brief Virtual destructor to keep compilers happy.
     */
    virtual ~GtCollectionInterface() {}

    /**
     * @brief Returns collection specific icon.
     * @return Collection spezific icon.
     */
    virtual QIcon collectionIcon() const = 0;

    /**
     * @brief Returns identification string of collection. Same id
     * is used to generate access point data within the netowrk interface.
     * @return Identification string of collection.
     */
    virtual QString collectionId() const = 0;

    /**
     * @brief Returns meta object of GtAbstractCollectionSettings class.
     * @return Meta object of GtAbstractCollectionSettings class.
     */
    virtual QMetaObject collectionSettings() const = 0;

    /**
     * @brief Returns structure of collection items.
     * @return Sturcture of collection items.
     */
    virtual QMap<QString, QMetaType::Type> collectionStructure() const = 0;

};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GtCollectionInterface,
                    "de.dlr.gtlab.GtCollectionInterface/0.1")
QT_END_NAMESPACE

#endif // GT_COLLECTIONINTERFACE_H
