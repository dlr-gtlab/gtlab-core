/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 08.10.2026
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GT_PROPERTYCONVERTER_H
#define GT_PROPERTYCONVERTER_H

#include "gt_datamodel_exports.h"
#include "gt_propertyconversion.h"


class GT_DATAMODEL_EXPORT GtPropertyConverter
{
public:

    GtPropertyConverter(QMetaObject from, QMetaObject to,
                        gt::conversion::convert conversion);

    GtPropertyConverter(QMetaObject from, QMetaObject to,
                        gt::conversion::convert conversion,
                        gt::conversion::canConnect canConnect);

    QString fromClassName() const;

    QString toClassName() const;

    gt::conversion::canConnect canConnectFunc() const;

    gt::conversion::convert convertFunc() const;

    bool canConnectDefined() const;

    bool conversionDefined() const;

private:
    QMetaObject m_from;

    QMetaObject m_to;

    gt::conversion::canConnect m_canConnect;

    gt::conversion::convert m_conversion;
};

#endif // GT_PROPERTYCONVERTER_H
