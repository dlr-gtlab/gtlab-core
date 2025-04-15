/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TW)
 */

#include "varianthasher.h"

#include "gt_qtutilities.h"

namespace gt
{
    namespace detail
    {

        VariantHasher::VariantHasher() : buff(&bb), ds(&buff)
        {
            bb.reserve(64);
            buff.open(QIODevice::WriteOnly);
        }

        void VariantHasher::addToHash(QCryptographicHash& hash,
                                      const QVariant& variant)
        {
            auto varType = gt::metaTypeId(variant);

            static auto type_QString =
                gt::metaTypeIdFromName("QString");
            static auto type_QStringList =
                gt::metaTypeIdFromName("QStringList");
            if (varType == type_QString)
            {
                // special case QString: we don't distinguish between empty and null strings!
                // (which the code below does...)
                hash.addData(variant.toString().toUtf8());
            }
            else if (varType == type_QStringList)
            {
                // special case QString: we don't distinguish between empty and null strings!
                // (which the code below does...)
                QStringList strList = variant.toStringList();
                foreach (const QString& s, strList)
                {
                    hash.addData(s.toUtf8());
                }
            }
            else
            {
                buff.seek(0);
                ds << variant;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                hash.addData(bb.constData(), buff.pos());
#else
                hash.addData(QByteArrayView(bb.constData(), buff.pos()));
#endif
            }
        }


    } // namespace detail
} // namespace gt
