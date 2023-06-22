/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TW)
 */

#ifndef GT_INTERNAL_VARIANTHASHER_H
#define GT_INTERNAL_VARIANTHASHER_H

#include "gt_datamodel_exports.h"

#include <QCryptographicHash>
#include <QBuffer>
#include <QDataStream>
#include <QVariant>

namespace gt
{
    namespace detail
    {

        /**
         * @brief Helper class to compute hashes from QVariants
         */
        class VariantHasher
        {
        public:
            GT_DATAMODEL_EXPORT VariantHasher();
            GT_DATAMODEL_EXPORT void addToHash(QCryptographicHash& hash,
                                               const QVariant& variant);

        private:
            QByteArray bb;
            QBuffer buff;
            QDataStream ds;
        };

    } // namespace detail
} // namespace gt


#endif // GT_INTERNAL_VARIANTHASHER_H
