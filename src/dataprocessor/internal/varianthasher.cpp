#include "varianthasher.h"

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
            static QVariant::Type type_QString =
                QVariant::nameToType("QString");
            static QVariant::Type type_QStringList =
                QVariant::nameToType("QStringList");
            if (variant.type() == type_QString)
            {
                // special case QString: we don't distinguish between empty and null strings!
                // (which the code below does...)
                hash.addData(variant.toString().toUtf8());
            }
            else if (variant.type() == type_QStringList)
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
                hash.addData(bb);
            }
        }


    } // namespace detail
} // namespace gt
