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
                        gt::conversion::convert conversion,
                        gt::conversion::isCompatible isCompatible = {});

    template <typename Converter,
              std::enable_if_t<
                  std::is_invocable_v<Converter&, const GtAbstractProperty&,
                                      const GtAbstractProperty&> &&
                      !std::is_same_v<std::decay_t<Converter>,
                                      gt::conversion::convert>,
                  int> = 0>
    GtPropertyConverter(QMetaObject from, QMetaObject to,
                        Converter&& conversion,
                        gt::conversion::isCompatible isCompatible = {}) :
        GtPropertyConverter(
            std::move(from), std::move(to),
            gt::conversion::eraseConverter(std::forward<Converter>(conversion)),
            std::move(isCompatible))
    {
    }

    QString fromClassName() const;

    QString toClassName() const;

    gt::conversion::isCompatible isCompatibletFunc() const;

    gt::conversion::convert convertFunc() const;

    bool isCompatibleDefined() const;

    bool conversionDefined() const;

private:
    QMetaObject m_from;

    QMetaObject m_to;

    gt::conversion::isCompatible m_isCompatibleFunc;

    gt::conversion::convert m_conversion;
};

#endif // GT_PROPERTYCONVERTER_H
