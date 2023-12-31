/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labelproperty.h
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELPROPERTY_H
#define GTLABELPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"

class GtObject;

/**
 * @brief The GtLabelProperty class
 */
class GT_DATAMODEL_EXPORT GtLabelProperty : public GtStringProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelProperty
     * @param name
     * @param brief
     */
    GtLabelProperty(const QString& ident,
                    const QString& name,
                    const QString& brief,
                    GtObject* parent);

    /**
     * @brief Returns parent object.
     * @return Parent object.
     */
    GtObject* parentObject();

private:
    /// Parent object
    GtObject* m_parentObj;

};

#endif // GTLABELPROPERTY_H
