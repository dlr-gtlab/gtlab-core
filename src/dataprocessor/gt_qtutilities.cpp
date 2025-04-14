/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 */

#include "gt_qtutilities.h"

#include <QVariant>

int
gt::qvariantCompare(const QVariant &a, const QVariant &b)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto ordering = QVariant::compare(a, b);

    if (ordering < 0) return -1;
    else if (ordering > 0) return 1;
    else return 0;
#else
    if (a < b) return -1;
    else if (a > b) return 1;
    else return 0;
#endif
}

int
gt::metaTypeId(const QVariant &v)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return v.typeId();
#else
    return static_cast<int>(v.type());
#endif
}

int
gt::metaTypeIdFromName(const char *name)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QMetaType::fromName(name).id();
#else
    return QVariant::nameToType(name);
#endif
}

bool
gt::metaTypeNameIsRegistered(const char *typeName)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QMetaType::fromName(typeName).isValid();
#else
    int typeId = QVariant::nameToType(typeName);
    return typeId != QVariant::Invalid && QMetaType::isRegistered(typeId);
#endif
}
