/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyreference.h
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GTPROPERTYREFERENCE_H
#define GTPROPERTYREFERENCE_H

#include "gt_datamodel_exports.h"
#include "gt_abstractproperty.h"

#include <QString>


/**
 * @brief A reference to a property inside a GTObject.
 *
 * A property can be either referenced
 *   - by its id "propertyID", or
 *   - if its in a container by its containerID, theEntryID, subPropertyID as
 *     "containerID[{reference-ID}].propertyID"
 *
 *  This class encapsulates the referencing and provides function
 *  to convert from and to string
 */
class GT_DATAMODEL_EXPORT GtPropertyReference
{
public:
    explicit GtPropertyReference(QString propertyId);

    GtPropertyReference(QString containerId, QString entryId,
                        QString propertyID);

    static GtPropertyReference fromString(QString s, bool& ok);
    QString toString() const;

    const QString& propertyId() const;
    const QString& containerId() const;
    const QString& entryId() const;

    /**
     * @brief  Finds the property inside the object
     *
     * Returns nullptr, if property does not exist
     */
    GtAbstractProperty* resolve(GtObject&) const;

private:
    //! The ID of the property, must be non-empty
    QString m_propertyId;

    //! The ID of the container, might be empty along with m_entryID
    QString m_containerId;
    //! The ID of the container entry, might be empty along with m_containerId
    QString m_entryId;
};


#endif // GTPROPERTYREFERENCE_H
