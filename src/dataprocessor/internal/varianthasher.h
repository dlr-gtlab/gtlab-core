#ifndef VARIANTHASHER_H
#define VARIANTHASHER_H

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


#endif // VARIANTHASHER_H