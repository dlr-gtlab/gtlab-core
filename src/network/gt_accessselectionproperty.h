/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessselectionproperty.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 01.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ACCESSSELECTIONPROPERTY_H
#define GT_ACCESSSELECTIONPROPERTY_H

#include "gt_network_exports.h"

#include "gt_stringproperty.h"

class GtAccessData;

/**
 * @brief The GtAccessSelectionProperty class
 */
class GT_NETWORK_EXPORT GtAccessSelectionProperty : public GtStringProperty
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Property identification string.
     * @param Property name.
     * @param Access identification string.
     * @param Property description.
     */
    GtAccessSelectionProperty(const QString& ident,
                              const QString& name,
                              const QString& accessId,
                              const QString& brief = QString());

    /**
     * @brief Returns access identification string.
     * @return Access identification string.
     */
    const QString& accessId();

    /**
     * @brief Returns access data object.
     * @return Access data object.
     */
    GtAccessData accessData();

private:
    /// Access identification string.
    QString m_accessId;

};

#endif // GT_ACCESSSELECTIONPROPERTY_H
