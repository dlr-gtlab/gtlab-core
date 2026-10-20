/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GTPROPERTYCONFIGURATION_H
#define GTPROPERTYCONFIGURATION_H

#include <QVariantMap>

namespace gt
{
    /** Maximum nested value depth in serialized property configuration. */
    inline constexpr int PropertyConfigurationMaxDepth = 64;

    /**
     * JSON-like construction settings for a property.
     *
     * This is an alias so modules can use the named API type without registering
     * a custom Qt metatype.
     */
    using PropertyConfiguration = QVariantMap;

} // namespace gt

#endif // GTPROPERTYCONFIGURATION_H
